#pragma once

#include "../ecs.h"

#include <glm/mat4x4.hpp>

#include <optional>

namespace ui
{
    namespace viewport 
    {
        void draw(unsigned int frameBufferTexcolorID, float& viewportWidth, float& viewportHeight, std::optional<ecs::Entity> selected, glm::mat4& viewMatrix, glm::mat4& projMatrix, int guizmoType);
    }
}
