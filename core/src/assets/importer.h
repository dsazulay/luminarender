#pragma once

#include "model_types.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <optional>

struct TextureData
{
    int width;
    int height;
    int channels;
    std::vector<void*> data;

    void freeData();
};

enum class ShaderSourceType
{
    NONE,
    VERTEX,
    FRAGMENT,
    GEOMETRY,
};

struct ShaderProperty
{
    std::string type;
    std::string name;
    std::string value;
};

struct ShaderData
{
    std::unordered_map<ShaderSourceType, std::string> shaders;
    std::vector<ShaderProperty> shaderProperties;
};

struct MaterialData
{
    std::string name;
    std::string diffusePath;
    int index;
};

struct MeshData
{
    std::string name;
    VertexIndexTuple vertexIndex;
    MaterialData material;
};

using ModelData = std::vector<MeshData>;


std::optional<TextureData> loadTextureFromFile(const std::string& file);
std::optional<TextureData> loadHDRTextureFromFile(const std::string& file);
std::optional<TextureData> loadCubeMapFromFiles(const std::vector<std::string>& faces);

std::optional<ShaderData> loadShader(std::string_view path);

std::optional<ModelData> loadModel(std::string_view path, bool material);
