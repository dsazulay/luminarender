#pragma once

#include "../renderer/gfxapi.h"

#include <string>
#include <vector>
#include <utility>

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

struct Model
{
    std::vector<std::pair<std::string, Mesh>> meshes;
};
