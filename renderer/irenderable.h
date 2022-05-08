#pragma once

#include "../pch.h"
#include "../entity.h"

class IRenderable
{
public:
    virtual void render(std::list<Entity>& objects) = 0;
};