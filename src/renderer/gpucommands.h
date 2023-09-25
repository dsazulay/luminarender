#pragma once

#include "opengl.h"

template <class T>
class GPUCommands
{
public:
    void setViewportSize(int x, int y, int width, int height)
    {
        backend.setViewportSize(x, y, width, height);
    }

    void setClearColor(float r, float g, float b, float a)
    {
        backend.setClearColor(r, g, b, a);
    }

    void clear(ClearMask mask)
    {
        backend.clear(mask);
    }

    void blit(int width, int height, ClearMask mask, Filtering filtering)
    {
        backend.blit(width, height, mask, filtering);
    }

private:
    T backend;
};
