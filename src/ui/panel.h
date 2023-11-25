#pragma once

#include "../ecs.h"
#include "../components/components.h"

#include "imgui.h"

class Scene;
class Entity;

namespace ui
{
    class HierarchySystem : public ecs::System
    {
    public:
        void update(ecs::Coordinator& coordinator);
    private:
        void draw(ecs::Tag& tag);
        bool drawTreeNode(std::string& name, ImGuiTreeNodeFlags flags);

        static const int bufferSize = 256;
        static std::string rename;
        static char renameBuffer[bufferSize];   
    };

    namespace mainmenu
    {
        void draw(Scene* scene);
    }

    namespace hierarchy
    {
        void draw(Scene* scene);
    }

    namespace properties
    {
        void draw(Entity* entity);
    }
}
