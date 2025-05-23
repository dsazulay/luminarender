#pragma once

#include "assets/asset_library.h"
#include "assets/asset_catalog.h"
#include "ecs.h"


class PbrScene
{
public:
    static void loadScene(AssetLibrary& assetLibrary,
                          AssetCatalog& assetCatalog,
                          ecs::Coordinator& coordinator);

private:
    static void loadShaders(AssetLibrary& assetLibrary);
    static void loadTextures(AssetLibrary& assetLibrary);
    static void loadIrradianceTextures(AssetLibrary& assetLibrary, unsigned int& skyboxTex);
    static void loadMaterials(AssetLibrary& assetLibrary, unsigned int& skyboxTex);
    static void loadModels(AssetLibrary& assetLibrary);
    static void loadLights(ecs::Coordinator& coordinator);
    static void loadSkybox(AssetLibrary& assetLibrary);
    static void loadObjects(AssetLibrary& assetLibrary,
                            AssetCatalog& assetCatalog,
                            ecs::Coordinator& coordinator);
};
