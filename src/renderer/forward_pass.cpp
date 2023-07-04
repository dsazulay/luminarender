#include "forward_pass.h"
#include "../components/transform.h"
#include "../components/mesh_renderer.h"
#include "../assets/material.h"
#include <glad/glad.h>

void ForwardPass::render(Scene& scene)
{
    for (auto& entity : scene.objects())
    {
        renderEntity(*entity, scene.mainLight());
    }

    if (scene.hasSkybox())
        renderSkybox(scene.skybox());
}

void ForwardPass::renderEntity(Entity& entity, Entity* mainLight)
{
    auto transform = entity.getComponent<Transform>();
    auto mesh = entity.getComponent<MeshRenderer>();
    if (mesh != nullptr && mesh->mesh != nullptr && mesh->material != nullptr)
    {
        Material *material = mesh->material;
        material->shader->use();

        // set object uniforms (e.g. transform)
        material->shader->setMat4("u_model", transform->modelMatrix());
        material->shader->setMat3("u_normalMatrix", glm::transpose(glm::inverse(glm::mat3(transform->modelMatrix()))));

        // shadow
        if (mainLight != nullptr)
        {
            auto t = mainLight->getComponent<Transform>();
            material->shader->setVec3("u_lightPos", t->getDirection() * -10.0f);
            material->shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);
        }

        // set material uniforms (e.g. color, textures)
        int texCount = 0;
        for (const auto &texture: material->textures)
        {
            material->shader->setInt(texture.first, texCount);
            glActiveTexture(GL_TEXTURE0 + texCount);
            glBindTexture(GL_TEXTURE_2D, texture.second);
            texCount++;
        }

        // bind global illumination textures
        material->shader->setInt("u_irradianceTex", 10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

        material->shader->setInt("u_prefilterMap", 11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

        material->shader->setInt("u_brdfLUT", 12);
        glActiveTexture(GL_TEXTURE12);
        glBindTexture(GL_TEXTURE_2D, brdfLUT);

        material->shader->setInt("u_shadowMap", 13);
        glActiveTexture(GL_TEXTURE13);
        glBindTexture(GL_TEXTURE_2D, shadowMap);

        material->setUniformData();

        glBindVertexArray(mesh->vao());
        glDrawElements(GL_TRIANGLES, (int) mesh->indicesCount(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    for (auto& childEntity : entity.getChildren())
    {
        renderEntity(*childEntity, mainLight);
    }

}

void ForwardPass::renderSkybox(Entity &skybox)
{
    auto mesh = skybox.getComponent<MeshRenderer>();
    Material* material = mesh->material;

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
}

