#include "hierarchy_panel.h"
#include "imgui.h"
#include "../events/event.h"
#include "../events/dispatcher.h"

void HierarchyPanel::update(Scene &scene)
{
    ImGui::Begin("Scene Hierarchy");

    if ((ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)) && ImGui::IsWindowHovered())
    {
        scene.selected(nullptr);
    }

    if (ImGui::BeginPopupContextWindow("Scene Hierarchy Popup"))
    {
        if (ImGui::Selectable("Create Empty"))
        {
             UiCreateEmptyEvent e{scene.selected()};
             Dispatcher::instance().post(e);
        }
        ImGui::EndPopup();
    }

    for (auto& entity : scene.objects())
    {
        renderEntity(*entity, scene);
    }

    for (auto& entity : scene.lights())
    {
        renderEntity(*entity, scene);
    }

    ImGui::End();
}

void HierarchyPanel::renderEntity(Entity &entity, Scene& scene)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    // TODO: change this to use entity's ID instead of name
    if (scene.selected() != nullptr && entity.name() == scene.selected()->name())
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (entity.getChildren().empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        ImGui::TreeNodeEx(entity.name().c_str(), flags, "%s", entity.name().c_str());
        if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
        {
            scene.selected(&entity);
        }
    }
    else
    {
        bool shouldExpand = ImGui::TreeNodeEx(entity.name().c_str(), flags, "%s", entity.name().c_str());
        if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
        {
            scene.selected(&entity);
        }

        if (shouldExpand)
        {
            for (auto& child : entity.getChildren())
            {
                renderEntity(*child, scene);
            }
            ImGui::TreePop();
        }
    }
}
