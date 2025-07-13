#include "debug_view.h"

#include "imgui.h"
#include "events/event.h"
#include "events/dispatcher.h"
#include "timer.h"


void DebugView::draw()
{
    ImGui::Begin("Debug");
    if (ImGui::Checkbox("SSAO", &m_ssao))
    {
        UiToggleSSAOEvent e(m_ssao);
        Dispatcher::instance().post(e);
    }
    ImGui::Text("Frame time: %.5fms", Timer::deltaTime);
    ImGui::Text("FPS: %d", (int)(1 / Timer::deltaTime));
    ImGui::End();
}

