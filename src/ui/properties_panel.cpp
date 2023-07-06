#include "properties_panel.h"

#include "../asset_library.h"
#include "../scene.h"
#include "../components/transform.h"
#include "../components/mesh_renderer.h"
#include "../assets/mesh.h"
#include "../assets/material.h"
#include "../events/event.h"
#include "../events/dispatcher.h"

#include "imgui.h"

#include <string>

namespace ui::hierarchy
{
    static const int bufferSize = 256;
    static std::string rename = "";
    static char renameBuffer[bufferSize] = "";   
    void draw(Scene* scene, Entity* entity);
    bool drawTreeNode(Scene* scene, Entity* entity, ImGuiTreeNodeFlags flags);
}

void ui::hierarchy::draw(Scene* scene)
{
    ImGui::Begin("Hierarchy");

    if ((ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)) && ImGui::IsWindowHovered())
    {
        scene->selected(nullptr);
    }

    if (ImGui::BeginPopupContextWindow("Scene Hierarchy Popup"))
    {
        if (ImGui::Selectable("Create Empty"))
        {
             UiCreateEmptyEvent e{scene->selected()};
             Dispatcher::instance().post(e);
        }
        ImGui::EndPopup();
    }

    for (auto& entity : scene->objects())
    {
        draw(scene, entity.get());
    }

    for (auto& entity : scene->lights())
    {
        draw(scene, entity.get());
    }

    ImGui::End();
}

void ui::hierarchy::draw(Scene* scene, Entity* entity)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    // TODO: change this to use entity's ID instead of name
    if (scene->selected() != nullptr && entity->name() == scene->selected()->name())
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (entity->name() == rename)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();

        ImGui::SetKeyboardFocusHere(0);
        ImGui::InputText("##rename", renameBuffer, bufferSize);
        if (ImGui::IsItemDeactivated())
        {
            if (strncmp(renameBuffer, "", bufferSize) != 0)
            {
                entity->name(renameBuffer);
            }
            renameBuffer[0] = '\0';
            rename = "";
        }

        ImGui::SetCursorPos(p);
    }

    if (entity->getChildren().empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        drawTreeNode(scene, entity, flags);
        return;
    }

    if (drawTreeNode(scene, entity, flags))
    {
        for (auto& child : entity->getChildren())
        {
            draw(scene, child.get());
        }
        ImGui::TreePop();
    }
}

bool ui::hierarchy::drawTreeNode(Scene* scene, Entity* entity, ImGuiTreeNodeFlags flags)
{
    bool shouldExpand = ImGui::TreeNodeEx(entity->name().c_str(), flags, "%s", entity->name().c_str());

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        rename = entity->name();
    }
    if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
    {
        scene->selected(entity);
    }

    return shouldExpand;
}

namespace ui::properties
{
    void draw(Transform* transform);
    void draw(MeshRenderer* renderer);
    void draw(Material* mat);
}

void ui::properties::draw(Entity* entity)
{
    ImGui::Begin("Properties");
    if (entity == nullptr)
    {
        ImGui::End();
        return;
    }
    
    static const int bufferSize = 256;
    static char renameBuffer[bufferSize] = "";
    strncpy(renameBuffer, entity->name().c_str(), bufferSize);
    ImGui::InputText("Name", renameBuffer, bufferSize);
    if (ImGui::IsItemDeactivated())
    {
        if (strncmp(renameBuffer, "", bufferSize) != 0)
            entity->name(renameBuffer);
    }

    auto transform = entity->getComponent<Transform>();
    ui::properties::draw(transform.get());
    entity->updateSelfAndChild();
    
    auto meshRenderer = entity->getComponent<MeshRenderer>();
    ui::properties::draw(meshRenderer.get());

    if (ImGui::BeginPopupContextWindow("Add Comp Popup"))
    {
        if (ImGui::Selectable("Mesh"))
        {
            MeshRenderer m;
            m.mesh = nullptr;
            m.material = nullptr;
            entity->addComponent(m);
        }
        if (ImGui::Selectable("Light"))
        {
            LOG_INFO("Add light");
        }
        ImGui::EndPopup();
    }
   
    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("Add Comp Popup");
    }

    ImGui::End();
}

void ui::properties::draw(Transform* transform)
{
    if (ImGui::TreeNodeEx("transform", ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
    {
        glm::vec3 pos = transform->position();
        ImGui::DragFloat3("Position", &pos.x, 0.1f);
        glm::vec3 rot = transform->eulerAngles();
        ImGui::DragFloat3("Rotation", &rot.x, 0.1f);
        glm::vec3 scale = transform->scale();
        ImGui::DragFloat3("Scale", &scale.x, 0.1f);

        transform->position(pos);
        transform->eulerAngles(rot);
        transform->scale(scale);

        transform->updateModelMatrix();

        ImGui::TreePop();
    }
}

void ui::properties::draw(MeshRenderer* renderer)
{
    if (renderer == nullptr)
        return;
    
    if (ImGui::TreeNodeEx("meshRenderer", ImGuiTreeNodeFlags_DefaultOpen, "Mesh Renderer"))
    {
        const char* items[] = { "None", "Quad", "Cube", "Sphere", "Custom" };
        int currentItem = (int) renderer->meshType();
        ImGui::Combo("Mesh Type", &currentItem, items, 4);

        if (((int) renderer->meshType()) == currentItem)
        {
            draw(renderer->material);
            ImGui::TreePop();
            return;
        }

        switch (currentItem)
        {
            case 0:
                renderer->mesh = nullptr;
                break;
            case 1:
                renderer->mesh = AssetLibrary::instance().getMesh("quad");
                renderer->mesh->meshType(MeshType::Quad);
                renderer->material = AssetLibrary::instance().getMaterial("defaultPbr");
                break;
            case 2:
                renderer->mesh = AssetLibrary::instance().getMesh("cube");
                renderer->mesh->meshType(MeshType::Cube);
                renderer->material = AssetLibrary::instance().getMaterial("defaultPbr");
                break;
            case 3:
                renderer->mesh = AssetLibrary::instance().getMesh("sphere");
                renderer->mesh->meshType(MeshType::Sphere);
                renderer->material = AssetLibrary::instance().getMaterial("defaultPbr");
                break;
 
        }

        draw(renderer->material);
        ImGui::TreePop();
    }
}


void ui::properties::draw(Material* mat)
{
    if (mat == nullptr)
        return;

    if (ImGui::TreeNodeEx("material", ImGuiTreeNodeFlags_DefaultOpen, "Material"))
    {
        ImGui::Text("%s", mat->name().c_str());
        ImGui::TreePop();
    }
}
