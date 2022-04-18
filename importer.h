//
// Created by Diego Azulay on 13/04/22.
//

#ifndef INTERACTIVEGRAPHICS_IMPORTER_H
#define INTERACTIVEGRAPHICS_IMPORTER_H

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "components/mesh.h"

class Importer
{
public:
    static unsigned int loadTextureFromFile(const char *path, const std::string &directory);
    static unsigned int loadCubeMapFromFiles(std::vector<std::string> faces, const std::string &directory);
    static unsigned int loadHDRTextureFromFile(const char *path, const std::string &directory);

    static std::vector<Mesh> loadModel(const char* path);
    static void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes);
    static std::pair<std::vector<Vertex>, std::vector<unsigned int>> processMesh(aiMesh* mesh, const aiScene* scene);
};


#endif //INTERACTIVEGRAPHICS_IMPORTER_H
