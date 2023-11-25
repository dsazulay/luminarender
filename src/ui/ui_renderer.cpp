#include "ui_renderer.h"

#include "panel.h"
#include "viewport.h"
#include "../events/event.h"
#include "../events/dispatcher.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>

void UiRenderer::init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImFont* robotoFont = io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Medium.ttf", 16.0f);
    io.FontDefault = robotoFont;

    m_hierarchySystem = m_coordinator.registerSystem<ui::HierarchySystem>().get();
    {
        ecs::Mask mask;
        mask.set(m_coordinator.getComponentType<ecs::Transform>());
        mask.set(m_coordinator.getComponentType<ecs::Tag>());
        m_coordinator.setSystemMask<ui::HierarchySystem>(mask);
    }

    Dispatcher::instance().subscribe(KeyPressEvent::descriptor,
        std::bind(&UiRenderer::onKeyPress, this, std::placeholders::_1));
}

void UiRenderer::terminate() 
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UiRenderer::update(unsigned int frameBufferTexcolorID, Scene& scene, glm::mat4& viewMatrix, glm::mat4& projMatrix)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    ui::mainmenu::draw(&scene);
    ui::viewport::draw(frameBufferTexcolorID, viewportWidth, viewportHeight, scene.selected(), viewMatrix, projMatrix, m_guizmoType);
    //ui::hierarchy::draw(&scene);
    m_hierarchySystem->update(m_coordinator);
    ui::properties::draw(scene.selected());

    // ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UiRenderer::onKeyPress(const Event& e)
{
    const auto& event = static_cast<const KeyPressEvent&>(e);
    int keyCode = event.keyCode();
    int modifier = event.modifier();

    if (modifier != -1)
        return;

    if (keyCode == 87)
        m_guizmoType = ImGuizmo::OPERATION::TRANSLATE;
    else if (keyCode == 69)
        m_guizmoType = ImGuizmo::OPERATION::ROTATE;
    else if (keyCode == 82)
        m_guizmoType = ImGuizmo::OPERATION::SCALE;

}

void UiRenderer::setBackendImplementation(GLFWwindow *window) {
    const char* glsl_version = "#version 410";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}
