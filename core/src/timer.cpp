#include "timer.h"

#include <GLFW/glfw3.h>

float Timer::time = 0.0;
float Timer::deltaTime = 0.0;
float Timer::lastFrameTime = 0.0;
std::time_t Timer::systemTime = 0;

void Timer::update() {
    lastFrameTime = time;
    time = glfwGetTime();
    deltaTime = time - lastFrameTime;
    systemTime = std::time(nullptr);
}
