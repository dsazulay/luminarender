#pragma once

class Scene;
class Entity;

namespace ui
{
    namespace hierarchy
    {
        void draw(Scene* scene);
    }

    namespace properties
    {
        void draw(Entity* entity);
    }
}
