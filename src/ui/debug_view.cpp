#include "debug_view.h"

#include "imgui.h"
#include "../log.h"


void DebugView::draw()
{
    ImGui::Begin("Debug");
    ImGui::Checkbox("SSAO", &m_ssao);
    ImGui::End();
}

