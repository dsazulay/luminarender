#pragma once

#include "window.h"
#include "renderer.h"
#include "scene.h"
#include "imgui_renderer.h"
#include "asset_library.h"
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
    ImguiRenderer m_imguiRenderer{};
    Scene m_scene;

    int m_windowWidth{};
    int m_windowHeight{};
    float m_deltaTime{};
    float m_lastFrame{};
};
