#pragma once

#include <GLFW/glfw3.h>
#include "frame_buffer.h"

class ImguiRenderer {
public:
    void init();
    void terminate();
    void update(unsigned int frameBufferTexcolorID);
    void setBackendImplementation(GLFWwindow* window);

    float viewportHeight;
    float viewportWidth;

private:
};
