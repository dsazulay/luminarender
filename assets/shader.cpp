#include "shader.h"
#include "glad/glad.h"

Shader::Shader(const char* vertexPath, const char* fragPath)
{
    std::string vertexCode;
    std::string fragCode;
    std::stringstream vertexStream;
    std::stringstream fragStream;

    vertexStream = getStreamFromFile(vertexPath);
    fragStream = getStreamFromFile(fragPath);

    vertexCode = preprocess(vertexStream, 0);
    fragCode = preprocess(fragStream, 0);

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragCode.c_str();

    unsigned int vertex, frag;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fShaderCode, nullptr);
    glCompileShader(frag);

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, frag);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(frag);

    // TODO: separate on another method
    matricesUniformBlockIndex = glGetUniformBlockIndex(ID, "Matrices");
    glUniformBlockBinding(ID, matricesUniformBlockIndex, 0);

    lightsUniformBlockIndex = glGetUniformBlockIndex(ID, "Lights");
    glUniformBlockBinding(ID, lightsUniformBlockIndex, 1);
}

std::string Shader::preprocess(std::stringstream& input, int level)
{
    std::regex re("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
    std::smatch matches;

    if(level > 32)
    {
        std::cout << "header inclusion depth limit reached, might be caused by cyclic header inclusion" << std::endl;
    }

    std::stringstream output;
    size_t line_number = 1;

    std::string line;
    while(std::getline(input,line))
    {
        if (std::regex_search(line, matches, re))
        {
            std::string include_file = matches[1];
            std::stringstream include_string;

            include_string = Shader::getStreamFromFile("Resources/Shaders/" + include_file);

            output << preprocess(include_string, level + 1) << std::endl;
        }
        else
        {
            output <<  line << std::endl;
        }
        ++line_number;
    }
    return output.str();
}

Shader::~Shader()
{

}

std::stringstream Shader::getStreamFromFile(const std::string& path)
{
    std::ifstream shaderFile;
    std::stringstream shaderStream;

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        shaderFile.open(path);

        shaderStream << shaderFile.rdbuf();

        shaderFile.close();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    return shaderStream;
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &vec) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &vec[0]);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &vec) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &vec[0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::addGeometryShader(const char* vertexPath)
{
    std::string vertexCode;
    std::stringstream vertexStream;

    vertexStream = getStreamFromFile(vertexPath);

    vertexCode = preprocess(vertexStream, 0);

    const char* vShaderCode = vertexCode.c_str();

    unsigned int vertex;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glAttachShader(ID, vertex);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
}
