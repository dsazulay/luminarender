#pragma once

#include "pch.h"
#include "assets/material.h"
#include "entity.h"
#include "uniform_buffer_object.h"
#include "camera.h"
#include "frame_buffer.h"
#include "renderer/shadow_pass.h"
#include "renderer/forward_pass.h"
#include "renderer/normal_visualizer_pass.h"

class Renderer {
public:
    Renderer(float viewportWidth, float viewportHeight, glm::vec3 cameraPos);
    void updateTransformMatrices();
    void setupLights(std::vector<std::unique_ptr<Entity>> &lights);
    void render(Scene& scene);

    glm::mat4 cascadeShadows(glm::vec3 lightDir);
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

    unsigned int getTexcolorBufferID();
    unsigned int getShadowMapTextureID();

    void setGlobalTextures(Scene& scene);

    glm::mat4& viewMatrix();
    glm::mat4& projMatrix();

private:
    UniformBufferObject m_matricesUBO;
    UniformBufferObject m_lightUBO;

    float m_viewportWidth;
    float m_viewportHeight;

    glm::mat4 lightSpaceMatrix;

    RenderTarget m_shadowRenderTarget;
    RenderTarget m_mainRenderTarget;

    ShadowPass m_shadowRenderPass;
    ForwardPass m_forwardPass;
    NormalVisualizerPass m_normalVisualizerPass;

    Camera m_camera;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projMatrix;

    void onViewportResize(const Event& e);
};
