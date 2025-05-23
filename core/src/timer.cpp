#include "timer.h"

#include <GLFW/glfw3.h>

float Timer::time = 0.0;
std::time_t Timer::systemTime = 0;

void Timer::update() {
    time = glfwGetTime();
    systemTime = std::time(nullptr);
}
