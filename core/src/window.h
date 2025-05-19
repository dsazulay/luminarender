#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
    Window();
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
};
