#pragma once

#include "assets.h"

#include <vector>
#include <string>
#include <utility>

class Model
{
public:
    std::vector<std::pair<std::string, Mesh>> m_meshes;
    ~Model();
};
