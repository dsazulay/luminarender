#pragma once

#include "ecs.h"

#include "imgui.h"

#include <optional>

class MainMenuSystem : public ecs::System
{
public:
    void init(ecs::Coordinator* coordinator);
    void update();
private:
    void draw();
    void drawMenuFile();
    void drawMenuEdit();

    ecs::Coordinator* m_coordinator{};
};

