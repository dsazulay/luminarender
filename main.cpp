#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
#include "imgui_renderer.h"
#include <glm/glm.hpp>
#include "shader.h"
#include "material.h"
#include "importer.h"
#include "camera.h"
#include "uniform_buffer_object.h"
#include "frame_buffer.h"
#include "components/transform.h"
#include "components/mesh.h"
#include "primitives.h"
#include "renderer.h"
#include "components/light.h"
#include "entity_factory.h"
#include "entity.h"
#include "scene.h"
#include "samples/sample_scene.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int viewportWidth = 640;
int viewportHeight = 400;


// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;


int main() {
    Window window{};
    window.init();
    window.createWindow(SCR_WIDTH, SCR_HEIGHT, "Interactive Graphics");
    window.m_camera = &camera;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // TODO: move to render config location
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    UniformBufferObject uboMatrices(2 * sizeof(glm::mat4) + sizeof(glm::vec4));
    uboMatrices.bindBufferToIndex(0);

    FrameBuffer frameBuffer(viewportWidth, viewportHeight);

    ImguiRenderer imguiRenderer{};
    imguiRenderer.init();
    imguiRenderer.setBackendImplementation(window.glfwWindow());
    imguiRenderer.frameBuffer = &frameBuffer;
    imguiRenderer.viewportWidth = viewportWidth;
    imguiRenderer.viewportHeight = viewportHeight;


    Scene myScene;
    // create shaders, materials, and objects and add them to the scene
    SampleScene sampleScene(&myScene);

    Renderer renderer;
    renderer.irradianceMap = myScene.irradianceMap;
    renderer.prefilterMap = myScene.prefilterMap;
    renderer.brdfLUT = myScene.brdfLUT;

    while (!window.windowShouldClose())
    {
        auto currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        window.processInput(deltaTime);

        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.getID());
        glViewport(0, 0, viewportWidth, viewportHeight);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)viewportWidth / (float)viewportHeight, 0.1f, 100.0f);

        uboMatrices.setBufferData(0, sizeof(glm::mat4), glm::value_ptr(view));
        uboMatrices.setBufferData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
        uboMatrices.setBufferData(2 * sizeof(glm::mat4), sizeof(glm::vec4), glm::value_ptr(camera.position));

        renderer.setupLights(myScene.lights());
        renderer.render(myScene.objects());
        renderer.renderSkybox(myScene.skybox());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.4f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        imguiRenderer.update();

        viewportWidth = imguiRenderer.viewportWidth;
        viewportHeight = imguiRenderer.viewportHeight;

        window.swapBuffers();
        window.pollEvents();
    }

    imguiRenderer.terminate();
    window.terminate();
    return 0;
}