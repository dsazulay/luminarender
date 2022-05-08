#pragma once

#include "../scene.h"

class IRenderable
{
public:
    virtual void render(Scene& scene) = 0;
};