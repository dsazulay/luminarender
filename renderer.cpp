#include "renderer.h"
#include "components/light.h"
#include "components/mesh_renderer.h"
#include "components/transform.h"
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

void Renderer::renderNormalVector(std::list<Entity>& objects)
{
    for (auto& entity : objects)
    {
        renderNormalVectorOfEntity(entity);

        for (auto& childEntity : entity.getChildren())
        {
            renderNormalVectorOfEntity(childEntity);
        }
    }
}

void Renderer::RecreateShadowMap(std::list<Entity> &objects, Entity &light)
{
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    auto t = light.getComponent<Transform>();

    glm::mat4 lightProjection, lightView;
    float near_plane = 0.1f, far_plane = 40.0f;
    //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
    lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
    // light vectors used for test
    lightView = glm::lookAt(t->getDirection() * -10.f, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;

    // render scene from light's point of view
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (auto& entity : objects)
    {
        createShadowMap(entity);

        for (auto& childEntity : entity.getChildren())
        {
            createShadowMap(childEntity);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    shadowSetup();
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

    // shadow
    material->shader->setVec3("u_lightPos", glm::vec3(-2.0f, 4.0f, -1.0f));
    material->shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

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

void Renderer::shadowSetup()
{
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::vector<glm::vec4> Renderer::getFrustumCornersWorldSpace(const glm::mat4 &proj, const glm::mat4 &view)
{
    const auto inv = glm::inverse(proj * view);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt =
                        inv * glm::vec4(
                                2.0f * x - 1.0f,
                                2.0f * y - 1.0f,
                                2.0f * z - 1.0f,
                                1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

void Renderer::renderNormalVectorOfEntity(Entity &entity)
{
    auto transform = entity.getComponent<Transform>();
    auto mesh = entity.getComponent<MeshRenderer>();
    if (mesh == nullptr)
        return;
    Material* material = mat;

    material->shader->use();

    // set object uniforms (e.g. transform)
    material->shader->setMat4("u_model", transform->modelMatrix());
    material->shader->setMat3("u_normalMatrix", glm::transpose(glm::inverse(glm::mat3(m_camera.getViewMatrix() * transform->modelMatrix()))));

    material->setUniformData();

    glBindVertexArray(mesh->vao());
    glDrawElements(GL_TRIANGLES, (int)mesh->mesh->indicesSize(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Renderer::createShadowMap(Entity& entity)
{
    auto transform = entity.getComponent<Transform>();
    auto mesh = entity.getComponent<MeshRenderer>();
    if (mesh == nullptr)
        return;
    Material *material = shadowMat;
    Material *originalMat = mesh->material;

//    originalMat->setTexture("u_mainTex", depthMap, 0);
    originalMat->setTexture("u_shadowMap", depthMap, 0);
    material->shader->use();

    // set object uniforms (e.g. transform)
    material->shader->setMat4("u_model", transform->modelMatrix());
    material->shader->setMat4("u_lightSpaceMatrix", lightSpaceMatrix);

    material->setUniformData();

    glBindVertexArray(mesh->vao());
    glDrawElements(GL_TRIANGLES, (int) mesh->mesh->indicesSize(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

glm::mat4 Renderer::cascadeShadows(glm::vec3 lightDir)
{
    glm::mat4 view = m_camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(m_camera.zoom),
                                            (float) m_viewportWidth / (float) m_viewportHeight, 0.1f, 100.0f);

    auto corners = getFrustumCornersWorldSpace(projection, view);

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(
            center + lightDir,
            center,
            glm::vec3(0.0f, 1.0f, 0.0f)
    );

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

    return lightProjection * lightView;
}
