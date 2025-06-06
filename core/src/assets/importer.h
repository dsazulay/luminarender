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
    static Texture* loadTextureFromFile(const std::string& file, const std::string &directory);
    static Texture* loadCubeMapFromFiles(std::vector<std::string> faces, const std::string &directory);
    static Texture* loadHDRTextureFromFile(const std::string& file, const std::string &directory);

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
    unsigned char* data;

    void freeData();
};

TextureData loadTextureFromFile(std::string_view file);
ModelData* loadModel(std::string_view path, bool material);

