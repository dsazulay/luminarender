#include "components/transform.h"
#include "entity.h"
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

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../imguizmo/ImGuizmo.h"

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

void ImguiRenderer::update(unsigned int frameBufferTexcolorID, Scene& scene, glm::mat4& viewMatrix, glm::mat4& projMatrix) {
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

    auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewportPos = ImGui::GetWindowPos();
    glm::vec2 viewportMinBound = { viewportMinRegion.x + viewportPos.x, viewportMinRegion.y + viewportPos.y };
    glm::vec2 viewportMaxBound = { viewportMaxRegion.x + viewportPos.x, viewportMaxRegion.y + viewportPos.y };


    // Guizmos
    Entity* selected = scene.selected();
    if (selected != nullptr)
    {
        auto t = selected->getComponent<Transform>();
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(viewportMinBound.x, viewportMinBound.y,
                          viewportMaxBound.x - viewportMinBound.x,
                          viewportMaxBound.y - viewportMinBound.y);


        int guizmoType = ImGuizmo::OPERATION::TRANSLATE;
        // int guizmoType = ImGuizmo::OPERATION::ROTATE;
        // int guizmoType = ImGuizmo::OPERATION::SCALE;
        glm::mat4 modelMatrix = t->modelMatrix();
        ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix),
                             (ImGuizmo::OPERATION) guizmoType, ImGuizmo::LOCAL,
                             const_cast<float *>(glm::value_ptr(modelMatrix)));


        if (ImGuizmo::IsUsing())
        {
            glm::vec3 translation;
            glm::vec3 rotation;
            glm::vec3 scale;

            glm::vec3 oldTranslation;

            glm::mat4 localMatrix = modelMatrix;
            if (selected->getParent() != nullptr)
            {
                localMatrix = glm::inverse(selected->getParent()->getComponent<Transform>()->modelMatrix()) * modelMatrix;
            }

            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(localMatrix), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));
            t->position(translation);
            glm::vec3 deltaRotation = rotation - t->eulerAngles();
            t->eulerAngles(t->eulerAngles() + deltaRotation);
            t->scale(scale);
        }
    }

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
