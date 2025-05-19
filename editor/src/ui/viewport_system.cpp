#include "viewport_system.h"

#include "components/components.h"
#include "events/event.h"
#include "events/dispatcher.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>


void ViewportSystem::init(ecs::Coordinator* coordinator)
{
    m_coordinator = coordinator;
}

void ViewportSystem::update(unsigned int frameBufferTexcolorID, 
        float& viewportWidth, float& viewportHeight, 
        std::optional<ecs::Entity> selected, 
        glm::mat4& viewMatrix, glm::mat4& projMatrix, int guizmoType)
{
    draw(frameBufferTexcolorID, viewportWidth, viewportHeight,
            selected, viewMatrix, projMatrix, guizmoType);
}

void ViewportSystem::draw(unsigned int frameBufferTexcolorID, 
        float& viewportWidth, float& viewportHeight, 
        std::optional<ecs::Entity> selected, 
        glm::mat4& viewMatrix, glm::mat4& projMatrix, int guizmoType)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Viewport");
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::Image((void*)(size_t)frameBufferTexcolorID, ImVec2{(float)viewportWidth, (float)viewportHeight}, ImVec2{0, 1}, ImVec2{1, 0});

    if (viewportSize.x != viewportWidth || viewportSize.y != viewportHeight)
    {
        viewportWidth = viewportSize.x;
        viewportHeight = viewportSize.y;
        ViewportResizeEvent e(viewportWidth, viewportHeight);
        Dispatcher::instance().post(e);
    }
    ImGui::PopStyleVar();

    drawGuizmo(selected, viewMatrix, projMatrix, guizmoType);

    ImGui::End();
}

void ViewportSystem::drawGuizmo(std::optional<ecs::Entity> selected, 
        glm::mat4& viewMatrix, glm::mat4& projMatrix, int guizmoType)
{
    if (!selected.has_value())
    {
        return;
    }
    
    auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewportPos = ImGui::GetWindowPos();
    glm::vec2 viewportMinBound = { viewportMinRegion.x + viewportPos.x, viewportMinRegion.y + viewportPos.y };
    glm::vec2 viewportMaxBound = { viewportMaxRegion.x + viewportPos.x, viewportMaxRegion.y + viewportPos.y };

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(viewportMinBound.x, viewportMinBound.y,
            viewportMaxBound.x - viewportMinBound.x,
            viewportMaxBound.y - viewportMinBound.y);


    auto& transform = m_coordinator->getComponent<ecs::Transform>(
            selected.value());
    glm::mat4 modelMatrix = transform.modelMatrix;
    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix),
            (ImGuizmo::OPERATION) guizmoType, ImGuizmo::LOCAL,
            const_cast<float *>(glm::value_ptr(modelMatrix)));

    if (ImGuizmo::IsUsing())
    {
        glm::vec3 translation;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::mat4 localMatrix = modelMatrix;
        if (auto parent = transform.parent)
        {
            auto& parentTransform = m_coordinator
                ->getComponent<ecs::Transform>(*parent);
            localMatrix = glm::inverse(parentTransform.modelMatrix) * modelMatrix;
        }

        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(localMatrix),
                glm::value_ptr(translation), glm::value_ptr(rotation),
                glm::value_ptr(scale));
        transform.position = translation;
        glm::vec3 deltaRotation = rotation - transform.rotation;
        transform.rotation = transform.rotation + deltaRotation;
        transform.scale = scale;
    }
}
