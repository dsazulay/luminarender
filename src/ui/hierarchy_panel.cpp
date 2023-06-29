#include "hierarchy_panel.h"
#include "imgui.h"
#include "../events/event.h"
#include "../events/dispatcher.h"
#include <cstring>

std::string HierarchyPanel::rename = "";
char HierarchyPanel::renameBuffer[32] = "";

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
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    // TODO: change this to use entity's ID instead of name
    if (scene.selected() != nullptr && entity.name() == scene.selected()->name())
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (entity.getChildren().empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (entity.name() == rename)
        {
            ImVec2 p = ImGui::GetCursorScreenPos();

            ImGui::SetKeyboardFocusHere(0);
            ImGui::InputText("##rename", renameBuffer, 32);
            if (ImGui::IsItemDeactivated())
            {
                if (strncmp(renameBuffer, "", 32) != 0)
                {
                    entity.name(renameBuffer);
                }
                renameBuffer[0] = '\0';
                rename = "";
            }

            ImGui::SetCursorPos(p);
        }

        ImGui::TreeNodeEx(entity.name().c_str(), flags, "%s", entity.name().c_str());
        if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
        {
            scene.selected(&entity);
        }
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            rename = entity.name();
        }
    }
    else
    {
        if (entity.name() == rename)
        {
            ImVec2 p = ImGui::GetCursorScreenPos();

            ImGui::SetKeyboardFocusHere(0);
            ImGui::InputText("##rename", renameBuffer, 32);
            if (ImGui::IsItemDeactivated())
            {
                if (strncmp(renameBuffer, "", 32) != 0)
                {
                    entity.name(renameBuffer);
                }
                renameBuffer[0] = '\0';
                rename = "";
            }

            ImGui::SetCursorPos(p);
        }

        bool shouldExpand = ImGui::TreeNodeEx(entity.name().c_str(), flags, "%s", entity.name().c_str());
        if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
        {
            scene.selected(&entity);
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            rename = entity.name();
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
