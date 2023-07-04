#pragma once

class Entity;
class Material;
class MeshRenderer;
class Transform;

namespace ui
{
    namespace hierarchy
    {

    }

    namespace properties
    {
        void draw(Entity* entity);
        void draw(Transform* transform);
        void draw(MeshRenderer* renderer);
        void draw(Material* mat);
    }
}
