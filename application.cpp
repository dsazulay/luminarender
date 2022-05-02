#include "pch.h"
#include <glm/gtc/type_ptr.hpp>
#include "application.h"
#include "samples/sample_scene.h"
#include "samples/unlit_scene.h"
#include <glad/glad.h>

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

    m_imguiRenderer.init();
    m_imguiRenderer.setBackendImplementation(m_window.glfwWindow());
    m_imguiRenderer.viewportWidth = (float) config.viewportWidth;
    m_imguiRenderer.viewportHeight = (float) config.viewportHeight;

    // create shaders, materials, and objects and add them to the scene
//    SampleScene sampleScene(&m_scene, &m_assetLibrary);
    UnlitScene::loadScene(m_scene, AssetLibrary::instance());

    m_renderer->irradianceMap = m_scene.irradianceMap;
    m_renderer->prefilterMap = m_scene.prefilterMap;
    m_renderer->brdfLUT = m_scene.brdfLUT;

    Shader* s = AssetLibrary::instance().loadShader("normalVector", "resources/shaders/normal_vector.glsl");
    Material* m = AssetLibrary::instance().createMaterial("normalVector", s);
    m_renderer->mat = m;

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

        m_renderer->bindFrameBuffer();
        m_renderer->updateViewportDimensions();
        m_renderer->clearFrameBuffer();
        m_renderer->updateTransformMatrices();
        if (!m_scene.lights().empty())
            m_renderer->setupLights(m_scene.lights());
        if (!m_scene.objects().empty())
        {
            m_renderer->render(m_scene.objects());
//            m_renderer->renderNormalVector(m_scene.objects());
        }
        if (m_scene.hasSkybox())
            m_renderer->renderSkybox(m_scene.skybox());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_windowWidth, m_windowHeight);
        glClearColor(0.4f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_imguiRenderer.update(m_renderer->getTexcolorBufferID());

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
