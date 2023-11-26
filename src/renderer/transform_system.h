#pragma once

#include "../ecs.h"
#include "../components/components.h"

#include <vector>

class TransformSystem : public ecs::System
{
public:
    void init(ecs::Coordinator* coordinator);
    void update();
    void updateHierarchically();
    void updateModelMatrix(ecs::Entity entity);
    void updateModelMatrix(ecs::Transform& transform);
    void updateFromModelMatrix(ecs::Entity entity);
    void updateSeflAndChilren(ecs::Entity entity);
    void updateRootEntities();
    void addChild(ecs::Entity parent, ecs::Entity child);
private:
    void updateFromModelMatrix(ecs::Transform& transform);

    std::vector<ecs::Entity> m_rootEntities;
    ecs::Coordinator* m_coordinator;
};
