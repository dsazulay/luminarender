#pragma once

#include "asset_manager.h"
#include "ecs.h"
#include "components/components.h"

#include <optional>

class PropertiesSystem :public  ecs::System
{
public:
    void init(ecs::Coordinator* coordinator,
            std::optional<ecs::Entity>* selected,
              AssetManager* assetManager);
    void update();
private:
    void draw(ecs::Transform& tranform);
    void draw(ecs::MeshRenderer& meshRenderer);
    void draw(Material* material);

    ecs::Coordinator* m_coordinator{};
    std::optional<ecs::Entity>* m_selected{};
    AssetManager* m_assetManager;
};

