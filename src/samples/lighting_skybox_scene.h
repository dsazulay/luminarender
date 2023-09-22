#pragma once

#include "../scene.h"
#include "../assets/asset_library.h"
#include "../ecs.h"


class LightingSkyboxScene
{
public:
    static void loadScene(Scene& scene, AssetLibrary& assetLibrary, ecs::Coordinator& coordinator);

private:
    static void loadShaders(AssetLibrary& assetLibrary);
    static void loadTextures(AssetLibrary& assetLibrary);
    static void loadMaterials(AssetLibrary& assetLibrary);
    static void loadModels(AssetLibrary& assetLibrary);
    static void loadLights(Scene& scene, ecs::Coordinator& coordinator);
    static void loadSkybox(Scene& scene, AssetLibrary& assetLibrary);
    static void loadObjects(Scene& scene, AssetLibrary& assetLibrary);
    static void loadObjects(Scene& scene, AssetLibrary& assetLibrary, ecs::Coordinator& coordinator);
};
