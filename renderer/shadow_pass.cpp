#include "shadow_pass.h"
#include "../components/transform.h"
#include "../components/mesh_renderer.h"
#include <glad/glad.h>

void ShadowPass::render(std::list<Entity>& objects)
{
    updateLightMatrices();
    updateViewportDimensions();
    bindFrameBuffer();
    clearFrameBuffer();

    for (auto& entity : objects)
    {
        renderEntity(entity);

        for (auto& childEntity : entity.getChildren())
        {
            renderEntity(childEntity);
        }
    }

    unbindFrameBuffer();
}

void ShadowPass::clearFrameBuffer()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowPass::updateLightMatrices()
{
    auto t = m_mainLight->getComponent<Transform>();

    float near_plane = 0.1f, far_plane = 40.0f;
    glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(t->getDirection() * -10.f, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
}

void ShadowPass::renderEntity(Entity &entity)
{
    auto transform = entity.getComponent<Transform>();
    auto mesh = entity.getComponent<MeshRenderer>();
    if (mesh == nullptr)
        return;
    Material *material = m_shadowMat;
    Material *originalMat = mesh->material;

    // TODO: use the same system as textures for global illumination (HDR textures)
//    originalMat->setTexture("u_mainTex", m_frameBuffer.getTexcolorBufferID(), 0);
    originalMat->setTexture("u_shadowMap", m_frameBuffer.getTexcolorBufferID(), 0);
    material->shader->use();

    // set object uniforms (e.g. transform)
    material->shader->setMat4("u_model", transform->modelMatrix());
    material->shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

    material->setUniformData();

    glBindVertexArray(mesh->vao());
    glDrawElements(GL_TRIANGLES, (int) mesh->mesh->indicesSize(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void ShadowPass::init()
{

}

void ShadowPass::resizeFrameBuffer(int width, int height)
{
    m_viewportWidth = width;
    m_viewportHeight = height;
    m_frameBuffer.resizeBuffer(m_viewportWidth, m_viewportHeight);
}

ShadowPass::ShadowPass(int width, int height, FrameBuffer::Type type) : RenderSystem(width, height, type)
{

}

void ShadowPass::shadowMaterial(Material *mat)
{
    m_shadowMat = mat;
}

void ShadowPass::mainLight(Entity *entity)
{
    m_mainLight = entity;
}
