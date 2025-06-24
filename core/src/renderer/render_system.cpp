#include "render_system.h"

#include "../components/components.h"
#include "../irradiance_map_factory.h"
#include "glad/gl.h"
#include "../logger.h"

#include <random>
#include <array>
#include <format>

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void RenderSystem::init(int width, int height, ecs::Coordinator* coordinator, 
        Camera* camera, AssetManager* assetManager)
{
    m_width = width;
    m_height = height;
    m_coordinator = coordinator;
    m_camera = camera;
    m_assetManager = assetManager;

    m_mainTargetFrameBuffer = std::make_unique<ColorDepthStencilBuffer>(width, height, m_gpurm);
    m_shadowFrameBuffer = std::make_unique<DepthBuffer>(shadowMapSize, shadowMapSize, m_gpurm);
    m_gbuffer = std::make_unique<GBuffer>(width, height, m_gpurm);
    m_ssaoBuffer = std::make_unique<ColorBuffer>(width, height, m_gpurm);
    m_ssaoBlurBuffer = std::make_unique<ColorBuffer>(width, height, m_gpurm);

    // TODO: Move this to asset initalization
    Shader s = m_assetManager->getShader("resources/shaders/gbuffer.glsl");
    m_assetManager->createMaterial("GBuffer", s);
    s = m_assetManager->getShader("resources/shaders/lightingpass.glsl");
    m_assetManager->createMaterial("LightingPass", s);
    s = m_assetManager->getShader("resources/shaders/simple_shadow_depth.glsl");
    m_assetManager->createMaterial("shadowMat", s);
    s = m_assetManager->getShader("resources/shaders/ssao.glsl");
    m_assetManager->createMaterial("ssaoMat", s);
    s = m_assetManager->getShader("resources/shaders/ssao_blur.glsl");
    m_assetManager->createMaterial("ssaoBlurMat", s);
    generateSSAONoiseTexture();
    s = m_assetManager->getShader("resources/shaders/normal_vector.glsl");
    m_assetManager->createMaterial("normalVector", s);
}

void RenderSystem::update()
{
    shadowPass();
    geometryPass();
    if (m_ssaoEnabled) {
        ssaoPass();
        ssaoBlurPass();
    }
    lightingPass();

    // copy depth buffer from geometry pass
    m_gbuffer->bind(FrameBufferOp::READ);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gbuffer->getDepthAttachmentID(), 0);
    m_mainTargetFrameBuffer->bind(FrameBufferOp::WRITE); 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_mainTargetFrameBuffer->getDepthAttachmentID(), 0);
    gpucommands.blit(m_width, m_height, ClearMask::DEPTH, Filtering::POINT);

    skyboxPass();
    //normalVisualizerPass();
}

void RenderSystem::updateIrradianceMaps(AssetManager& assetManager)
{
    IrradianceMaps maps = IrradianceMapFactory::generateIrradianceMapsFromHDR(assetManager.getTextureHDR("resources/textures/skybox/tiber_2.hdr").handle, assetManager);
    m_irradianceMap = maps.irradianceMap;
    m_prefilterMap = maps.prefilterMap;
    m_brdfLUT = maps.brdfLUTMap;
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

void RenderSystem::toggleSSAO(bool enabled)
{
    m_ssaoEnabled = enabled;
}

id_t RenderSystem::getFinalRenderTexID()
{
    return m_mainTargetFrameBuffer->getColorAttachmentID();
}

void RenderSystem::shadowPass()
{
    m_shadowFrameBuffer->bind();
    gpucommands.setViewportSize(0, 0, shadowMapSize, shadowMapSize);
    gpucommands.clear(ClearMask::DEPTH);

    for (auto entity : m_entities)
    {
        auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
        auto& meshRenderer = m_coordinator->getComponent<ecs::MeshRenderer>(
                entity);

        Material material = m_assetManager->getMaterial("shadowMat");
        id_t shader = material.shader.handle;
        m_gpurm.bindShader(shader);

        // set object uniforms (e.g. transform)
        m_gpurm.setUniform(shader, "u_model", transform.modelMatrix);
        for (auto& kv : material.uniforms.floatValues) {
            m_gpurm.setUniform(shader, kv.first.c_str(), kv.second);
        }
        for (auto& kv : material.uniforms.colorValues) {
            m_gpurm.setUniform(shader, kv.first.c_str(), kv.second);
        }

        glBindVertexArray(meshRenderer.mesh.handle);
        glDrawElements(GL_TRIANGLES, (int) meshRenderer.mesh.indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    m_shadowFrameBuffer->unbind();
}

void RenderSystem::geometryPass()
{
    m_gbuffer->bind();
    gpucommands.setViewportSize(0, 0, m_width, m_height);
    gpucommands.setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gpucommands.clear(ClearMask::COLORDEPTH);

    for (auto entity : m_entities)
    {
        auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
        auto& meshRenderer = m_coordinator->getComponent<ecs::MeshRenderer>(
                entity);

        Material &origMaterial = *meshRenderer.material;
        Material &material = m_assetManager->getMaterial("GBuffer");
        id_t shader = material.shader.handle;
        m_gpurm.bindShader(shader);

        m_gpurm.setUniform(shader, "u_model", transform.modelMatrix);
        m_gpurm.setUniform(shader, "u_normalMatrix", glm::transpose(
            glm::inverse(glm::mat3(transform.modelMatrix))));

        for (auto& kv : origMaterial.uniforms.floatValues) {
            m_gpurm.setUniform(shader, kv.first.c_str(), kv.second);
        }

        for (auto& kv : origMaterial.uniforms.colorValues) {
            m_gpurm.setUniform(shader, kv.first.c_str(), kv.second);
        }

        int texCount = 0;
        for (auto& kv : origMaterial.uniforms.texValues) {
            m_gpurm.setUniform(shader, kv.first.c_str(), texCount);
            glActiveTexture(GL_TEXTURE0 + texCount);
            glBindTexture(GL_TEXTURE_2D, kv.second);
            ++texCount;
        }

        glBindVertexArray(meshRenderer.mesh.handle);
        glDrawElements(GL_TRIANGLES, (int) meshRenderer.mesh.indexCount, GL_UNSIGNED_INT, nullptr);
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

    Mesh mesh = m_assetManager->getMesh(MeshType::Quad);
    Material material = m_assetManager->getMaterial("ssaoMat");
    id_t shader = material.shader.handle;
    m_gpurm.bindShader(shader);

    for (unsigned int i = 0; i < 64; ++i) {
        m_gpurm.setUniform(shader, std::format("u_samples[{}]", i).c_str(), m_ssaoKernel[i]);
    }

    m_gpurm.setUniform(shader, "u_depth", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getDepthAttachmentID());

    m_gpurm.setUniform(shader, "u_normal", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getNormalAttachmentID());

    m_gpurm.setUniform(shader, "u_noise", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_ssaoNoiseTex);

    glBindVertexArray(mesh.handle);
    glDrawElements(GL_TRIANGLES, (int) mesh.indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    m_ssaoBuffer->unbind();
}

void RenderSystem::ssaoBlurPass()
{
    m_ssaoBlurBuffer->bind();
    gpucommands.setViewportSize(0, 0, m_width, m_height);
    gpucommands.setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gpucommands.clear(ClearMask::COLOR);

    Mesh mesh = m_assetManager->getMesh(MeshType::Quad);
    Material material = m_assetManager->getMaterial("ssaoBlurMat");
    id_t shader = material.shader.handle;
    m_gpurm.bindShader(shader);

    for (unsigned int i = 0; i < 64; ++i) {
        m_gpurm.setUniform(shader, std::format("u_samples[{}]", i).c_str(), m_ssaoKernel[i]);
    }

    m_gpurm.setUniform(shader, "u_ssao", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ssaoBuffer->getColorAttachmentID());

    glBindVertexArray(mesh.handle);
    glDrawElements(GL_TRIANGLES, (int) mesh.indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    m_ssaoBuffer->unbind();
}

void RenderSystem::lightingPass()
{
    m_mainTargetFrameBuffer->bind();
    gpucommands.setViewportSize(0, 0, m_width, m_height);
    gpucommands.setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gpucommands.clear(ClearMask::COLORDEPTH);

    Mesh mesh = m_assetManager->getMesh(MeshType::Quad);
    Material material = m_assetManager->getMaterial("LightingPass");
    id_t shader = material.shader.handle;
    m_gpurm.bindShader(shader);

    m_gpurm.setUniform(shader, "u_depth", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getDepthAttachmentID());

    m_gpurm.setUniform(shader, "u_normal", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getNormalAttachmentID());

    m_gpurm.setUniform(shader, "u_albedo", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getAlbedoSpecAttachmentID());

    m_gpurm.setUniform(shader, "u_shadowMap", 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_shadowFrameBuffer->getDepthAttachmentID());

    m_gpurm.setUniform(shader, "u_ssao", 4);
    glActiveTexture(GL_TEXTURE4);
    if (m_ssaoEnabled)
        glBindTexture(GL_TEXTURE_2D, m_ssaoBlurBuffer->getColorAttachmentID());
    else
    {
        glBindTexture(GL_TEXTURE_2D, m_assetManager->getTexture2D(
            "resources/textures/default_white.png").handle);
    }

    m_gpurm.setUniform(shader, "u_orm", 5);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getPositionAttachmentID());

    // bind global illumination textures
    m_gpurm.setUniform(shader, "u_irradianceTex", 10);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);

    m_gpurm.setUniform(shader, "u_prefilterMap", 11);
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap);

    m_gpurm.setUniform(shader, "u_brdfLUT", 12);
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, m_brdfLUT);

    glBindVertexArray(mesh.handle);
    glDrawElements(GL_TRIANGLES, (int) mesh.indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    m_mainTargetFrameBuffer->unbind();
}

void RenderSystem::skyboxPass()
{
    m_mainTargetFrameBuffer->bind();

    Mesh mesh = m_assetManager->getMesh(MeshType::TriangleMap);
    Material material = m_assetManager->getMaterial("skyboxMat");
    id_t shader = material.shader.handle;
    m_gpurm.bindShader(shader);

    int texCount = 0;
    for (auto& kv : material.uniforms.texValues) {
        m_gpurm.setUniform(shader, kv.first.c_str(), texCount);
        glActiveTexture(GL_TEXTURE0 + texCount);
        glBindTexture(GL_TEXTURE_CUBE_MAP, kv.second);
        ++texCount;
    }

    glBindVertexArray(mesh.handle);
    glDrawElements(GL_TRIANGLES, (int)mesh.indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    m_mainTargetFrameBuffer->unbind();
}

void RenderSystem::normalVisualizerPass()
{
    m_mainTargetFrameBuffer->bind();

    for (auto entity : m_entities)
    {
        auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
        auto& meshRenderer = m_coordinator->getComponent<ecs::MeshRenderer>(
                entity);

        Material material = m_assetManager->getMaterial("normalVector");
        id_t shader = material.shader.handle;
        m_gpurm.bindShader(shader);

        m_gpurm.setUniform(shader, "u_model", transform.modelMatrix);
        m_gpurm.setUniform(shader, "u_normalMatrix", glm::transpose(
            glm::inverse(glm::mat3(m_camera->getViewMatrix() *
                    transform.modelMatrix))));

        for (auto& kv : material.uniforms.floatValues) {
            m_gpurm.setUniform(shader, kv.first.c_str(), kv.second);
        }

        for (auto& kv : material.uniforms.colorValues) {
            m_gpurm.setUniform(shader, kv.first.c_str(), kv.second);
        }

        int texCount = 0;
        for (auto& kv : material.uniforms.texValues) {
            m_gpurm.setUniform(shader, kv.first.c_str(), texCount);
            glActiveTexture(GL_TEXTURE0 + texCount);
            glBindTexture(GL_TEXTURE_2D, kv.second);
            ++texCount;
        }

        glBindVertexArray(meshRenderer.mesh.handle);
        glDrawElements(GL_TRIANGLES, (int) meshRenderer.mesh.indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

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
