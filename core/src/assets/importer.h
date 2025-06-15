#pragma once

#include "model_types.h"

#include <string>
#include <string_view>
#include <vector>

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

TextureData loadTextureFromFile(const std::string &file);
TextureData loadHDRTextureFromFile(const std::string &file);
TextureData loadCubeMapFromFiles(std::vector<std::string> &faces);

ModelData* loadModel(std::string_view path, bool material);
