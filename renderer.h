#pragma once

#include "pch.h"
#include "assets/material.h"
#include "entity.h"
#include "uniform_buffer_object.h"
#include "camera.h"
#include "frame_buffer.h"
#include "renderer/shadow_pass.h"

class Renderer {
public:
    Renderer(float viewportWidth, float viewportHeight, glm::vec3 cameraPos);
    void bindFrameBuffer();
    void updateViewportDimensions();
    void clearFrameBuffer();
    void updateTransformMatrices();
    void setupLights(std::vector<Entity>& lights);
    void render(std::list<Entity>& objects);
    void renderSkybox(Entity& skybox);
    void renderNormalVector(std::list<Entity>& objects);
    void shadowSetup();
    void RecreateShadowMap(std::list<Entity> &objects, Entity* light);
    void createShadowMap(Entity& objects);

    glm::mat4 cascadeShadows(glm::vec3 lightDir);
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

    unsigned int getTexcolorBufferID();

    void setShadowMaterialAndLight(Entity* light);

    Material* mat;
    Material* shadowMat;
    Entity* m_light;

    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUT;

private:
    UniformBufferObject m_matricesUBO;
    UniformBufferObject m_lightUBO;

    float m_viewportWidth;
    float m_viewportHeight;

    FrameBuffer m_viewportFrameBuffer;
    unsigned int depthMapFBO;
    unsigned int depthMap;
    glm::mat4 lightSpaceMatrix;

    ShadowPass m_shadowRenderPass;

    Camera m_camera;

    void onViewportResize(const Event& e);
    void renderEntity(Entity& entity);
    void renderNormalVectorOfEntity(Entity &entity);
};
