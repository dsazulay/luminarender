#pragma once

#include "ecs.h"
#include "components/components.h"

#include <imgui.h>

#include <optional>

class HierarchySystem : public ecs::System
{
public:
    void init(ecs::Coordinator* coordinator, 
            std::optional<ecs::Entity>* selected);
    void update();
private:
    void draw(ecs::Entity entity);
    bool drawTreeNode(ecs::Entity entity, std::string& name, 
            ImGuiTreeNodeFlags flags);

    void updateRootEntities();

    std::vector<ecs::Entity> m_rootEntities;
    ecs::Coordinator* m_coordinator{};
    std::optional<ecs::Entity>* m_selected{};

    static const int bufferSize = 256;
    static std::string rename;
    static char renameBuffer[bufferSize];
};

