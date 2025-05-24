#pragma once

#include "assets/asset_library.h"
#include "asset_manager.h"
#include "ecs.h"


class PbrScene
{
public:
    static void loadScene(AssetLibrary& assetLibrary,
                          AssetManager& assetManager,
                          ecs::Coordinator& coordinator);

private:
    static void loadShaders(AssetLibrary& assetLibrary);
    static void loadTextures(AssetLibrary& assetLibrary);
    static void loadIrradianceTextures(AssetLibrary& assetLibrary,
                                       unsigned int& skyboxTex,
                                       AssetManager& assetManager);
    static void loadMaterials(AssetLibrary& assetLibrary, unsigned int& skyboxTex);
    static void loadModels(AssetLibrary& assetLibrary);
    static void loadLights(ecs::Coordinator& coordinator);
    static void loadSkybox(AssetLibrary& assetLibrary, AssetManager& assetManager);
    static void loadObjects(AssetLibrary& assetLibrary,
                            AssetManager& assetManager,
                            ecs::Coordinator& coordinator);
};
