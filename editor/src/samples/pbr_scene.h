#pragma once

#include "asset_manager.h"
#include "ecs.h"


class PbrScene
{
public:
    static void loadScene(AssetManager& assetManager,
                          ecs::Coordinator& coordinator);

private:
    static void loadIrradianceTextures(unsigned int& skyboxTex,
                                       AssetManager& assetManager);
    static void loadMaterials(AssetManager& assetManager,
                              unsigned int& skyboxTex);
    static void loadLights(ecs::Coordinator& coordinator);
    static void loadSkybox(AssetManager& assetManager);
    static void loadObjects(AssetManager& assetManager,
                            ecs::Coordinator& coordinator);
};
