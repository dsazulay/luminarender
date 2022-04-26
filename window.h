//
// Created by Diego Azulay on 12/04/22.
//

#ifndef INTERACTIVEGRAPHICS_WINDOW_H
#define INTERACTIVEGRAPHICS_WINDOW_H


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"

class Window
{
public:
    void init();
    void terminate();
    void createWindow(int width, int height, const char* name);
    bool windowShouldClose() const;
    void swapBuffers() const;
    void pollEvents();
    void processInput() const;
    GLFWwindow* glfwWindow();

private:
    GLFWwindow* m_window;

    inline static float lastX = 0;
    inline static float lastY = 0;

    static void frameBufferCallback(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
};

#endif //INTERACTIVEGRAPHICS_WINDOW_H
