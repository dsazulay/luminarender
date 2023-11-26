#include "window.h"

#include "events/event.h"
#include "events/dispatcher.h"
#include "log.h"


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
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, frameBufferCallback);
    glfwSetCursorPosCallback(m_window, 
            [](GLFWwindow* window, double xPos, double yPos)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_ALT))
        {
            MouseMoveEvent e(xPos, yPos, lastX, lastY);
            Dispatcher::instance().post(e);
        }

        lastX = xPos;
        lastY = yPos;
    });

    glfwSetScrollCallback(m_window, 
            [](GLFWwindow* window, double xOffset, double yOffset)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_ALT))
        {
            MouseScrollEvent e(xOffset, yOffset);
            Dispatcher::instance().post(e);
        }
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        // TODO: make this work with multiple keys pressed
//        if (action == GLFW_PRESS || action == GLFW_REPEAT)
//        {
//            KeyPressEvent e(key);
//            Dispatcher::instance().post(e);
//        }
    });

    ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD");
}

bool Window::windowShouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

void Window::processInput() const
{
    if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);

    int modifier = -1;
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_ALT))
        modifier = GLFW_KEY_LEFT_ALT;

    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
    {
        KeyPressEvent e(GLFW_KEY_W, modifier);
        Dispatcher::instance().post(e);
    }
    else if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
    {
        KeyPressEvent e(GLFW_KEY_S, modifier);
        Dispatcher::instance().post(e);
    }
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
    {
        KeyPressEvent e(GLFW_KEY_A, modifier);
        Dispatcher::instance().post(e);
    }
    else if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
    {
        KeyPressEvent e(GLFW_KEY_D, modifier);
        Dispatcher::instance().post(e);
    }

    if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
    {
       KeyPressEvent e(GLFW_KEY_Q, modifier);
       Dispatcher::instance().post(e);
    }
    else if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
    {
       KeyPressEvent e(GLFW_KEY_E, modifier);
       Dispatcher::instance().post(e);
    }

    if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
    {
         KeyPressEvent e(GLFW_KEY_R, modifier);
         Dispatcher::instance().post(e);
    }
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

void Window::pollEvents()
{
    glfwPollEvents();
}

GLFWwindow* Window::glfwWindow()
{
    return m_window;
}

void Window::frameBufferCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
