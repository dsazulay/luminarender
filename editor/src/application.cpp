#include "application.h"

#include "asset_manager.h"
#include "components/components.h"
#include "renderer/light_system.h"
#include "renderer/render_system.h"
#include "renderer/transform_system.h"
//#include "samples/lighting_skybox_scene.h"
#include "samples/pbr_scene.h"
#include "locator.h"

#include <glm/gtc/type_ptr.hpp>


float Application::deltaTime;

Application::Application(AppConfig& config) : m_config(config)
{
    Locator::provide(&m_gpucommands);
    Locator::provide(&m_gpurm);
    Locator::provide(&m_coordinator);
    Locator::provide(&m_assetManager);

    initEcs();
    m_assetManager.loadDefaultResources();
    initWindow();
    initRenderer();
    initUiRenderer();

    auto transformSystem = m_coordinator.getSystem<TransformSystem>();
    transformSystem->init();

    PbrScene pbrScene;
    pbrScene.loadScene();
    m_renderer->updateIrradianceMaps();
}

void Application::initEcs()
{
    m_coordinator.init();

    m_coordinator.registerComponent<ecs::Transform>();
    m_coordinator.registerComponent<ecs::Tag>();
    m_coordinator.registerComponent<ecs::MeshRenderer>();
    m_coordinator.registerComponent<ecs::Light>();

    m_coordinator.registerSystem<TransformSystem>();
    {
        ecs::Mask mask;
        mask.set(m_coordinator.getComponentType<ecs::Transform>());
        m_coordinator.setSystemMask<TransformSystem>(mask);
    }

    m_coordinator.registerSystem<LightSystem>();
    {
        ecs::Mask mask;
        mask.set(m_coordinator.getComponentType<ecs::Transform>());
        mask.set(m_coordinator.getComponentType<ecs::Light>());
        m_coordinator.setSystemMask<LightSystem>(mask);
    }

    auto renderSystem = m_coordinator.registerSystem<RenderSystem>();
    {
        ecs::Mask mask;
        mask.set(m_coordinator.getComponentType<ecs::Transform>());
        mask.set(m_coordinator.getComponentType<ecs::MeshRenderer>());
        m_coordinator.setSystemMask<RenderSystem>(mask);
    }

    // TODO: Check why this is necessary (very strange bug?)
    m_coordinator.getSystem<TransformSystem>();
    m_coordinator.getSystem<LightSystem>();
    m_coordinator.getSystem<RenderSystem>();
}

void Application::initWindow()
{
    m_window.createWindow(m_config.windowWidth, m_config.windowHeight,
                          m_config.windowName.c_str());
}

void Application::initRenderer()
{
    m_renderer = std::make_unique<Renderer>((float) m_config.viewportWidth,
                                            (float) m_config.viewportHeight,
                                            glm::vec3(0.0, 0.0, 8.0f));

    m_renderer->init();
}

void Application::initUiRenderer()
{
    m_uiRenderer = std::make_unique<UiRenderer>(&m_coordinator, &m_assetManager);
    m_uiRenderer->init();
    m_uiRenderer->setBackendImplementation(m_window.glfwWindow());
    m_uiRenderer->viewportWidth = (float) m_config.viewportWidth;
    m_uiRenderer->viewportHeight = (float) m_config.viewportHeight;
}

void Application::mainloop()
{
    while (!m_window.windowShouldClose())
    {
        auto currentFrame = (float) glfwGetTime();
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;
        deltaTime = m_deltaTime;

        m_window.processInput();

        m_renderer->render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_config.windowWidth, m_config.windowHeight);
        glClearColor(0.4f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_uiRenderer->update(m_renderer->getTexcolorBufferID(), m_renderer->viewMatrix(), m_renderer->projMatrix());

        m_window.swapBuffers();
        m_window.pollEvents();
    }
}



void Application::run()
{
    mainloop();
    terminate();
}

void Application::terminate()
{
    m_uiRenderer->terminate();
//    m_renderer->terminate();
    m_window.terminate();
//    m_coordinator->terminate();
}

