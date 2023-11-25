#include "panel.h"

#include "../assets/asset_library.h"
#include "../scene.h"
#include "../components/transform.h"
#include "../components/mesh_renderer.h"
#include "../assets/mesh.h"
#include "../assets/material.h"
#include "../events/event.h"
#include "../events/dispatcher.h"
#include "../entity_factory.h"
#include "../log.h"

#include "../tinyfiledialogs.h"

#include <string>

std::string ui::HierarchySystem::rename = "";
char ui::HierarchySystem::renameBuffer[ui::HierarchySystem::bufferSize] = "";

void ui::HierarchySystem::update(ecs::Coordinator& coordinator)
{
    ImGui::Begin("Hierarchy");

    if ((ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)) && ImGui::IsWindowHovered())
    {
        //scene->selected(nullptr);
    }

    if (ImGui::BeginPopupContextWindow("Scene Hierarchy Popup"))
    {
        if (ImGui::Selectable("Create Empty"))
        {
             //UiCreateEmptyEvent e{scene->selected()};
             //Dispatcher::instance().post(e);
        }
        ImGui::EndPopup();
    }

    updateRootEntities(coordinator);
    for (auto entity : m_rootEntities)
    {
        draw(entity, coordinator);
    }

    ImGui::End();
}

void ui::HierarchySystem::draw(ecs::Entity entity, ecs::Coordinator& coordinator)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    /*
    // TODO: change this to use entity's ID instead of name
    if (scene->selected() != nullptr && tag.name == scene->selected()->name())
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }*/

    auto& tag = coordinator.getComponent<ecs::Tag>(entity);
    auto& transform = coordinator.getComponent<ecs::Transform>(entity);
    if (tag.name == rename)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();

        ImGui::SetKeyboardFocusHere(0);
        ImGui::InputText("##rename", renameBuffer, bufferSize);
        if (ImGui::IsItemDeactivated())
        {
            if (strncmp(renameBuffer, "", bufferSize) != 0)
            {
                tag.name = renameBuffer;
            }
            renameBuffer[0] = '\0';
            rename = "";
        }

        ImGui::SetCursorPos(p);
    }

    if (transform.children.size() == 0)
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        drawTreeNode(tag.name, flags);

        return;
    }

    if (drawTreeNode(tag.name, flags))
    {
        for (auto child : transform.children)
        {
            draw(child, coordinator);
        }
        ImGui::TreePop();
    }
}

bool ui::HierarchySystem::drawTreeNode(std::string& name, ImGuiTreeNodeFlags flags)
{
    bool shouldExpand = ImGui::TreeNodeEx(name.c_str(), flags, "%s", name.c_str());

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        rename = name;
    }
    if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
    {
        //scene->selected(entity);
    }

    return shouldExpand;
}

void ui::HierarchySystem::updateRootEntities(ecs::Coordinator& coordinator)
{
    m_rootEntities.clear();

    for (auto entity : m_entities)
    {
        auto& transform = coordinator.getComponent<ecs::Transform>(entity);
        if (auto parent = transform.parent)
            continue;    

        m_rootEntities.push_back(entity);
    }
}

namespace ui::mainmenu
{
    void drawMenuFile(Scene* scene);
    void drawMenuEdit();
}

void ui::mainmenu::draw(Scene* scene)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            drawMenuFile(scene);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            drawMenuEdit(); 
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void ui::mainmenu::drawMenuFile(Scene* scene)
{
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        if (ImGui::BeginMenu("More.."))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Sailor");
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As..")) {}

    ImGui::Separator();
    if (ImGui::MenuItem("Import model"))
    {
        const char* selection = tinyfd_openFileDialog("Select File",
                "", 0, nullptr, nullptr, 0);

        if (selection != nullptr)
        {
            // TODO: extract file name + check for name repetition
            auto model = AssetLibrary::instance().loadModel(selection, selection, true);
            auto entity = EntityFactory::createFromModel("name", model);
            scene->addObject(std::move(entity));
        }
    }
}

void ui::mainmenu::drawMenuEdit()
{
    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
    ImGui::Separator();
    if (ImGui::MenuItem("Cut", "CTRL+X")) {}
    if (ImGui::MenuItem("Copy", "CTRL+C")) {}
    if (ImGui::MenuItem("Paste", "CTRL+V")) {}
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
