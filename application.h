//
// Created by Diego Azulay on 24/04/22.
//

#ifndef INTERACTIVEGRAPHICS_APPLICATION_H
#define INTERACTIVEGRAPHICS_APPLICATION_H


#include "window.h"
#include "renderer.h"
#include "scene.h"
#include "imgui_renderer.h"
#include "asset_library.h"
#include <glm/glm.hpp>


class Application
{
public:
    Application(struct AppConfig config);
    ~Application();
    void init();
    void run();

    static float deltaTime;

private:
    void mainloop();
    void terminate();

    Window m_window;
    Renderer* m_renderer;
    ImguiRenderer m_imguiRenderer;
    Scene m_scene;
    AssetLibrary m_assetLibrary;

    int m_windowWidth;
    int m_windowHeight;
    float m_deltaTime;
    float m_lastFrame;
};

struct AppConfig
{
    std::string windowName;
    int windowWidth;
    int windowHeight;
    int viewportWidth;
    int viewportHeight;
};


#endif //INTERACTIVEGRAPHICS_APPLICATION_H
