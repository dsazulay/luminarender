#include "mainmenu_system.h"

#include "assets/asset_library.h"
#include "assets/mesh.h"
#include "assets/material.h"
#include "log.h"

#include "tinyfiledialogs.h"

#include <optional>
#include <string>


void MainMenuSystem::init(ecs::Coordinator* coordinator)
{
    m_coordinator = coordinator;
}

void MainMenuSystem::update()
{
    draw();
}

void MainMenuSystem::draw()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            drawMenuFile();
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

void MainMenuSystem::drawMenuFile()
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
            // TODO: create entity and add to the scene
        }
    }
}

void MainMenuSystem::drawMenuEdit()
{
    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
    ImGui::Separator();
    if (ImGui::MenuItem("Cut", "CTRL+X")) {}
    if (ImGui::MenuItem("Copy", "CTRL+C")) {}
    if (ImGui::MenuItem("Paste", "CTRL+V")) {}
}

