#include "entity_factory.h"
#include "events/event.h"
#include "pch.h"
#include <glm/gtc/type_ptr.hpp>
#include "application.h"
#include "samples/unlit_scene.h"
#include "samples/lighting_skybox_scene.h"
#include "samples/pbr_scene.h"
#include <glad/glad.h>
#include "events/dispatcher.h"

float Application::deltaTime;

Application::Application(const AppConfig& config)
{
    m_window.init();
    m_window.createWindow(config.windowWidth, config.windowHeight, config.windowName.c_str());

    ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD");

    m_renderer = new Renderer((float) config.viewportWidth, (float) config.viewportHeight,
                              glm::vec3(0.0, 0.0, 8.0f));

    // TODO: move to render config location
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    Dispatcher::instance().subscribe(UiCreateEmptyEvent::descriptor,
            std::bind(&Application::onUiCreateEmptyEvent, this, std::placeholders::_1));


    m_imguiRenderer.init();
    m_imguiRenderer.setBackendImplementation(m_window.glfwWindow());
    m_imguiRenderer.viewportWidth = (float) config.viewportWidth;
    m_imguiRenderer.viewportHeight = (float) config.viewportHeight;

    // create shaders, materials, and objects and add them to the scene
//    UnlitScene::loadScene(m_scene, AssetLibrary::instance());
    LightingSkyboxScene::loadScene(m_scene, AssetLibrary::instance());
//    PbrScene::loadScene(m_scene, AssetLibrary::instance());

    m_renderer->setGlobalTextures(m_scene);
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

        m_renderer->setGlobalTextures(m_scene);

        m_renderer->updateTransformMatrices();
        if (!m_scene.lights().empty())
            m_renderer->setupLights(m_scene.lights());

        if (!m_scene.objects().empty())
            m_renderer->render(m_scene);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_windowWidth, m_windowHeight);
        glClearColor(0.4f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_imguiRenderer.update(m_renderer->getTexcolorBufferID(), m_scene, m_renderer->viewMatrix(), m_renderer->projMatrix());

        m_window.swapBuffers();
        m_window.pollEvents();
    }
}

void Application::init()
{

}

void Application::run()
{
    init();
    mainloop();
    terminate();
}

void Application::terminate()
{
    m_imguiRenderer.terminate();
    m_window.terminate();
}

Application::~Application()
{
    delete m_renderer;
}

void Application::onUiCreateEmptyEvent(const Event& e)
{
    const auto& event = static_cast<const UiCreateEmptyEvent&>(e);
    m_scene.addObject(EntityFactory::createEmpty(), event.entity());
}