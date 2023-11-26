#pragma once

#include "../ecs.h"

#include <glm/mat4x4.hpp>

#include <optional>

class ViewportSystem : public ecs::System
{
public:
    void init(ecs::Coordinator* coordinator);
    void update(unsigned int frameBufferTexcolorID, 
            float& viewportWidth, float& viewportHeight, 
            std::optional<ecs::Entity> selected, 
            glm::mat4& viewMatrix, glm::mat4& projMatrix, int guizmoType);
private:
    void draw(unsigned int frameBufferTexcolorID, 
            float& viewportWidth, float& viewportHeight, 
            std::optional<ecs::Entity> selected, 
            glm::mat4& viewMatrix, glm::mat4& projMatrix, int guizmoType);
    void drawGuizmo(std::optional<ecs::Entity> selected, 
            glm::mat4& viewMatrix, glm::mat4& projMatrix, int guizmoType);

    ecs::Coordinator* m_coordinator{};
};

