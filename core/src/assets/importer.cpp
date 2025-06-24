#include "importer.h"

#include "../logger.h"

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cassert>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <regex>

void processNode(aiNode *node, const aiScene *scene, ModelData *data, bool material);
MeshData processMesh(aiMesh *mesh, const aiScene *scene, bool importMaterial);

std::optional<std::stringstream> getStreamFromFile(std::string_view path);
ShaderSourceType getShaderTypeFromString(std::string_view type);
void shaderPreprocess(std::stringstream &input, ShaderData &shaderData);
std::string getIncludeFilesAsString(std::stringstream &input, int level);
void extractDefaultPropertiesValues(std::stringstream &input, ShaderData &shaderData);


void TextureData::freeData()
{
    for (auto &d : data) {
        stbi_image_free(d);
    }
}

std::optional<TextureData> loadTextureFromFile(const std::string &file)
{
    TextureData textureData;
    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load(file.c_str(),
                                    &textureData.width,
                                    &textureData.height,
                                    &textureData.channels, 0);
    if (!data) {
        return std::nullopt;
    }

    textureData.data.push_back(data);

    return textureData;
}

std::optional<TextureData> loadHDRTextureFromFile(const std::string &file)
{
    TextureData textureData;
    stbi_set_flip_vertically_on_load(true);

    float *data = stbi_loadf(file.c_str(),
                             &textureData.width,
                             &textureData.height,
                             &textureData.channels, 0);
    if (!data) {
        return std::nullopt;
    }

    textureData.data.push_back(data);


    return textureData;
}

std::optional<TextureData> loadCubeMapFromFiles(std::vector<std::string> &faces)
{
    TextureData textureData;
    stbi_set_flip_vertically_on_load(false);

    for (auto &face : faces)
    {
        unsigned char *data = stbi_load(face.c_str(),
                                        &textureData.width,
                                        &textureData.height,
                                        &textureData.channels, 0);
        if (!data) {
            return std::nullopt;
        }

        textureData.data.push_back(data);
    }

    return textureData;
}

std::optional<ShaderData> loadShader(std::string_view path)
{
    ShaderData shaderData;
    std::optional<std::stringstream> shaderStream = getStreamFromFile(path);

    if (!shaderStream.has_value()) {
        return std::nullopt;
    }

    shaderPreprocess(shaderStream.value(), shaderData);

    return shaderData;
}

std::optional<std::stringstream> getStreamFromFile(std::string_view path)
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
        logger::logError("Shader file {} not successfully read: {}", path, e.what());
        return std::nullopt;
    }

    return shaderStream;
}

ShaderSourceType getShaderTypeFromString(std::string_view type)
{
    if (type == "vertex")
        return ShaderSourceType::VERTEX;
    else if (type == "fragment")
        return ShaderSourceType::FRAGMENT;
    else if (type == "geometry")
        return ShaderSourceType::GEOMETRY;

    logger::logError("Invalid Shader Type {}", type);
    return ShaderSourceType::NONE;
}

void shaderPreprocess(std::stringstream& input, ShaderData& shaderData)
{
    std::string line;
    std::stringstream outTest;
    ShaderSourceType shaderType = ShaderSourceType::NONE;

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
            if (shaderType != ShaderSourceType::NONE)
            {
                shaderData.shaders[shaderType] = getIncludeFilesAsString(outTest, 0);
                outTest.str(std::string());
                outTest.clear();
            }

            shaderType = getShaderTypeFromString(line.substr(typeTokenLength + 1));
        }
        else if (line.find(propertiesToken) != std::string::npos)
        {
            shouldReadProperties = true;
        }
        else if ((line.find(endPropertiesToken) != std::string::npos))
        {
            shouldReadProperties = false;
            extractDefaultPropertiesValues(propertiesStream, shaderData);
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
    shaderData.shaders[shaderType] = getIncludeFilesAsString(outTest, 0);
}

std::string getIncludeFilesAsString(std::stringstream& input, int level)
{
    std::regex re("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
    std::smatch matches;

    assert(level < 8 && "Header inclusion depth limit reached, might be caused by cyclic header inclusion");

    std::stringstream output;
    size_t line_number = 1;

    std::string line;
    while(std::getline(input, line))
    {
        if (std::regex_search(line, matches, re))
        {
            std::string include_file = matches[1];
            std::optional<std::stringstream> include_string = getStreamFromFile("resources/shaders/" + include_file);

            if (!include_string.has_value())
            {
                logger::logError("Include file {} not successfully read: ignoring it!", include_file);
                continue;
            }

            output << getIncludeFilesAsString(include_string.value(), level + 1) << std::endl;
        }
        else {
            output <<  line << std::endl;
        }
        ++line_number;
    }
    return output.str();
}

void extractDefaultPropertiesValues(std::stringstream &input, ShaderData &shaderData)
{
    std::regex re("^[ ]*(.*?)[ ]+(.*?)[ ]*=[ ]*(.*)");
    std::smatch matches;
    std::string line;

    while(std::getline(input, line))
    {
        if (std::regex_search(line, matches, re))
        {
            if (matches[1] == "Color" || matches[1] == "Float") {
                shaderData.uniformDefaultValues.push_back({ matches[1], matches[2], matches[3] });
            }
            else if (matches[1] == "2D") {
                shaderData.texDefaultValues.push_back({ matches[1], matches[2], matches[3] });
            }
        }
        else
        {
            // TODO: Show error msg and fallback to a default shader
            logger::logError("Could not parse properties!");
        }
    }
}

std::optional<ModelData> loadModel(std::string_view path, bool material)
{
    ModelData data;

    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path.data(), aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        logger::logError("Failed to load model {}: {}", path, import.GetErrorString());
        return std::nullopt;
    }

    data.reserve(scene->mNumMeshes);
    processNode(scene->mRootNode, scene, &data, material);

    return data;
}

void processNode(aiNode *node, const aiScene *scene, ModelData *data, bool material)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        data->push_back(processMesh(mesh, scene, material));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, data, material);
    }
}

MeshData processMesh(aiMesh *mesh, const aiScene *scene, bool importMaterial)
{
    VertexIndexTuple m;

    // process vertices
    m.vertices.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vert{};
        glm::vec3 vec;

        vec.x = mesh->mVertices[i].x;
        vec.y = mesh->mVertices[i].y;
        vec.z = mesh->mVertices[i].z;
        vert.position = vec;

        vec.x = mesh->mNormals[i].x;
        vec.y = mesh->mNormals[i].y;
        vec.z = mesh->mNormals[i].z;
        vert.normal = vec;

        glm::vec2 uv;
        uv.x = mesh->mTextureCoords[0][i].x;
        uv.y = mesh->mTextureCoords[0][i].y;
        vert.texcoord = uv;

        m.vertices.push_back(vert);
    }

    // process indices
    if (mesh->mNumFaces > 0) {
        m.indices.reserve(mesh->mNumFaces * mesh->mFaces[0].mNumIndices);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            m.indices.push_back(face.mIndices[j]);
        }
    }

    if (!importMaterial) {
        return { mesh->mName.C_Str(), m, {} };
    }

    // process material
    MaterialData materialData;
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString matName = material->GetName();

        materialData.name = matName.C_Str();

        for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); ++i)
        {
            aiString texName;
            material->GetTexture(aiTextureType_DIFFUSE, i, &texName);
            materialData.diffusePath = texName.C_Str();

            #if defined (__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
            std::replace(materialData.diffusePath.begin(), materialData.diffusePath.end(), '\\', '/');
            #endif
        }
    }

    return { mesh->mName.C_Str(), m, materialData };
}

