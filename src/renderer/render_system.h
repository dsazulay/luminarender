#pragma once

#include "../ecs.h"
#include "gpucommands.h"
#include "frame_buffer.h"
#include "../assets/material.h"

#include <memory>
#include <array>
#include <glm/vec3.hpp>

class RenderSystem : public ecs::System
{
public:
    void init(int width, int height, ecs::Coordinator* coordinator);
    void update();
    void resizeBuffers(int width, int height);
    id_t getFinalRenderTexID();
private:
    void shadowPass();
    void geometryPass();
    void ssaoPass();
    void ssaoBlurPass();
    void lightingPass();
    void skyboxPass();
    void normalVisualizerPass(ecs::Coordinator& coordinator);

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
    std::array<glm::vec3, 64> m_ssaoKernel;

    ecs::Coordinator* m_coordinator;
};

