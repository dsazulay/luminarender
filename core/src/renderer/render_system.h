#pragma once

#include "../ecs.h"
#include "gfxapi.h"
#include "gpucommands.h"
#include "frame_buffer.h"
#include "../asset_manager.h"
#include "../camera.h"

#include <memory>
#include <string>
#include <glm/vec3.hpp>

class RenderSystem : public ecs::System
{
public:
    void init(int width, int height, Camera* camera);
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
    void transparentPass();
    void normalVisualizerPass();

    void drawMesh(id_t mesh, int indexCount);
    void setAndBindTexture(id_t shader, const std::string& uniform,
                           id_t texture, int slot, TextureTarget target);
    void generateSSAONoiseTexture();

    void setDefaultRenderState();

private:
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

    GPUCommands<OpenGL>* m_gpucommands;
    GPUResourceManager<OpenGL>* m_gpurm;
    ecs::Coordinator* m_coordinator;
    AssetManager* m_assetManager;
    Camera* m_camera;
};

