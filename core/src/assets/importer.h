#pragma once

#include "model_types.h"

#include <string>
#include <string_view>
#include <vector>
#include <optional>

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

std::optional<TextureData> loadTextureFromFile(const std::string &file);
std::optional<TextureData> loadHDRTextureFromFile(const std::string &file);
std::optional<TextureData> loadCubeMapFromFiles(std::vector<std::string> &faces);

ModelData* loadModel(std::string_view path, bool material);
