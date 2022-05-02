#include "material.h"
#include "../asset_library.h"

void Material::setProperty(const std::string &name, const std::any& value)
{
    uniforms[name] = value;
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
    setDefaultValues();
}

void Material::setDefaultValues()
{
    for (const auto& uniform : shader->uniformDefaultValues())
    {
        float values[4];
        std::istringstream tokenStream(uniform.value);
        std::string token;
        int index = 0;
        while (std::getline(tokenStream, token, ','))
        {
            values[index++] = std::stof(token);
        }
        setProperty(uniform.name, glm::vec4(values[0], values[1], values[2], values[3]));
    }

    for (const auto& tex : shader->texDefaultValues())
    {
        if (tex.value == "white") {
            setTexture(tex.name, AssetLibrary::instance().getTexture(
                    AssetLibrary::DefaultResources::texWhite)->ID(), 0);
        }
    }
}
