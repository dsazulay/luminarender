#pragma once

#include "../scene.h"
#include <string>

class HierarchyPanel
{
public:
    static void update(Scene& scene);
    static void renderEntity(Entity& entity, Scene& scene);
    static std::string rename;
    static char renameBuffer[32];
};
