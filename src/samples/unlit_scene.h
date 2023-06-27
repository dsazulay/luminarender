#pragma once

#include "../scene.h"
#include "../asset_library.h"

class UnlitScene
{
public:
    static void loadScene(Scene& scene, AssetLibrary& assetLibrary);

private:
    static void loadShaders(AssetLibrary& assetLibrary);
    static void loadTextures(AssetLibrary& assetLibrary);
    static void loadMaterials(AssetLibrary& assetLibrary);
    static void loadModels(AssetLibrary& assetLibrary);
    static void loadObjects(Scene& scene, AssetLibrary& assetLibrary);
};
