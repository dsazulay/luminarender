#pragma once

class Scene;
class Entity;

namespace ui
{
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
