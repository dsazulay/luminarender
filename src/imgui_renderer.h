#pragma once

#include <GLFW/glfw3.h>
#include "events/event.h"
#include "frame_buffer.h"
#include "scene.h"
#include <glm/glm.hpp>

class ImguiRenderer {
public:
    void init();
    void terminate();
    void update(unsigned int frameBufferTexcolorID, Scene& scene, glm::mat4& viewMatrix, glm::mat4& projMatrix);
    void setBackendImplementation(GLFWwindow* window);

    void onKeyPress(const Event& e);

    float viewportHeight;
    float viewportWidth;

private:
    int m_guizmoType;
};
