#pragma once

#include <GLFW/glfw3.h>
#include "../events/event.h"
#include "../scene.h"
#include "panel.h"
#include "../ecs.h"

#include <glm/mat4x4.hpp>

#include <optional>

class UiRenderer
{
public:
    UiRenderer(ecs::Coordinator& coordinator) : m_coordinator{coordinator} {}
    void init();
    void terminate();
    void update(unsigned int frameBufferTexcolorID, Scene& scene, glm::mat4& viewMatrix, glm::mat4& projMatrix);
    void setBackendImplementation(GLFWwindow* window);

    void onKeyPress(const Event& e);

    float viewportHeight;
    float viewportWidth;

private:
    int m_guizmoType;
    std::optional<ecs::Entity> m_selected;

    ecs::Coordinator& m_coordinator;
    ui::HierarchySystem* m_hierarchySystem;
    PropertiesSystem* m_properiesSystem;
};
