#pragma once

#include "../ecs.h"
#include "mainmenu_system.h"
#include "viewport_system.h"
#include "hierarchy_system.h"
#include "properties_system.h"
#include "../events/event.h"

#include <glm/mat4x4.hpp>
#include <GLFW/glfw3.h>

#include <optional>

class UiRenderer
{
public:
    UiRenderer(ecs::Coordinator* coordinator) : m_coordinator{coordinator} {}
    void init();
    void terminate();
    void update(unsigned int frameBufferTexcolorID, glm::mat4& viewMatrix, glm::mat4& projMatrix);
    void setBackendImplementation(GLFWwindow* window);

    void onKeyPress(const Event& e);

    float viewportHeight{};
    float viewportWidth{};

private:
    int m_guizmoType{};
    std::optional<ecs::Entity> m_selected;

    ecs::Coordinator* m_coordinator;
    MainMenuSystem* m_mainMenuSystem{};
    ViewportSystem* m_viewportSystem{};
    HierarchySystem* m_hierarchySystem{};
    PropertiesSystem* m_properiesSystem{};
};
