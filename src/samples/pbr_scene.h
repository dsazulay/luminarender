#pragma once

#include "../scene.h"
#include "../assets/asset_library.h"


class PbrScene
{
public:
    static void loadScene(Scene& scene, AssetLibrary& assetLibrary);

private:
    static void loadShaders(AssetLibrary& assetLibrary);
    static void loadTextures(AssetLibrary& assetLibrary);
    static void loadIrradianceTextures(Scene& scene, AssetLibrary& assetLibrary, unsigned int& skyboxTex);
    static void loadMaterials(AssetLibrary& assetLibrary, unsigned int& skyboxTex);
    static void loadModels(AssetLibrary& assetLibrary);
    static void loadLights(Scene& scene);
    static void loadSkybox(Scene& scene, AssetLibrary& assetLibrary);
    static void loadObjects(Scene& scene, AssetLibrary& assetLibrary);
};
