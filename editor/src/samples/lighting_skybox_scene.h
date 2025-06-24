#pragma once

#include "asset_manager.h"
#include "ecs.h"


class LightingSkyboxScene
{
public:
    static void loadScene(AssetManager& assetManager,
                          ecs::Coordinator& coordinator);

private:
    static void loadShaders();
    static void loadTextures();
    static void loadMaterials(AssetManager& assetManager);
    static void loadModels();
    static void loadLights(ecs::Coordinator& coordinator);
    static void loadSkybox(AssetManager& assetManager);
    static void loadObjects();
    static void loadObjects(ecs::Coordinator& coordinator);
};
