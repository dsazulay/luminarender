#pragma once

#include "../scene.h"

class PropertiesPanel
{
public:
    static void update(Entity* entity);
    static const int bufferSize = 256;
    static char renameBuffer[bufferSize];
};
