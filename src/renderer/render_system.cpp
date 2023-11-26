#include "render_system.h"

#include "../components/components.h"
#include "../assets/shader.h"
#include "../assets/asset_library.h"
#include "../log.h"

#include <random>
#include <array>

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}  

void RenderSystem::init(int width, int height)
{
    m_width = width;
    m_height = height;
    m_mainTargetFrameBuffer = std::make_unique<ColorDepthStencilBuffer>(width, height, m_gpurm);
    m_shadowFrameBuffer = std::make_unique<DepthBuffer>(shadowMapSize, shadowMapSize, m_gpurm);
    m_gbuffer = std::make_unique<GBuffer>(width, height, m_gpurm);
    m_ssaoBuffer = std::make_unique<ColorBuffer>(width, height, m_gpurm);
    m_ssaoBlurBuffer = std::make_unique<ColorBuffer>(width, height, m_gpurm);

    // TODO: Move this to asset initalization
    Shader* s = AssetLibrary::instance().loadShader("GBuffer", "resources/shaders/gbuffer.glsl");
    AssetLibrary::instance().createMaterial("GBuffer", s);
    s = AssetLibrary::instance().loadShader("LightingPass", "resources/shaders/lightingpass.glsl");
    AssetLibrary::instance().createMaterial("LightingPass", s);
    s = AssetLibrary::instance().loadShader("simpleShadowMap", "resources/shaders/simple_shadow_depth.glsl");
    AssetLibrary::instance().createMaterial("shadowMat", s);
    s = AssetLibrary::instance().loadShader("ssao", "resources/shaders/ssao.glsl");
    AssetLibrary::instance().createMaterial("ssaoMat", s);
    s = AssetLibrary::instance().loadShader("ssaoBlur", "resources/shaders/ssao_blur.glsl");
    AssetLibrary::instance().createMaterial("ssaoBlurMat", s);
    generateSSAONoiseTexture();
}

void RenderSystem::update(ecs::Coordinator* coordinator)
{
    shadowPass(coordinator);
    geometryPass(coordinator);
    ssaoPass();
    ssaoBlurPass();
    lightingPass();

    // copy depth buffer from geometry pass
    m_gbuffer->bind(FrameBufferOp::READ);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gbuffer->getDepthAttachmentID(), 0);
    m_mainTargetFrameBuffer->bind(FrameBufferOp::WRITE); 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_mainTargetFrameBuffer->getDepthAttachmentID(), 0);
    gpucommands.blit(m_width, m_height, ClearMask::DEPTH, Filtering::POINT);

    skyboxPass();
}

void RenderSystem::resizeBuffers(int width, int height)
{
    m_width = width;
    m_height = height;
    m_mainTargetFrameBuffer->resizeBuffer(width, height);
    m_gbuffer->resizeBuffer(width, height);
    m_ssaoBuffer->resizeBuffer(width, height);
    m_ssaoBlurBuffer->resizeBuffer(width, height);
}

id_t RenderSystem::getFinalRenderTexID()
{
    return m_mainTargetFrameBuffer->getColorAttachmentID();
}

void RenderSystem::shadowPass(ecs::Coordinator* coordinator)
{
    m_shadowFrameBuffer->bind();
    gpucommands.setViewportSize(0, 0, shadowMapSize, shadowMapSize);
    gpucommands.clear(ClearMask::DEPTH);
    
    for (auto entity : m_entities)
    {
        auto& transform = coordinator->getComponent<ecs::Transform>(entity);
        auto& meshRenderer = coordinator->getComponent<ecs::MeshRenderer>(entity);

        Material *material = AssetLibrary::instance().getMaterial("shadowMat");
        material->shader->use();

        // set object uniforms (e.g. transform)
        material->shader->setMat4("u_model", transform.modelMatrix);
        material->setUniformData();

        glBindVertexArray(meshRenderer.mesh->vao());
        glDrawElements(GL_TRIANGLES, (int) meshRenderer.mesh->indicesCount(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    m_shadowFrameBuffer->unbind();
}

void RenderSystem::geometryPass(ecs::Coordinator* coordinator)
{
    m_gbuffer->bind();
    gpucommands.setViewportSize(0, 0, m_width, m_height);
    gpucommands.setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gpucommands.clear(ClearMask::COLORDEPTH);

    for (auto entity : m_entities)
    {
        auto& transform = coordinator->getComponent<ecs::Transform>(entity);
        auto& meshRenderer = coordinator->getComponent<ecs::MeshRenderer>(entity);

        Material* origMaterial = meshRenderer.material;
        Material* material = AssetLibrary::instance().getMaterial("GBuffer");
        material->shader->use();

        material->shader->setMat4("u_model", transform.modelMatrix);
        material->shader->setMat3("u_normalMatrix", glm::transpose(
            glm::inverse(glm::mat3(transform.modelMatrix))));

        int texCount = 0;
        for (const auto &texture: origMaterial->textures)
        {
            material->shader->setInt(texture.first, texCount);
            glActiveTexture(GL_TEXTURE0 + texCount);
            glBindTexture(GL_TEXTURE_2D, texture.second);
            texCount++;
        }

        glm::vec4 color = std::any_cast<glm::vec4>(origMaterial->getUniformData()
                .find("u_color")->second);
        material->setProperty("u_color", color);
        material->setUniformData();

        glBindVertexArray(meshRenderer.mesh->vao());
        glDrawElements(GL_TRIANGLES, (int) meshRenderer.mesh->indicesCount(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    m_gbuffer->unbind();
}

void RenderSystem::ssaoPass()
{
    m_ssaoBuffer->bind();
    gpucommands.setViewportSize(0, 0, m_width, m_height);
    gpucommands.setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gpucommands.clear(ClearMask::COLOR);

    Mesh* mesh = AssetLibrary::instance().getMesh("quad");
    Material* material = AssetLibrary::instance().getMaterial("ssaoMat");
    material->shader->use();

    for (unsigned int i = 0; i < 64; ++i)
        material->shader->setVec3("u_samples[" + std::to_string(i) + "]", m_ssaoKernel[i]);

    material->shader->setInt("u_depth", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getDepthAttachmentID());

    material->shader->setInt("u_normal", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getNormalAttachmentID());

    material->shader->setInt("u_noise", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_ssaoNoiseTex);

    glBindVertexArray(mesh->vao());
    glDrawElements(GL_TRIANGLES, (int) mesh->indicesCount(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    m_ssaoBuffer->unbind();
}

void RenderSystem::ssaoBlurPass()
{
    m_ssaoBlurBuffer->bind();
    gpucommands.setViewportSize(0, 0, m_width, m_height);
    gpucommands.setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gpucommands.clear(ClearMask::COLOR);

    Mesh* mesh = AssetLibrary::instance().getMesh("quad");
    Material* material = AssetLibrary::instance().getMaterial("ssaoBlurMat");
    material->shader->use();

    for (unsigned int i = 0; i < 64; ++i)
        material->shader->setVec3("u_samples[" + std::to_string(i) + "]", m_ssaoKernel[i]);

    material->shader->setInt("u_ssao", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ssaoBuffer->getColorAttachmentID());

    glBindVertexArray(mesh->vao());
    glDrawElements(GL_TRIANGLES, (int) mesh->indicesCount(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    m_ssaoBuffer->unbind();
}

void RenderSystem::lightingPass()
{
    m_mainTargetFrameBuffer->bind();
    gpucommands.setViewportSize(0, 0, m_width, m_height);
    gpucommands.setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gpucommands.clear(ClearMask::COLORDEPTH);

    Mesh* mesh = AssetLibrary::instance().getMesh("quad");
    Material* material = AssetLibrary::instance().getMaterial("LightingPass");
    material->shader->use();

    material->shader->setInt("u_depth", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getDepthAttachmentID());

    material->shader->setInt("u_normal", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getNormalAttachmentID());

    material->shader->setInt("u_albedospec", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getAlbedoSpecAttachmentID());

    material->shader->setInt("u_shadowMap", 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_shadowFrameBuffer->getDepthAttachmentID());

    material->shader->setInt("u_ssao", 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_ssaoBlurBuffer->getColorAttachmentID());

    material->shader->setInt("u_gposition", 5);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getPositionAttachmentID());


    glBindVertexArray(mesh->vao());
    glDrawElements(GL_TRIANGLES, (int) mesh->indicesCount(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    m_mainTargetFrameBuffer->unbind();
}

void RenderSystem::skyboxPass()
{
    m_mainTargetFrameBuffer->bind();
    
    auto mesh = AssetLibrary::instance().getMesh("triangleMap");
    Material* material = AssetLibrary::instance().getMaterial("skyboxMat");
    material->shader->use();

    int texCount = 0;
    for (const auto& texture : material->textures)
    {
        material->shader->setInt(texture.first, texCount);
        glActiveTexture(GL_TEXTURE0 + texCount);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture.second);
    }

    glBindVertexArray(mesh->vao());
    glDrawElements(GL_TRIANGLES, (int)mesh->indicesCount(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    m_mainTargetFrameBuffer->unbind();
}

// TODO: move this to asset library init
void RenderSystem::generateSSAONoiseTexture()
{
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        m_ssaoKernel[i] = sample;
    }

    std::array<glm::vec3, 16> ssaoNoise;
    for (unsigned int i = 0; i < 16; ++i)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise[i] = noise;
    }

    m_ssaoNoiseTex = m_gpurm.createTexture({
        .width = 4,
        .height = 4,
        .format = Format::RGB,
        .byteFormat = ByteFormat::RGBA32F,
        .filtering = Filtering::POINT,
        .type = TexType::FLOAT,
        .initialData = ssaoNoise.data(),
    });
}
