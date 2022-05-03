#pragma once

#include "../asset_library.h"
#include "../scene.h"

class LightingSkyboxScene
{
public:
    static void loadScene(Scene& scene, AssetLibrary& assetLibrary);

private:
    static void loadShaders(AssetLibrary& assetLibrary);
    static void loadTextures(AssetLibrary& assetLibrary);
    static void loadMaterials(AssetLibrary& assetLibrary);
    static void loadModels(AssetLibrary& assetLibrary);
    static void loadLights(Scene& scene);
    static void loadSkybox(Scene& scene, AssetLibrary& assetLibrary);
    static void loadObjects(Scene& scene, AssetLibrary& assetLibrary);
};
