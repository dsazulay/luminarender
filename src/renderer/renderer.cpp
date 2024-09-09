#include "renderer.h"

#include "../events/dispatcher.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../log.h"
#include "../ecs.h"
#include "../components/components.h"


const int MATRICESUBO_SIZE = 2 * sizeof(glm::mat4) + 2 * sizeof(glm::vec4);
const int LIGHTUBO_SIZE = (2 * sizeof(glm::vec4) + sizeof(glm::mat4) 
        + 12 * sizeof(LightUniformStruct));

void Renderer::render()
{
    updateTransformMatrices();

    m_lightSystem->update(m_lightUBO);
    m_renderSystem->update();
}

glm::mat4& Renderer::viewMatrix()
{
    return m_viewMatrix;
}

glm::mat4& Renderer::projMatrix()
{
    return m_projMatrix;
}

void Renderer::updateIrradianceMaps()
{
    m_renderSystem->updateIrradianceMaps();
}

void Renderer::updateTransformMatrices()
{
    m_viewMatrix = m_camera.getViewMatrix();
    m_projMatrix = glm::perspective(glm::radians(m_camera.zoom), 
            (float) m_viewportWidth / (float) m_viewportHeight, 0.1f, 100.0f);

    m_matricesUBO.setBufferData(0, sizeof(glm::mat4), glm::value_ptr(m_viewMatrix));
    m_matricesUBO.setBufferData(sizeof(glm::mat4), sizeof(glm::mat4), 
            glm::value_ptr(m_projMatrix));
    m_matricesUBO.setBufferData(2 * sizeof(glm::mat4), sizeof(glm::vec4), 
            glm::value_ptr(glm::vec4(m_camera.position, 1.0)));
    m_matricesUBO.setBufferData(2 * sizeof(glm::mat4) + sizeof(glm::vec4), 
            sizeof(glm::vec4), glm::value_ptr(glm::vec4(m_camera.front, 1.0)));
}

Renderer::Renderer(float viewportWidth, float viewportHeight, 
        glm::vec3 cameraPos, ecs::Coordinator* coordinator) : 
    m_viewportWidth(viewportWidth), m_viewportHeight(viewportHeight),
    m_matricesUBO(MATRICESUBO_SIZE), m_lightUBO(LIGHTUBO_SIZE),
    m_camera(cameraPos), m_coordinator(coordinator)
{
}

void Renderer::init()
{
    m_matricesUBO.bindBufferToIndex(0);
    m_lightUBO.bindBufferToIndex(1);

    m_lightSystem = m_coordinator->registerSystem<LightSystem>().get();
    {
        ecs::Mask mask;
        mask.set(m_coordinator->getComponentType<ecs::Transform>());
        mask.set(m_coordinator->getComponentType<ecs::Light>());
        m_coordinator->setSystemMask<LightSystem>(mask);
    }
    m_lightSystem->init(m_coordinator);

    m_renderSystem = m_coordinator->registerSystem<RenderSystem>().get();
    {
        ecs::Mask mask;
        mask.set(m_coordinator->getComponentType<ecs::Transform>());
        mask.set(m_coordinator->getComponentType<ecs::MeshRenderer>());
        m_coordinator->setSystemMask<RenderSystem>(mask);
    }
    m_renderSystem->init((int) m_viewportWidth, (int) m_viewportHeight, 
            m_coordinator, &m_camera);

    Dispatcher::instance().subscribe(ViewportResizeEvent::descriptor,
        std::bind(&Renderer::onViewportResize, this, std::placeholders::_1));
    Dispatcher::instance().subscribe(UiToggleSSAOEvent::descriptor,
        [&] (const auto& arg) { Renderer::onUiToggleSSAO(arg); });
}

unsigned int Renderer::getTexcolorBufferID()
{
    return m_renderSystem->getFinalRenderTexID();
}

void Renderer::onViewportResize(const Event& e)
{
    const auto& event = static_cast<const ViewportResizeEvent&>(e);
    m_viewportWidth = event.width();
    m_viewportHeight = event.height();
    m_renderSystem->resizeBuffers(m_viewportWidth, m_viewportHeight);

    LOG_INFO("{} {}", m_viewportWidth, m_viewportHeight);
}

void Renderer::onUiToggleSSAO(const Event& e)
{
    const auto& event = static_cast<const UiToggleSSAOEvent&>(e);
    m_renderSystem->toggleSSAO(event.enabled());
}


//std::vector<glm::vec4> Renderer::getFrustumCornersWorldSpace(const glm::mat4 &proj, const glm::mat4 &view)
//{
//    const auto inv = glm::inverse(proj * view);
//
//    std::vector<glm::vec4> frustumCorners;
//    for (unsigned int x = 0; x < 2; ++x)
//    {
//        for (unsigned int y = 0; y < 2; ++y)
//        {
//            for (unsigned int z = 0; z < 2; ++z)
//            {
//                const glm::vec4 pt =
//                        inv * glm::vec4(
//                                2.0f * x - 1.0f,
//                                2.0f * y - 1.0f,
//                                2.0f * z - 1.0f,
//                                1.0f);
//                frustumCorners.push_back(pt / pt.w);
//            }
//        }
//    }
//
//    return frustumCorners;
//}

//glm::mat4 Renderer::cascadeShadows(glm::vec3 lightDir)
//{
//    glm::mat4 view = m_camera.getViewMatrix();
//    glm::mat4 projection = glm::perspective(glm::radians(m_camera.zoom),
//                                            (float) m_viewportWidth / (float) m_viewportHeight, 0.1f, 100.0f);
//
//    auto corners = getFrustumCornersWorldSpace(projection, view);
//
//    glm::vec3 center = glm::vec3(0, 0, 0);
//    for (const auto& v : corners)
//    {
//        center += glm::vec3(v);
//    }
//    center /= corners.size();
//
//    const auto lightView = glm::lookAt(
//            center + lightDir,
//            center,
//            glm::vec3(0.0f, 1.0f, 0.0f)
//    );
//
//    float minX = std::numeric_limits<float>::max();
//    float maxX = std::numeric_limits<float>::min();
//    float minY = std::numeric_limits<float>::max();
//    float maxY = std::numeric_limits<float>::min();
//    float minZ = std::numeric_limits<float>::max();
//    float maxZ = std::numeric_limits<float>::min();
//    for (const auto& v : corners)
//    {
//        const auto trf = lightView * v;
//        minX = std::min(minX, trf.x);
//        maxX = std::max(maxX, trf.x);
//        minY = std::min(minY, trf.y);
//        maxY = std::max(maxY, trf.y);
//        minZ = std::min(minZ, trf.z);
//        maxZ = std::max(maxZ, trf.z);
//    }
//
//    // Tune this parameter according to the scene
//    constexpr float zMult = 10.0f;
//    if (minZ < 0)
//    {
//        minZ *= zMult;
//    }
//    else
//    {
//        minZ /= zMult;
//    }
//    if (maxZ < 0)
//    {
//        maxZ /= zMult;
//    }
//    else
//    {
//        maxZ *= zMult;
//    }
//
//    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
//
//    return lightProjection * lightView;
//}
