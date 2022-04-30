#pragma once

#include "pch.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "assets/model.h"
#include "assets/texture.h"

class Importer
{
public:
    static Texture* loadTextureFromFile(const std::string& file, const std::string &directory);
    static Texture* loadCubeMapFromFiles(std::vector<std::string> faces, const std::string &directory);
    static Texture* loadHDRTextureFromFile(const std::string& file, const std::string &directory);

    static Model* loadModel(const char* path);
    static void processNode(aiNode* node, const aiScene* scene, Model* meshes);
    static std::pair<std::vector<Vertex>, std::vector<unsigned int>> processMesh(aiMesh* mesh, const aiScene* scene);
};
