#include "renderer.h"

#include "../components/light.h"
#include "../components/mesh_renderer.h"
#include "../components/transform.h"
#include "../events/dispatcher.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../log.h"


void Renderer::render(Scene& scene)
{
    /*
    if (scene.mainLight() != nullptr)
    {
        m_shadowFrameBuffer.bind();
        gpucommands.setViewportSize(0, 0, 2048, 2048);
        gpucommands.clear(ClearMask::DEPTH);
        m_shadowRenderPass.render(scene);
        m_shadowFrameBuffer.unbind();
        m_forwardPass.lightSpaceMatrix = m_shadowRenderPass.lightSpaceMatrix;
    }

    m_mainTargetFrameBuffer.bind();
    gpucommands.setViewportSize(0, 0, m_viewportWidth, m_viewportHeight);
    gpucommands.setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    gpucommands.clear(ClearMask::COLORDEPTH);
    m_forwardPass.render(scene);
    //m_normalVisualizerPass.render(scene);
    m_mainTargetFrameBuffer.unbind();
    */
   
    m_gbuffer.bind();
    gpucommands.setViewportSize(0, 0, m_viewportWidth, m_viewportHeight);
    gpucommands.setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    gpucommands.clear(ClearMask::COLORDEPTH);
    m_geometryPass.render(scene);
    m_gbuffer.unbind();

    m_mainTargetFrameBuffer.bind();
    gpucommands.setViewportSize(0, 0, m_viewportWidth, m_viewportHeight);
    gpucommands.setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    gpucommands.clear(ClearMask::COLORDEPTH);
    m_lightingPass.render();
    //m_normalVisualizerPass.render(scene);
    m_mainTargetFrameBuffer.unbind();

}

void Renderer::setupLights(std::vector<std::unique_ptr<Entity>> &lights)
{
    std::size_t offset = sizeof(glm::vec4);
    std::size_t uniformStructSize = sizeof(LightUniformStruct);

    glm::vec4 nLights = glm::vec4(2, 1, 1, 0);
    m_lightUBO.setBufferData(0, sizeof(glm::vec4), &nLights);

    for (auto& entity : lights)
    {
        auto transform = entity->getComponent<Transform>();
        auto light = entity->getComponent<Light>();

        light->uniformStruct.posAndCutoff = glm::vec4(transform->position(), light->cutoff);
        light->uniformStruct.dirAndOuterCutoff = glm::vec4(transform->getDirection(), light->outerCutoff);
        light->uniformStruct.colorAndIntensity = glm::vec4(light->color, light->intensity);
        light->uniformStruct.attenuationAndType = glm::vec4(light->attenuation, light->lightType);

        m_lightUBO.setBufferData(offset, uniformStructSize, &light->uniformStruct);
        offset += uniformStructSize;
    }
}

glm::mat4& Renderer::viewMatrix()
{
    return m_viewMatrix;
}

glm::mat4& Renderer::projMatrix()
{
    return m_projMatrix;
}

void Renderer::updateTransformMatrices()
{
    m_viewMatrix = m_camera.getViewMatrix();
    m_projMatrix = glm::perspective(glm::radians(m_camera.zoom),
                                            (float) m_viewportWidth / (float) m_viewportHeight, 0.1f, 100.0f);

    m_matricesUBO.setBufferData(0, sizeof(glm::mat4), glm::value_ptr(m_viewMatrix));
    m_matricesUBO.setBufferData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_projMatrix));
    m_matricesUBO.setBufferData(2 * sizeof(glm::mat4), sizeof(glm::vec4), glm::value_ptr(m_camera.position));
}

Renderer::Renderer(float viewportWidth, float viewportHeight, glm::vec3 cameraPos) :
    m_viewportWidth(viewportWidth), m_viewportHeight(viewportHeight),
    m_matricesUBO(2 * sizeof(glm::mat4) + sizeof(glm::vec4)),
    m_lightUBO((sizeof(glm::vec4) + 12 * sizeof(LightUniformStruct))),
    m_camera(cameraPos),
    m_mainTargetFrameBuffer(m_viewportWidth, m_viewportHeight, gpurm),
    m_shadowFrameBuffer(2048, 2048, gpurm),
    m_gbuffer(m_viewportWidth, m_viewportHeight, gpurm)
{

    m_matricesUBO.bindBufferToIndex(0);
    m_lightUBO.bindBufferToIndex(1);

    Dispatcher::instance().subscribe(ViewportResizeEvent::descriptor,
     std::bind(&Renderer::onViewportResize, this, std::placeholders::_1));

    m_normalVisualizerPass.camera(&m_camera);

    m_lightingPass.gposition = m_gbuffer.getPositionAttachmentID();
    m_lightingPass.gnormal = m_gbuffer.getNormalAttachmentID();
    m_lightingPass.galbedospec = m_gbuffer.getAlbedoSpecAttachmentID();
}

unsigned int Renderer::getTexcolorBufferID()
{
    return m_mainTargetFrameBuffer.getColorAttachmentID();
}

void Renderer::onViewportResize(const Event& e)
{
    const auto& event = static_cast<const ViewportResizeEvent&>(e);
    m_viewportWidth = event.width();
    m_viewportHeight = event.height();
    m_mainTargetFrameBuffer.resizeBuffer(m_viewportWidth, m_viewportHeight);
    m_gbuffer.resizeBuffer(m_viewportWidth, m_viewportHeight);
    
    LOG_INFO("{} {}", m_viewportWidth, m_viewportHeight);
}

void Renderer::setGlobalTextures(Scene& scene)
{
    m_forwardPass.irradianceMap = scene.irradianceMap;
    m_forwardPass.prefilterMap = scene.prefilterMap;
    m_forwardPass.brdfLUT = scene.brdfLUT;
    m_forwardPass.shadowMap = getShadowMapTextureID();
}

unsigned int Renderer::getShadowMapTextureID()
{
    return m_shadowFrameBuffer.getDepthAttachmentID();
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
