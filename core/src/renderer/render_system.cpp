#include "render_system.h"

#include "../components/components.h"
#include "../irradiance_map_factory.h"
#include "gfxapi.h"
#include "../locator.h"

#include <random>
#include <array>
#include <format>

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void RenderSystem::init(int width, int height, Camera* camera)
{
    m_gpucommands = Locator::getGpuCommands();
    m_gpurm = Locator::getGpuResourceManager();
    m_coordinator = Locator::getEcsCoordinator();
    m_assetManager = Locator::getAssetManager();
    m_width = width;
    m_height = height;
    m_camera = camera;

    m_mainTargetFrameBuffer = std::make_unique<ColorDepthStencilBuffer>(m_width, m_height);
    m_shadowFrameBuffer = std::make_unique<DepthBuffer>(shadowMapSize, shadowMapSize);
    m_gbuffer = std::make_unique<GBuffer>(m_width, m_height);
    m_ssaoBuffer = std::make_unique<ColorBuffer>(m_width, m_height);
    m_ssaoBlurBuffer = std::make_unique<ColorBuffer>(m_width, m_height);

    // TODO: Move this to asset initalization
    Shader* s = &m_assetManager->getShader("resources/shaders/gbuffer.glsl");
    m_assetManager->createMaterial("GBuffer", s);
    s = &m_assetManager->getShader("resources/shaders/lightingpass.glsl");
    m_assetManager->createMaterial("LightingPass", s);
    s = &m_assetManager->getShader("resources/shaders/simple_shadow_depth.glsl");
    m_assetManager->createMaterial("shadowMat", s);
    s = &m_assetManager->getShader("resources/shaders/ssao.glsl");
    m_assetManager->createMaterial("ssaoMat", s);
    s = &m_assetManager->getShader("resources/shaders/ssao_blur.glsl");
    m_assetManager->createMaterial("ssaoBlurMat", s);
    generateSSAONoiseTexture();
    s = &m_assetManager->getShader("resources/shaders/normal_vector.glsl");
    m_assetManager->createMaterial("normalVector", s);

    setDefaultRenderState();
}

void RenderSystem::setDefaultRenderState()
{
    m_gpucommands->enable(Capability::CULL);
    m_gpucommands->enable(Capability::DEPTHTEST);
    m_gpucommands->setDepthFunction(DepthFunction::LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    m_gpucommands->enable(Capability::CUBEMAP_SEAMLESS);
}

void RenderSystem::update()
{
    shadowPass();
    geometryPass();
    if (m_ssaoEnabled) {
        ssaoPass();
        ssaoBlurPass();
    }
    lightingPass();

    // copy depth buffer from geometry pass
    m_gbuffer->bind(FrameBufferOp::READ);
    m_mainTargetFrameBuffer->bind(FrameBufferOp::WRITE); 
    m_gpucommands->blit(m_width, m_height, ClearMask::DEPTH, Filtering::POINT);

    skyboxPass();
    //normalVisualizerPass();
}

void RenderSystem::updateIrradianceMaps()
{
    IrradianceMaps maps = IrradianceMapFactory::generateIrradianceMapsFromHDR(m_assetManager->getTextureHDR("resources/textures/skybox/tiber_2.hdr").handle, *m_assetManager);
    m_irradianceMap = maps.irradianceMap;
    m_prefilterMap = maps.prefilterMap;
    m_brdfLUT = maps.brdfLUTMap;
}

void RenderSystem::resizeBuffers(int width, int height)
{
    m_width = width;
    m_height = height;
    m_mainTargetFrameBuffer->resizeBuffer(m_width, m_height);
    m_gbuffer->resizeBuffer(m_width, m_height);
    m_ssaoBuffer->resizeBuffer(m_width, m_height);
    m_ssaoBlurBuffer->resizeBuffer(m_width, m_height);
}

void RenderSystem::toggleSSAO(bool enabled)
{
    m_ssaoEnabled = enabled;
}

id_t RenderSystem::getFinalRenderTexID()
{
    return m_mainTargetFrameBuffer->getColorAttachmentID();
}

void RenderSystem::shadowPass()
{
    m_shadowFrameBuffer->bind();
    m_gpucommands->setViewportSize(0, 0, shadowMapSize, shadowMapSize);
    m_gpucommands->clear(ClearMask::DEPTH);

    for (auto entity : m_entities)
    {
        auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
        auto& meshRenderer = m_coordinator->getComponent<ecs::MeshRenderer>(
                entity);

        Material material = m_assetManager->getMaterial("shadowMat");
        id_t shader = material.shader->handle;
        m_gpurm->bindShader(shader);

        m_gpurm->setUniform(shader, "u_model", transform.modelMatrix);

        drawMesh(meshRenderer.mesh.handle, meshRenderer.mesh.indexCount);
    }

    m_shadowFrameBuffer->unbind();
}

void RenderSystem::geometryPass()
{
    m_gbuffer->bind();
    m_gpucommands->setViewportSize(0, 0, m_width, m_height);
    m_gpucommands->setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_gpucommands->clear(ClearMask::COLORDEPTH);

    for (auto entity : m_entities)
    {
        auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
        auto& meshRenderer = m_coordinator->getComponent<ecs::MeshRenderer>(
                entity);

        Material& origMaterial = *meshRenderer.material;
        Material& material = m_assetManager->getMaterial("GBuffer");
        id_t shader = material.shader->handle;
        m_gpurm->bindShader(shader);

        m_gpurm->setUniform(shader, "u_model", transform.modelMatrix);
        m_gpurm->setUniform(shader, "u_normalMatrix", glm::transpose(
            glm::inverse(glm::mat3(transform.modelMatrix))));

        for (auto& kv : origMaterial.uniforms.floatValues) {
            m_gpurm->setUniform(shader, kv.first.c_str(), kv.second);
        }

        for (auto& kv : origMaterial.uniforms.colorValues) {
            m_gpurm->setUniform(shader, kv.first.c_str(), kv.second);
        }

        int texCount = 0;
        for (auto& kv : origMaterial.uniforms.texValues) {
            setAndBindTexture(shader, kv.first, kv.second, texCount,
                              TextureTarget::TEX2D);
            ++texCount;
        }

        drawMesh(meshRenderer.mesh.handle, meshRenderer.mesh.indexCount);
    }

    m_gbuffer->unbind();
}

void RenderSystem::ssaoPass()
{
    m_ssaoBuffer->bind();
    m_gpucommands->setViewportSize(0, 0, m_width, m_height);
    m_gpucommands->setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_gpucommands->clear(ClearMask::COLOR);

    Mesh mesh = m_assetManager->getMesh(MeshType::Quad);
    Material material = m_assetManager->getMaterial("ssaoMat");
    id_t shader = material.shader->handle;
    m_gpurm->bindShader(shader);

    for (unsigned int i = 0; i < 64; ++i) {
        m_gpurm->setUniform(shader, std::format("u_samples[{}]", i).c_str(), m_ssaoKernel[i]);
    }

    setAndBindTexture(shader, "u_depth", m_gbuffer->getDepthAttachmentID(),
                      0, TextureTarget::TEX2D);
    setAndBindTexture(shader, "u_normal", m_gbuffer->getNormalAttachmentID(),
                      1, TextureTarget::TEX2D);
    setAndBindTexture(shader, "u_noise", m_ssaoNoiseTex,
                      2, TextureTarget::TEX2D);

    drawMesh(mesh.handle, mesh.indexCount);

    m_ssaoBuffer->unbind();
}

void RenderSystem::ssaoBlurPass()
{
    m_ssaoBlurBuffer->bind();
    m_gpucommands->setViewportSize(0, 0, m_width, m_height);
    m_gpucommands->setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_gpucommands->clear(ClearMask::COLOR);

    Mesh mesh = m_assetManager->getMesh(MeshType::Quad);
    Material material = m_assetManager->getMaterial("ssaoBlurMat");
    id_t shader = material.shader->handle;
    m_gpurm->bindShader(shader);

    for (unsigned int i = 0; i < 64; ++i) {
        m_gpurm->setUniform(shader, std::format("u_samples[{}]", i).c_str(), m_ssaoKernel[i]);
    }
    setAndBindTexture(shader, "u_ssao", m_ssaoBuffer->getColorAttachmentID(),
                      0, TextureTarget::TEX2D);

    drawMesh(mesh.handle, mesh.indexCount);

    m_ssaoBuffer->unbind();
}

void RenderSystem::lightingPass()
{
    m_mainTargetFrameBuffer->bind();
    m_gpucommands->setViewportSize(0, 0, m_width, m_height);
    m_gpucommands->setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_gpucommands->clear(ClearMask::COLORDEPTH);

    Mesh mesh = m_assetManager->getMesh(MeshType::Quad);
    Material material = m_assetManager->getMaterial("LightingPass");
    id_t shader = material.shader->handle;
    m_gpurm->bindShader(shader);

    setAndBindTexture(shader, "u_depth", m_gbuffer->getDepthAttachmentID(),
                      0, TextureTarget::TEX2D);
    setAndBindTexture(shader, "u_normal", m_gbuffer->getNormalAttachmentID(),
                      1, TextureTarget::TEX2D);
    setAndBindTexture(shader, "u_albedo",
                      m_gbuffer->getAlbedoSpecAttachmentID(),
                      2, TextureTarget::TEX2D);
    setAndBindTexture(shader, "u_shadowMap",
                      m_shadowFrameBuffer->getDepthAttachmentID(),
                      3, TextureTarget::TEX2D);
    id_t ssaoTex;
    if (m_ssaoEnabled) {
        ssaoTex = m_ssaoBlurBuffer->getColorAttachmentID();
    }
    else {
        ssaoTex = m_assetManager->getTexture2D(
            "resources/textures/default_white.png").handle;
    }
    setAndBindTexture(shader, "u_ssao", ssaoTex, 4, TextureTarget::TEX2D);
    setAndBindTexture(shader, "u_orm", m_gbuffer->getPositionAttachmentID(),
                      5, TextureTarget::TEX2D);

    // bind global illumination textures
    setAndBindTexture(shader, "u_irradianceTex", m_irradianceMap, 10,
                      TextureTarget::CUBEMAP);
    setAndBindTexture(shader, "u_prefilterMap", m_prefilterMap, 11,
                      TextureTarget::CUBEMAP);
    setAndBindTexture(shader, "u_brdfLUT", m_brdfLUT, 12,
                      TextureTarget::TEX2D);

    drawMesh(mesh.handle, mesh.indexCount);

    m_mainTargetFrameBuffer->unbind();
}

void RenderSystem::skyboxPass()
{
    m_mainTargetFrameBuffer->bind();

    Mesh mesh = m_assetManager->getMesh(MeshType::TriangleMap);
    Material material = m_assetManager->getMaterial("skyboxMat");
    id_t shader = material.shader->handle;
    m_gpurm->bindShader(shader);

    int texCount = 0;
    for (auto& kv : material.uniforms.texValues) {
        setAndBindTexture(shader, kv.first, kv.second, texCount,
                          TextureTarget::CUBEMAP);
        ++texCount;
    }

    drawMesh(mesh.handle, mesh.indexCount);

    m_mainTargetFrameBuffer->unbind();
}

void RenderSystem::normalVisualizerPass()
{
    m_mainTargetFrameBuffer->bind();

    for (auto entity : m_entities)
    {
        auto& transform = m_coordinator->getComponent<ecs::Transform>(entity);
        auto& meshRenderer = m_coordinator->getComponent<ecs::MeshRenderer>(
                entity);

        Material material = m_assetManager->getMaterial("normalVector");
        id_t shader = material.shader->handle;
        m_gpurm->bindShader(shader);

        m_gpurm->setUniform(shader, "u_model", transform.modelMatrix);
        m_gpurm->setUniform(shader, "u_normalMatrix", glm::transpose(glm::inverse(
            glm::mat3(m_camera->getViewMatrix() * transform.modelMatrix))));

        for (auto& kv : material.uniforms.floatValues) {
            m_gpurm->setUniform(shader, kv.first.c_str(), kv.second);
        }

        for (auto& kv : material.uniforms.colorValues) {
            m_gpurm->setUniform(shader, kv.first.c_str(), kv.second);
        }

        int texCount = 0;
        for (auto& kv : material.uniforms.texValues) {
            setAndBindTexture(shader, kv.first, kv.second, texCount,
                              TextureTarget::TEX2D);
            ++texCount;
        }

        drawMesh(meshRenderer.mesh.handle, meshRenderer.mesh.indexCount);
    }

    m_mainTargetFrameBuffer->unbind();
}

void RenderSystem::drawMesh(id_t mesh, int indexCount)
{
    m_gpurm->bindMesh(mesh);
    m_gpurm->drawTriangleElements(indexCount);
    m_gpurm->unbindMesh();
}

void RenderSystem::setAndBindTexture(
    id_t shader, const std::string& uniform, id_t texture, 
    int slot, TextureTarget target)
{
    m_gpurm->setUniform(shader, uniform.c_str(), slot);
    m_gpurm->activeTexture(slot);
    m_gpurm->bindTexture(texture, target);
}

// TODO: move this to asset library init
void RenderSystem::generateSSAONoiseTexture()
{
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;

    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        m_ssaoKernel[i] = sample;
    }

    std::array<glm::vec3, 16> ssaoNoise;
    for (unsigned int i = 0; i < 16; ++i)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise[i] = noise;
    }

    m_ssaoNoiseTex = m_gpurm->createTexture({
        .width = 4,
        .height = 4,
        .format = Format::RGB,
        .byteFormat = ByteFormat::RGBA32F,
        .filtering = Filtering::POINT,
        .type = TexType::FLOAT,
        .initialData = ssaoNoise.data(),
    });
}
