//
// Created by Diego Azulay on 13/04/22.
//

#include "material.h"
#include "../log.h"

#include <typeindex>

void Material::setProperty(const std::string &name, const std::any& value)
{
    uniforms.insert({name, value});
}

void Material::setUniformData()
{
    for (auto& uniform : uniforms)
    {
        castAndSetUniform(uniform.first, uniform.second);
    }
}

void Material::castAndSetUniform(const std::string &name, std::any value)
{
    const auto it = typeIndexIntMap.find(std::type_index(value.type()));
    switch (it->second) {
        case 0:
            setUniform(name, std::any_cast<bool>(value));
            break;
        case 1:
            setUniform(name, std::any_cast<int>(value));
            break;
        case 2:
            setUniform(name, std::any_cast<unsigned int>(value));
            break;
        case 3:
            setUniform(name, std::any_cast<float>(value));
            break;
        case 4:
            setUniform(name, std::any_cast<glm::vec3>(value));
            break;
        case 5:
            setUniform(name, std::any_cast<glm::vec4>(value));
            break;
        case 6:
            setUniform(name, std::any_cast<glm::mat3>(value));
            break;
        case 7:
            setUniform(name, std::any_cast<glm::mat4>(value));
            break;
        default:
            break;
    }
}

void Material::setUniform(const std::string &name, bool value) const
{
    shader->setBool(name, value);
}

void Material::setUniform(const std::string &name, int value) const
{
    shader->setInt(name, value);
}

void Material::setUniform(const std::string &name, unsigned int value) const
{
    shader->setInt(name, (int)value);
}

void Material::setUniform(const std::string &name, float value) const
{
    shader->setFloat(name, value);
}

void Material::setUniform(const std::string &name, const glm::vec3 &vec) const
{
    shader->setVec3(name, vec);
}

void Material::setUniform(const std::string &name, const glm::vec4 &vec) const
{
    shader->setVec4(name, vec);
}

void Material::setUniform(const std::string &name, const glm::mat3 &mat) const
{
    shader->setMat3(name, mat);
}

void Material::setUniform(const std::string &name, const glm::mat4 &mat) const
{
    shader->setMat4(name, mat);
}

void Material::setTexture(const std::string &name, unsigned int texID, int texUnit)
{
    textures.insert(textures.end(), {name, texID});
}

std::unordered_map<std::string, std::any>& Material::getUniformData() {
    return uniforms;
}

Material::~Material()
{
}

Material::Material(Shader* s)
{
    shader = s;
}
