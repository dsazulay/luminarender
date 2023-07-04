#pragma once

#include "../pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

struct PropertyTuple
{
    std::string type;
    std::string name;
    std::string value;
};

class Shader
{
public:
    unsigned int ID{};
    unsigned int matricesUniformBlockIndex{};
    unsigned int lightsUniformBlockIndex{};

    explicit Shader(const char* shaderPath);

    void addGeometryShader(const char* vertexPath);

    std::vector<PropertyTuple> uniformDefaultValues();
    std::vector<PropertyTuple> texDefaultValues();


    void use() const;
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &vec) const;
    void setVec4(const std::string &name, const glm::vec4 &vec) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    std::unordered_map<GLenum, std::string> preprocess(std::stringstream& input);
    std::string preprocessIncludes(std::stringstream& input, int level);
    void preprocessProperties(std::stringstream& input);
    void compile(std::unordered_map<GLenum, std::string>& shaderSources);

    static std::stringstream getStreamFromFile(const std::string& path);
    static GLenum getShaderTypeFromString(const std::string& type);

    std::vector<PropertyTuple> m_uniformDefaultValues;
    std::vector<PropertyTuple> m_texDefaultValues;
};
