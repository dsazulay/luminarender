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

    m_hierarchySystem = m_coordinator->registerSystem<HierarchySystem>().get();
    {
        ecs::Mask mask;
        mask.set(m_coordinator->getComponentType<ecs::Transform>());
        mask.set(m_coordinator->getComponentType<ecs::Tag>());
        m_coordinator->setSystemMask<HierarchySystem>(mask);
    }
    m_hierarchySystem->init(m_coordinator, &m_selected);
    m_properiesSystem = new PropertiesSystem();
    m_properiesSystem->init(m_coordinator, &m_selected);

    Dispatcher::instance().subscribe(KeyPressEvent::descriptor,
        std::bind(&UiRenderer::onKeyPress, this, std::placeholders::_1));
}

void UiRenderer::terminate() 
{
    delete m_properiesSystem;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UiRenderer::update(unsigned int frameBufferTexcolorID, glm::mat4& viewMatrix, glm::mat4& projMatrix)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    ui::mainmenu::draw();
    ui::viewport::draw(frameBufferTexcolorID, viewportWidth, viewportHeight, m_selected, viewMatrix, projMatrix, m_guizmoType);
    m_hierarchySystem->update();
    m_properiesSystem->update();

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
