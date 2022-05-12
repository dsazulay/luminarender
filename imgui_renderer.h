#pragma once

#include <GLFW/glfw3.h>
#include "frame_buffer.h"
#include "scene.h"

class ImguiRenderer {
public:
    void init();
    void terminate();
    void update(unsigned int frameBufferTexcolorID, Scene& scene);
    void setBackendImplementation(GLFWwindow* window);

    float viewportHeight;
    float viewportWidth;

private:
};
