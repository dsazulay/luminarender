#pragma once

#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model;
class Texture;

class Importer
{
public:
    static Model* loadModel(const char* path, bool material);
    static void processNode(aiNode* node, const aiScene* scene, Model* meshes, bool material);
    static VertexIndexTuple processMesh(aiMesh* mesh, const aiScene* scene, bool material);
};

struct MaterialData
{
    std::string name;
    std::string diffusePath;
};

struct MeshData
{
    std::string name;
    VertexIndexTuple vertexIndex;
    MaterialData material;
};

using ModelData = std::vector<MeshData>;

struct TextureData
{
    int width;
    int height;
    int channels;
    std::vector<void*> data;

    void freeData();
};

TextureData loadTextureFromFile(const std::string& file);
TextureData loadHDRTextureFromFile(const std::string& file);
TextureData loadCubeMapFromFiles(std::vector<std::string> &faces);

ModelData* loadModel(std::string_view path, bool material);
