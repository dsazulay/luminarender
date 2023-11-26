#pragma once

#include "ecs.h"
#include "window.h"
#include "renderer/renderer.h"
#include "ui/ui_renderer.h"

#include <memory>

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
    explicit Application(AppConfig& config);
    void init();
    void run();

    static float deltaTime;

private:
    void mainloop();
    void terminate();

    std::unique_ptr<ecs::Coordinator> m_coordinator;
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<UiRenderer> m_uiRenderer;

    AppConfig m_config;
    // TODO: move this to a time class
    float m_deltaTime{};
    float m_lastFrame{};
};
