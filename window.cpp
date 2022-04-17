//
// Created by Diego Azulay on 12/04/22.
//

#include <iostream>
#include "window.h"

void Window::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void Window::terminate()
{
    glfwTerminate();
}

void Window::createWindow(int width, int height, const char *name)
{
    m_window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (m_window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, frameBufferCallback);
    glfwSetCursorPosCallback(m_window, mouseCallback);
    glfwSetScrollCallback(m_window, scrollCallback);

    lastX = width / 2.0;
    lastY = height / 2.0;
}

bool Window::windowShouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

void Window::processInput(float deltaTime) const
{
    if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);

    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera->processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera->processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera->processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera->processKeyboard(RIGHT, deltaTime);

//    if (glfwGetKey(m_Window, GLFW_KEY_Q) == GLFW_PRESS)
//        m_GuizmoType = -1;
//    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
//        m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
//    if (glfwGetKey(m_Window, GLFW_KEY_E) == GLFW_PRESS)
//        m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
//    if (glfwGetKey(m_Window, GLFW_KEY_R) == GLFW_PRESS)
//        m_GuizmoType = ImGuizmo::OPERATION::SCALE;
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

void Window::pollEvents()
{
    glfwPollEvents();
}

GLFWwindow *Window::glfwWindow() {
    return m_window;
}

void Window::frameBufferCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Window::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT))
    {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        camera.processMouseMovement(xoffset, yoffset);
    }
    else
    {
        lastX = xpos;
        lastY = ypos;
    }
}

void Window::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}
