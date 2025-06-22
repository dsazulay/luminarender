#pragma once

#include <vector>
#include <string>
#include <utility>

class Mesh;

class Model
{
public:
    std::vector<std::pair<std::string, Mesh*>> m_meshes;
    ~Model();
};
