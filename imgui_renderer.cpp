#include "pch.h"
#include "imgui_renderer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "events/event.h"
#include "events/dispatcher.h"
#include "ui/hierarchy_panel.h"
#include "ui/properties_panel.h"
#include <cstddef>

void ImguiRenderer::init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigWindowsMoveFromTitleBarOnly = true;
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImFont* robotoFont = io.Fonts->AddFontFromFileTTF("resources/fonts/Roboto-Medium.ttf", 16.0f);
    io.FontDefault = robotoFont;

//    ImGuiStyle& style = ImGui::GetStyle();
//    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//    {
//        style.WindowRounding = 0.0f;
//        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//    }
}

void ImguiRenderer::terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImguiRenderer::update(unsigned int frameBufferTexcolorID, Scene& scene) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    // ImGui::ShowDemoWindow();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Viewport");
    ImVec2 viewPortSize = ImGui::GetContentRegionAvail();
    ImGui::Image((void*)(size_t)frameBufferTexcolorID, ImVec2{(float)viewportWidth, (float)viewportHeight}, ImVec2{0, 1}, ImVec2{1, 0});

    if (viewPortSize.x != viewportWidth || viewPortSize.y != viewportHeight)
    {
        viewportWidth = viewPortSize.x;
        viewportHeight = viewPortSize.y;
        ViewportResizeEvent e(viewportWidth, viewportHeight);
        Dispatcher::instance().post(e);
    }
    ImGui::PopStyleVar();

//    auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
//    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
//    auto viewportPos = ImGui::GetWindowPos();
//    glm::vec2 viewportMinBound = { viewportMinRegion.x + viewportPos.x, viewportMinRegion.y + viewportPos.y };
//    glm::vec2 viewportMaxBound = { viewportMaxRegion.x + viewportPos.x, viewportMaxRegion.y + viewportPos.y };

    ImGui::End();

    HierarchyPanel::update(scene);
    if (scene.selected() != nullptr)
    {
        PropertiesPanel::update(scene.selected());
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//    GLFWwindow* backup_current_context = glfwGetCurrentContext();
//    ImGui::UpdatePlatformWindows();
//    ImGui::RenderPlatformWindowsDefault();
//    glfwMakeContextCurrent(backup_current_context);
}

void ImguiRenderer::setBackendImplementation(GLFWwindow *window) {
    const char* glsl_version = "#version 410";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}
