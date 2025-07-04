#pragma once

#include "asset_manager.h"
#include "ecs.h"


class PbrScene
{
public:
    void loadScene();

private:
    void loadIrradianceTextures(unsigned int& skyboxTex);
    void loadMaterials(unsigned int& skyboxTex);
    void loadLights();
    void loadSkybox();
    void loadObjects();

    ecs::Coordinator* m_ecsCoordinator;
    AssetManager* m_assetManager;
};
