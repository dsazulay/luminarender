#include "shadow_pass.h"

#include "../components/transform.h"
#include "../components/mesh_renderer.h"
#include "../assets/asset_library.h"
#include "../assets/material.h"

#include <glad/glad.h>
/*
ShadowPass::ShadowPass()
{
    Shader* s = AssetLibrary::instance().loadShader("simpleShadowMap", "resources/shaders/simple_shadow_depth.glsl");
    m_shadowMat = AssetLibrary::instance().createMaterial("shadowMat", s);
}

void ShadowPass::render(Scene& scene)
{
    updateLightMatrices(scene.mainLight());

    for (auto& entity : scene.objects())
    {
        renderEntity(*entity);
    }
}

void ShadowPass::updateLightMatrices(Entity* mainLight)
{
    auto t = mainLight->getComponent<Transform>();

    float near_plane = 0.1f, far_plane = 40.0f;
    glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(t->getDirection() * -10.f, t->getDirection(), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
}

void ShadowPass::renderEntity(Entity &entity)
{
    auto transform = entity.getComponent<Transform>();
    auto mesh = entity.getComponent<MeshRenderer>();
    if (mesh != nullptr && mesh->mesh != nullptr && mesh->material != nullptr)
    {
        Material *material = m_shadowMat;
        material->shader->use();

        // set object uniforms (e.g. transform)
        material->shader->setMat4("u_model", transform->modelMatrix());
        material->shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

        material->setUniformData();

        glBindVertexArray(mesh->vao());
        glDrawElements(GL_TRIANGLES, (int) mesh->indicesCount(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    for (auto& childEntity : entity.getChildren())
    {
        renderEntity(*childEntity);
    }
}
*/
