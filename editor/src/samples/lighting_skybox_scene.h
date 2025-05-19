#pragma once

#include "assets/asset_library.h"
#include "ecs.h"


class LightingSkyboxScene
{
public:
    static void loadScene(AssetLibrary& assetLibrary, ecs::Coordinator& coordinator);

private:
    static void loadShaders(AssetLibrary& assetLibrary);
    static void loadTextures(AssetLibrary& assetLibrary);
    static void loadMaterials(AssetLibrary& assetLibrary);
    static void loadModels(AssetLibrary& assetLibrary);
    static void loadLights(ecs::Coordinator& coordinator);
    static void loadSkybox(AssetLibrary& assetLibrary);
    static void loadObjects(AssetLibrary& assetLibrary);
    static void loadObjects(AssetLibrary& assetLibrary, ecs::Coordinator& coordinator);
};
