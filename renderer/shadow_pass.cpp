#include "shadow_pass.h"
#include "../components/transform.h"
#include "../components/mesh_renderer.h"
#include <glad/glad.h>

void ShadowPass::render(std::list<Entity>& objects)
{
    updateLightMatrices();

    for (auto& entity : objects)
    {
        renderEntity(entity);

        for (auto& childEntity : entity.getChildren())
        {
            renderEntity(childEntity);
        }
    }
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
    material->shader->use();

    // set object uniforms (e.g. transform)
    material->shader->setMat4("u_model", transform->modelMatrix());
    material->shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

    material->setUniformData();

    glBindVertexArray(mesh->vao());
    glDrawElements(GL_TRIANGLES, (int) mesh->mesh->indicesSize(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void ShadowPass::shadowMaterial(Material *mat)
{
    m_shadowMat = mat;
}

void ShadowPass::mainLight(Entity *entity)
{
    m_mainLight = entity;
}
