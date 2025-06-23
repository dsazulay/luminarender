#pragma once

#include "../renderer/gfxapi.h"

struct Texture
{
    id_t handle;
};

struct Mesh
{
    id_t handle;
    int indexCount;
    std::string importedMatName;
};
