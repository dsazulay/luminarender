//
// Created by Diego Azulay on 20/04/22.
//

#ifndef INTERACTIVEGRAPHICS_MODEL_H
#define INTERACTIVEGRAPHICS_MODEL_H

#include <vector>
#include "mesh.h"

class Model
{
public:
    std::vector<std::pair<std::string, Mesh*>> m_meshes;
    ~Model();
};


#endif //INTERACTIVEGRAPHICS_MODEL_H
