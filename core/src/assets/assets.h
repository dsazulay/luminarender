#pragma once

#include "../renderer/gfxapi.h"

#include <glm/vec4.hpp>

#include <string>
#include <unordered_map>

struct Texture
{
    id_t handle;
};

struct ShaderUniforms
{
    std::unordered_map<std::string, float> floatValues;
    std::unordered_map<std::string, glm::vec4> colorValues;
    std::unordered_map<std::string, id_t> texValues;
};

struct Shader
{
    id_t handle;
    ShaderUniforms uniformsDefaultValues;
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
    Shader* shader;
    ShaderUniforms uniforms;

    void setFloat(const std::string& name, float value);
    void setColor(const std::string& name, glm::vec4 value);
    void setTexture(const std::string& name, id_t value);
};

inline void Material::setFloat(const std::string& name, float value)
{
    uniforms.floatValues[name] = value;
}

inline void Material::setColor(const std::string& name, glm::vec4 value)
{
    uniforms.colorValues[name] = value;
}

inline void Material::setTexture(const std::string& name, id_t value)
{
    uniforms.texValues[name] = value;
}
