#pragma once

#include "renderer/light_system.h"
#include "window.h"
#include "scene.h"
#include "renderer/renderer.h"
#include "ui/ui_renderer.h"
#include "ecs.h"

#include <glm/glm.hpp>

struct AppConfig
{
    std::string windowName;
    int windowWidth;
    int windowHeight;
    int viewportWidth;
    int viewportHeight;
};

class Application
{
public:
    explicit Application(const AppConfig& config);
    void onUiCreateEmptyEvent(const Event& e);
    ~Application();
    void init();
    void run();

    static float deltaTime;

private:
    void mainloop();
    void terminate();

    Window m_window{};
    Renderer* m_renderer;
    UiRenderer* m_uiRenderer;
    Scene m_scene;
    ecs::Coordinator m_coordinator;

    int m_windowWidth{};
    int m_windowHeight{};
    float m_deltaTime{};
    float m_lastFrame{};
};
