#pragma once

#include "../ecs.h"
#include "light_system.h"
#include "render_system.h"
#include "../camera.h"
#include "uniform_buffer_object.h"

#include <glm/mat4x4.hpp>

class Renderer {
public:
    Renderer(float viewportWidth, float viewportHeight, glm::vec3 cameraPos);
    void init();
    void updateTransformMatrices();
    void updateIrradianceMaps();
    void render();

    glm::mat4 cascadeShadows(glm::vec3 lightDir);
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

    unsigned int getTexcolorBufferID();

    glm::mat4& viewMatrix();
    glm::mat4& projMatrix();

private:
    void onViewportResize(const Event& e);
    void onUiToggleSSAO(const Event& e);

    UniformBufferObject m_matricesUBO;
    UniformBufferObject m_lightUBO;

    float m_viewportWidth;
    float m_viewportHeight;

    ecs::Coordinator* m_coordinator{};
    LightSystem* m_lightSystem{};
    RenderSystem* m_renderSystem{};

    Camera m_camera;

    glm::mat4 m_viewMatrix{};
    glm::mat4 m_projMatrix{};
    glm::mat4 lightSpaceMatrix{};
};

