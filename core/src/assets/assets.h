#pragma once

#include "../renderer/gfxapi.h"

#include <glm/vec4.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <utility>

struct Texture
{
    id_t handle;
};

struct Shader
{
    id_t handle;
    int uniformIndex;
};

struct ShaderUniforms
{
    std::vector<std::pair<std::string, float>> floatValues;
    std::vector<std::pair<std::string, glm::vec4>> colorValues;
    std::vector<std::pair<std::string, id_t>> texValues;
};

struct Mesh
{
    id_t handle;
    int indexCount;
    std::string importedMatName;
};

struct Model
{
    std::vector<std::pair<std::string, Mesh>> meshes;
};

struct Material
{
    std::string name;
    Shader shader;
    ShaderUniforms uniforms;

    void setFloat(std::string_view name, float value);
    void setColor(std::string_view name, glm::vec4 value);
    void setTexture(std::string_view name, id_t value);
};

inline void Material::setFloat(std::string_view name, float value)
{
    uniforms.floatValues.push_back(std::make_pair(std::string{ name }, value));
}

inline void Material::setColor(std::string_view name, glm::vec4 value)
{
    uniforms.colorValues.push_back(std::make_pair(std::string{ name }, value));
}

inline void Material::setTexture(std::string_view name, id_t value)
{
    uniforms.texValues.push_back(std::make_pair(std::string{ name }, value));
}
