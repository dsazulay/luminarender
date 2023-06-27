#include "shader.h"

Shader::Shader(const char* shaderPath)
{
    std::stringstream shaderStream = getStreamFromFile(shaderPath);
    auto shaderSources = preprocess(shaderStream);

    compile(shaderSources);
}

std::unordered_map<GLenum, std::string> Shader::preprocess(std::stringstream& input)
{
    std::string line;
    std::stringstream outTest;
    GLenum type = -1;
    std::unordered_map<GLenum, std::string> shaders;

    const char* typeToken = "#shader";
    std::size_t typeTokenLength = strnlen(typeToken, 16);

    const char* propertiesToken = "#properties";
    const char* endPropertiesToken = "#endproperties";
    bool shouldReadProperties = false;
    std::stringstream propertiesStream;

    while (std::getline(input, line))
    {
        if (line.find(typeToken) != std::string::npos)
        {
            if (type != -1)
            {
                shaders[type] = preprocessIncludes(outTest, 0);
                outTest.str(std::string());
                outTest.clear();
            }

            type = getShaderTypeFromString(line.substr(typeTokenLength + 1));
        }
        else if (line.find(propertiesToken) != std::string::npos)
        {
            shouldReadProperties = true;
        }
        else if ((line.find(endPropertiesToken) != std::string::npos))
        {
            shouldReadProperties = false;
            preprocessProperties(propertiesStream);
        }
        else if (shouldReadProperties)
        {
            propertiesStream << line << std::endl;
        }
        else
        {
            outTest << line << std::endl;
        }
    }
    shaders[type] = preprocessIncludes(outTest, 0);
    return shaders;
}

std::string Shader::preprocessIncludes(std::stringstream& input, int level)
{
    std::regex re("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
    std::smatch matches;

    ASSERT(level < 8, "Header inclusion depth limit reached, might be caused by cyclic header inclusion");

    std::stringstream output;
    size_t line_number = 1;

    std::string line;
    while(std::getline(input,line))
    {
        if (std::regex_search(line, matches, re))
        {
            std::string include_file = matches[1];
            std::stringstream include_string;

            include_string = Shader::getStreamFromFile("resources/shaders/" + include_file);

            output << preprocessIncludes(include_string, level + 1) << std::endl;
        }
        else
        {
            output <<  line << std::endl;
        }
        ++line_number;
    }
    return output.str();
}

void Shader::preprocessProperties(std::stringstream& input)
{
    std::regex re("^[ ]*(.*?)[ ]+(.*?)[ ]*=[ ]*(.*)");
    std::smatch matches;

    std::string line;
    while(std::getline(input,line))
    {
        if (std::regex_search(line, matches, re))
        {
            if (matches[1] == "Color" || matches[1] == "Float")
            {
                m_uniformDefaultValues.push_back({ matches[1], matches[2], matches[3] });
            }
            else if (matches[1] == "2D")
            {
                m_texDefaultValues.push_back({ matches[1], matches[2], matches[3] });
            }
        }
        else
        {
            LOG_ERROR("Could not parse properties!");
        }
    }
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
        LOG_ERROR("Shader file not successfully read!");
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

    vertexCode = preprocessIncludes(vertexStream, 0);

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
        LOG_ERROR("Shader compilation failed: {}", infoLog);
    }

    glAttachShader(ID, vertex);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        LOG_ERROR("Shader program linking failed: {}", infoLog);
    }

    glDeleteShader(vertex);
}

GLenum Shader::getShaderTypeFromString(const std::string& type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    else if (type == "fragment")
        return GL_FRAGMENT_SHADER;
    else if (type == "geometry")
        return GL_GEOMETRY_SHADER;

    LOG_ERROR("Invalid shader type: {}", type);
    return -1;
}

void Shader::compile(std::unordered_map<GLenum, std::string>& shaderSources)
{
    ID = glCreateProgram();
    for (auto& kv : shaderSources)
    {
        const char* shaderCode = kv.second.c_str();

        unsigned int shaderID;
        int success;
        char infoLog[512];

        shaderID = glCreateShader(kv.first);
        glShaderSource(shaderID, 1, &shaderCode, nullptr);
        glCompileShader(shaderID);

        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
            LOG_ERROR("Shader compilation failed: {}", infoLog);
        }

        glAttachShader(ID, shaderID);
        glDeleteShader(shaderID);
    }

    glLinkProgram(ID);

    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        LOG_ERROR("Shader program linking failed: {}", infoLog);
    }

    // TODO: separate on another method
    matricesUniformBlockIndex = glGetUniformBlockIndex(ID, "Matrices");
    glUniformBlockBinding(ID, matricesUniformBlockIndex, 0);

    lightsUniformBlockIndex = glGetUniformBlockIndex(ID, "Lights");
    glUniformBlockBinding(ID, lightsUniformBlockIndex, 1);
}

std::vector<PropertyTuple> Shader::uniformDefaultValues()
{
    return m_uniformDefaultValues;
}

std::vector<PropertyTuple> Shader::texDefaultValues()
{
    return m_texDefaultValues;
}
