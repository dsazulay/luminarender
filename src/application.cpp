#include "application.h"

#include "renderer/transform_system.h"
#include "samples/unlit_scene.h"
#include "samples/lighting_skybox_scene.h"
#include "samples/pbr_scene.h"
#include "events/event.h"
#include "events/dispatcher.h"
#include "log.h"
#include "components/components.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


int logLevel = LogLevel::ERROR | LogLevel::WARNING | LogLevel::INFO;
float Application::deltaTime;

Application::Application(const AppConfig& config)
{
    m_window.init();
    m_window.createWindow(config.windowWidth, config.windowHeight, config.windowName.c_str());

    ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD");

    AssetLibrary::instance().createDefaultResources();

    // TODO: move to render config location
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    Dispatcher::instance().subscribe(UiCreateEmptyEvent::descriptor,
            std::bind(&Application::onUiCreateEmptyEvent, this, std::placeholders::_1));

    m_coordinator.init();
    m_coordinator.registerComponent<ecs::Transform>();
    m_coordinator.registerComponent<ecs::MeshRenderer>();
    m_coordinator.registerComponent<ecs::Light>();

    m_uiRenderer.init();
    m_uiRenderer.setBackendImplementation(m_window.glfwWindow());
    m_uiRenderer.viewportWidth = (float) config.viewportWidth;
    m_uiRenderer.viewportHeight = (float) config.viewportHeight;

    TransformSystem* transformSystem = m_coordinator.registerSystem<TransformSystem>().get();
    {
        ecs::Mask mask;
        mask.set(m_coordinator.getComponentType<ecs::Transform>());
        m_coordinator.setSystemMask<TransformSystem>(mask);
    }
    transformSystem->init(&m_coordinator);

    m_renderer = new Renderer((float) config.viewportWidth, (float) config.viewportHeight, glm::vec3(0.0, 0.0, 8.0f), m_coordinator);
    // create shaders, materials, and objects and add them to the scene
//    UnlitScene::loadScene(m_scene, AssetLibrary::instance());
    LightingSkyboxScene::loadScene(m_scene, AssetLibrary::instance(), m_coordinator);
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

        if (!m_scene.objects().empty())
            m_renderer->render(m_scene);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_windowWidth, m_windowHeight);
        glClearColor(0.4f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_uiRenderer.update(m_renderer->getTexcolorBufferID(), m_scene, m_renderer->viewMatrix(), m_renderer->projMatrix());

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
    m_uiRenderer.terminate();
    m_window.terminate();
}

Application::~Application()
{
    delete m_renderer;
}

void Application::onUiCreateEmptyEvent(const Event& e)
{
    const auto& event = static_cast<const UiCreateEmptyEvent&>(e);
    //m_scene.addObject(EntityFactory::createEmpty(), event.entity());
}
