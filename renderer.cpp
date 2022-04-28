//
// Created by Diego Azulay on 14/04/22.
//

#include "renderer.h"
#include "assets/material.h"
#include "components/light.h"
#include "components/mesh_renderer.h"
#include "events/dispatcher.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

void Renderer::render(std::list<Entity>& objects)
{
    for (auto& entity : objects)
    {
        renderEntity(entity);

        for (auto& childEntity : entity.getChildren())
        {
            renderEntity(childEntity);
        }
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

void Renderer::updateTransformMatrices()
{
    glm::mat4 view = m_camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(m_camera.zoom),
                                            (float) m_viewportWidth / (float) m_viewportHeight, 0.1f, 100.0f);

    m_matricesUBO.setBufferData(0, sizeof(glm::mat4), glm::value_ptr(view));
    m_matricesUBO.setBufferData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    m_matricesUBO.setBufferData(2 * sizeof(glm::mat4), sizeof(glm::vec4), glm::value_ptr(m_camera.position));
}

Renderer::Renderer(float viewportWidth, float viewportHeight, glm::vec3 cameraPos) :
    m_viewportWidth(viewportWidth), m_viewportHeight(viewportHeight),
    m_viewportFrameBuffer(m_viewportWidth, m_viewportHeight),
    m_matricesUBO(2 * sizeof(glm::mat4) + sizeof(glm::vec4)),
    m_lightUBO((sizeof(glm::vec4) + 12 * sizeof(LightUniformStruct))),
    m_camera(cameraPos)
{

    m_matricesUBO.bindBufferToIndex(0);
    m_lightUBO.bindBufferToIndex(1);

    Dispatcher::instance().subscribe(ViewportResizeEvent::descriptor,
     std::bind(&Renderer::onViewportResize, this, std::placeholders::_1));
}

void Renderer::bindFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_viewportFrameBuffer.getID());
}

void Renderer::updateViewportDimensions()
{
    // TODO: verify if this is necessary
    glViewport(0, 0, m_viewportWidth, m_viewportHeight);
}

void Renderer::clearFrameBuffer()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

unsigned int Renderer::getTexcolorBufferID()
{
    return m_viewportFrameBuffer.getTexcolorBufferID();
}

void Renderer::onViewportResize(const Event& e)
{
    const auto& event = static_cast<const ViewportResizeEvent&>(e);
    m_viewportWidth = event.width();
    m_viewportHeight = event.height();
    m_viewportFrameBuffer.resizeBuffer(m_viewportWidth, m_viewportHeight);
}

void Renderer::renderEntity(Entity &entity)
{
    auto transform = entity.getComponent<Transform>();
    auto mesh = entity.getComponent<MeshRenderer>();
    if (mesh == nullptr)
        return;
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
