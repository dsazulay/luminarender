#pragma once

#include "../ecs.h"
#include "gpucommands.h"
#include "frame_buffer.h"
#include "../assets/material.h"
#include "../camera.h"

#include <memory>
#include <array>
#include <glm/vec3.hpp>

class RenderSystem : public ecs::System
{
public:
    void init(int width, int height, ecs::Coordinator* coordinator, Camera* camera);
    void update();
    void updateIrradianceMaps();
    void resizeBuffers(int width, int height);
    void toggleSSAO(bool enabled);
    id_t getFinalRenderTexID();
private:
    void shadowPass();
    void geometryPass();
    void ssaoPass();
    void ssaoBlurPass();
    void lightingPass();
    void skyboxPass();
    void normalVisualizerPass();

    void generateSSAONoiseTexture();

private:
    GPUResourceManager<OpenGL> m_gpurm{};
    GPUCommands<OpenGL> gpucommands{};

    std::unique_ptr<ColorDepthStencilBuffer> m_mainTargetFrameBuffer;
    std::unique_ptr<DepthBuffer> m_shadowFrameBuffer;
    std::unique_ptr<GBuffer> m_gbuffer;
    std::unique_ptr<ColorBuffer> m_ssaoBuffer;
    std::unique_ptr<ColorBuffer> m_ssaoBlurBuffer;

    int m_width;
    int m_height;

    const int shadowMapSize = 1024;

    id_t m_ssaoNoiseTex;
    id_t m_irradianceMap;
    id_t m_prefilterMap;
    id_t m_brdfLUT;
    std::array<glm::vec3, 64> m_ssaoKernel;

    bool m_ssaoEnabled = true;

    ecs::Coordinator* m_coordinator;
    Camera* m_camera;
};

