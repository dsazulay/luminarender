#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <string>
#include <unordered_map>
#include <any>
#include <typeindex>

class Shader;

class Material
{
public:
    Shader* shader;
    Material(Shader* s);
    ~Material();


    void setProperty(const std::string& name, const std::any& value);
    void setTexture(const std::string& name, unsigned int texID, int texUnit);
    void setUniformData();
    std::unordered_map<std::string, std::any>& getUniformData();
    std::unordered_map<std::string, unsigned int> textures;

    void name(std::string n);
    std::string name() const;
    
    static void setDefaultTexWhite(unsigned int id);

private:
    std::unordered_map<std::string, std::any> uniforms;

    void setDefaultValues();

    void castAndSetUniform(const std::string &name, std::any value);
    void setUniform(const std::string &name, bool value) const;
    void setUniform(const std::string &name, int value) const;
    void setUniform(const std::string &name, unsigned int value) const;
    void setUniform(const std::string &name, float value) const;
    void setUniform(const std::string &name, const glm::vec3 &vec) const;
    void setUniform(const std::string &name, const glm::vec4 &vec) const;
    void setUniform(const std::string &name, const glm::mat3 &mat) const;
    void setUniform(const std::string &name, const glm::mat4 &mat) const;

    std::string m_name;

    static unsigned int defaultTexWhite;

};

static std::unordered_map<std::type_index, int> typeIndexIntMap
{
    {std::type_index(typeid(bool)), 0},
    {std::type_index(typeid(int)), 1},
    {std::type_index(typeid(unsigned int)), 2},
    {std::type_index(typeid(float)), 3},
    {std::type_index(typeid(glm::vec3)), 4},
    {std::type_index(typeid(glm::vec4)), 5},
    {std::type_index(typeid(glm::mat3)), 6},
    {std::type_index(typeid(glm::mat4)), 7}
};
