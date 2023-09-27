#include "render_system.h"

#include "../components/components.h"
#include "../assets/shader.h"
#include "../assets/asset_library.h"
#include "../log.h"

void RenderSystem::init(int width, int height)
{
    m_width = width;
    m_height = height;
    m_mainTargetFrameBuffer = std::make_unique<ColorDepthStencilBuffer>(width, height, m_gpurm);
    m_shadowFrameBuffer = std::make_unique<DepthBuffer>(2048, 2048, m_gpurm);
    m_gbuffer = std::make_unique<GBuffer>(width, height, m_gpurm);

    // TODO: Move this to asset initalization
    Shader* s = AssetLibrary::instance().loadShader("GBuffer", "resources/shaders/gbuffer.glsl");
    AssetLibrary::instance().createMaterial("GBuffer", s);
    s = AssetLibrary::instance().loadShader("LightingPass", "resources/shaders/lightingpass.glsl");
    AssetLibrary::instance().createMaterial("LightingPass", s);
    s = AssetLibrary::instance().loadShader("simpleShadowMap", "resources/shaders/simple_shadow_depth.glsl");
    AssetLibrary::instance().createMaterial("shadowMat", s);
}

void RenderSystem::update(ecs::Coordinator& coordinator)
{
    shadowPass(coordinator);
    geometryPass(coordinator);
    lightingPass();

    // copy depth buffer from geometry pass
    m_gbuffer->bind(FrameBufferOp::READ);
    m_mainTargetFrameBuffer->bind(FrameBufferOp::WRITE); 
    gpucommands.blit(m_width, m_height, ClearMask::DEPTH, Filtering::POINT);

    skyboxPass();
}

void RenderSystem::resizeBuffers(int width, int height)
{
    m_width = width;
    m_height = height;
    m_mainTargetFrameBuffer->resizeBuffer(width, height);
    m_gbuffer->resizeBuffer(width, height);
}

id_t RenderSystem::getFinalRenderTexID()
{
    return m_mainTargetFrameBuffer->getColorAttachmentID();
}

void RenderSystem::shadowPass(ecs::Coordinator& coordinator)
{
    m_shadowFrameBuffer->bind();
    gpucommands.setViewportSize(0, 0, 2048, 2048);
    gpucommands.clear(ClearMask::DEPTH);
    
    for (auto entity : m_entities)
    {
        auto& transform = coordinator.getComponent<ecs::Transform>(entity);
        auto& meshRenderer = coordinator.getComponent<ecs::MeshRenderer>(entity);

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

void RenderSystem::geometryPass(ecs::Coordinator& coordinator)
{
    m_gbuffer->bind();
    gpucommands.setViewportSize(0, 0, m_width, m_height);
    gpucommands.setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    gpucommands.clear(ClearMask::COLORDEPTH);

    for (auto entity : m_entities)
    {
        auto& transform = coordinator.getComponent<ecs::Transform>(entity);
        auto& meshRenderer = coordinator.getComponent<ecs::MeshRenderer>(entity);

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

void RenderSystem::lightingPass()
{
    m_mainTargetFrameBuffer->bind();
    gpucommands.setViewportSize(0, 0, m_width, m_height);
    gpucommands.setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    gpucommands.clear(ClearMask::COLORDEPTH);

    Mesh* mesh = AssetLibrary::instance().getMesh("quad");
    Material* material = AssetLibrary::instance().getMaterial("LightingPass");
    material->shader->use();

    material->shader->setInt("u_gposition", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getPositionAttachmentID());

    material->shader->setInt("u_normal", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getNormalAttachmentID());

    material->shader->setInt("u_albedospec", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->getAlbedoSpecAttachmentID());

    material->shader->setInt("u_shadowMap", 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_shadowFrameBuffer->getDepthAttachmentID());

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
