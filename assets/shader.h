#pragma once

#include "../pch.h"
#include <glm/glm.hpp>

class Shader
{
public:
    unsigned int ID;
    unsigned int matricesUniformBlockIndex;
    unsigned int lightsUniformBlockIndex;

    Shader() = default;
    Shader(const char* vertexPath, const char* fragPath);
    ~Shader();

    void addGeometryShader(const char* vertexPath);

    void use() const;
    std::string preprocess(std::stringstream& input, int level);


    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &vec) const;
    void setVec4(const std::string &name, const glm::vec4 &vec) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    static std::stringstream getStreamFromFile(const std::string& path);

};
