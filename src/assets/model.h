#pragma once

#include "../pch.h"
#include "mesh.h"

class Model
{
public:
    std::vector<std::pair<std::string, Mesh*>> m_meshes;
    ~Model();
};
