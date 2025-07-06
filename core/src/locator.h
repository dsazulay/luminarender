#pragma once

#include "asset_manager.h"
#include "ecs.h"
#include "renderer/gpucommands.h"
#include "renderer/gpuresourcemanager.h"
#include "renderer/opengl.h"

class Locator
{
public:
    static void provide(GPUCommands<OpenGL>* gpucommands)
    {
        gpucommands_ = gpucommands;
    }

    static void provide(GPUResourceManager<OpenGL>* gpurm)
    {
        gpurm_ = gpurm;
    }

    static void provide(ecs::Coordinator* ecsCoordinator)
    {
        ecsCoordinator_ = ecsCoordinator;
    }

    static void provide(AssetManager* assetManager)
    {
        assetManager_ = assetManager;
    }

    static GPUCommands<OpenGL>* getGpuCommands()
    {
        return gpucommands_;
    }

    static GPUResourceManager<OpenGL>* getGpuResourceManager()
    {
        return gpurm_;
    }

    static ecs::Coordinator* getEcsCoordinator()
    {
        return ecsCoordinator_;
    }

    static AssetManager* getAssetManager()
    {
        return assetManager_;
    }


private:
    static inline GPUCommands<OpenGL>* gpucommands_;
    static inline GPUResourceManager<OpenGL>* gpurm_;
    static inline ecs::Coordinator* ecsCoordinator_;
    static inline AssetManager* assetManager_;
};
