#pragma once

#include <glm/mat4x4.hpp>

class Entity;

namespace ui
{
    namespace viewport 
    {
        void draw(unsigned int frameBufferTexcolorID, float& viewportWidth, float& viewportHeight, Entity* selected, glm::mat4& viewMatrix, glm::mat4& projMatrix, int guizmoType);
    }
}
