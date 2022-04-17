//
// Created by Diego Azulay on 12/04/22.
//

#ifndef INTERACTIVEGRAPHICS_IMGUI_RENDERER_H
#define INTERACTIVEGRAPHICS_IMGUI_RENDERER_H


#include <GLFW/glfw3.h>
#include "frame_buffer.h"

class ImguiRenderer {
public:
    void init();
    void terminate();
    void update();
    void setBackendImplementation(GLFWwindow* window);

    float viewportHeight;
    float viewportWidth;
    FrameBuffer* frameBuffer;

private:
};


#endif //INTERACTIVEGRAPHICS_IMGUI_RENDERER_H
