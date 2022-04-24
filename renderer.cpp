//
// Created by Diego Azulay on 14/04/22.
//

#include "renderer.h"
#include "assets/material.h"
#include "components/light.h"
#include "components/mesh_renderer.h"
#include "log.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

void Renderer::render(std::vector<Entity>& objects) {
    for (auto& entity : objects)
    {
        auto transform = entity.getComponent<Transform>();
        auto mesh = entity.getComponent<MeshRenderer>();
        if (mesh == nullptr)
            continue;
        Material* material = mesh->material;


        material->shader->use();

        // set object uniforms (e.g. transform)
        material->shader->setMat4("u_model", transform->modelMatrix());
        material->shader->setMat3("u_normalMatrix", glm::transpose(glm::inverse(glm::mat3(transform->modelMatrix()))));

        // set material uniforms (e.g. color, textures)
        int texCount = 0;
        for (const auto& texture : material->textures)
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

        material->setUniformData();

        glBindVertexArray(mesh->vao());
        glDrawElements(GL_TRIANGLES, (int)mesh->mesh->indicesSize(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
}

void Renderer::renderSkybox(Entity &skybox)
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
    glDrawElements(GL_TRIANGLES, (int)mesh->mesh->indicesSize(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Renderer::setupLights(std::vector<Entity> &lights)
{
    std::size_t offset = sizeof(glm::vec4);
    std::size_t uniformStructSize = sizeof(LightUniformStruct);

    glm::vec4 nLights = glm::vec4(2, 1, 1, 0);
    m_lightUBO.setBufferData(0, sizeof(glm::vec4), &nLights);

    for (auto& entity : lights)
    {
        auto transform = entity.getComponent<Transform>();
        auto light = entity.getComponent<Light>();

        light->uniformStruct.posAndCutoff = glm::vec4(transform->position(), light->cutoff);
        light->uniformStruct.dirAndOuterCutoff = glm::vec4(transform->getDirection(), light->outerCutoff);
        light->uniformStruct.colorAndIntensity = glm::vec4(light->color, light->intensity);
        light->uniformStruct.attenuationAndType = glm::vec4(light->attenuation, light->lightType);

        m_lightUBO.setBufferData(offset, uniformStructSize, &light->uniformStruct);
        offset += uniformStructSize;
    }
}

Renderer::Renderer() : m_lightUBO((sizeof(glm::vec4) + 12 * sizeof(LightUniformStruct)))
{
    m_lightUBO.bindBufferToIndex(1);
}
