#pragma once

#include "../ecs.h"
#include "gpucommands.h"
#include "frame_buffer.h"
#include "../assets/material.h"

#include <memory>

class RenderSystem : public ecs::System
{
public:
    void init(int width, int height);
    void update(ecs::Coordinator& coordinator);
    void resizeBuffers(int width, int height);
    id_t getFinalRenderTexID();
private:
    void geometryPass(ecs::Coordinator& coordinator);
    void lightingPass();
    void skyboxPass();
    void normalVisualizerPass(ecs::Coordinator& coordinator);

private:
    GPUResourceManager<OpenGL> m_gpurm{};
    GPUCommands<OpenGL> gpucommands{};
    
    std::unique_ptr<ColorDepthStencilBuffer> m_mainTargetFrameBuffer;
    std::unique_ptr<DepthBuffer> m_shadowFrameBuffer;
    std::unique_ptr<GBuffer> m_gbuffer;

    int m_width;
    int m_height;
};

