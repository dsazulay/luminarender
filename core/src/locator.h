#pragma once

#include "asset_manager.h"
#include "ecs.h"

class Locator
{
public:
    static ecs::Coordinator* getEcsCoordinator()
    {
        return ecsCoordinator_;
    }

    static AssetManager* getAssetManager()
    {
        return assetManager_;
    }

    static void provide(ecs::Coordinator* ecsCoordinator)
    {
        ecsCoordinator_ = ecsCoordinator;
    }

    static void provide(AssetManager* assetManager)
    {
        assetManager_ = assetManager;
    }
private:
    static inline ecs::Coordinator* ecsCoordinator_;
    static inline AssetManager* assetManager_;
};
