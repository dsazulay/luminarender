#include "importer.h"

#include "../logger.h"
#include "assimp/material.h"
#include "assimp/types.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stb_image.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string_view>

constexpr int MAX_INCLUDE_DEPTH = 8;

void processNode(aiNode* node, const aiScene* scene, ModelData* data, bool material);
MeshData processMesh(aiMesh* mesh, const aiScene* scene, bool importMaterial);

std::optional<std::stringstream> getStreamFromFile(std::filesystem::path path);
ShaderSourceType getShaderTypeFromString(std::string_view type);
void shaderPreprocess(std::stringstream& input, ShaderData& shaderData);
std::string resolveIncludes(std::stringstream& input, int level);
void extractShaderProperties(std::stringstream& input, ShaderData& shaderData);


void TextureData::freeData()
{
    for (auto& d : data) {
        stbi_image_free(d);
    }
}

std::optional<TextureData> loadTextureFromFile(const std::string& file)
{
    TextureData textureData;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(file.c_str(),
                                    &textureData.width,
                                    &textureData.height,
                                    &textureData.channels, 0);
    if (!data) {
        return std::nullopt;
    }

    textureData.data.push_back(data);

    return textureData;
}

std::optional<TextureData> loadHDRTextureFromFile(const std::string& file)
{
    TextureData textureData;
    stbi_set_flip_vertically_on_load(true);

    float* data = stbi_loadf(file.c_str(),
                             &textureData.width,
                             &textureData.height,
                             &textureData.channels, 0);
    if (!data) {
        return std::nullopt;
    }

    textureData.data.push_back(data);


    return textureData;
}

std::optional<TextureData> loadCubeMapFromFiles(const std::vector<std::string>& faces)
{
    TextureData textureData;
    stbi_set_flip_vertically_on_load(false);

    for (const auto& face : faces)
    {
        unsigned char* data = stbi_load(face.c_str(),
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

std::optional<std::stringstream> getStreamFromFile(std::filesystem::path path)
{
    std::ifstream shaderFile(path, std::ios::in);
    std::stringstream shaderStream;

    if (!shaderFile.is_open()) {
        return std::nullopt;
    }

    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    return shaderStream;
}

ShaderSourceType getShaderTypeFromString(std::string_view type)
{
    if (type == "vertex") {
        return ShaderSourceType::VERTEX;
    }
    else if (type == "fragment") {
        return ShaderSourceType::FRAGMENT;
    }
    else if (type == "geometry") {
        return ShaderSourceType::GEOMETRY;
    }

    logger::logError("Invalid Shader Type {}", type);
    return ShaderSourceType::NONE;
}

void shaderPreprocess(std::stringstream& input, ShaderData& shaderData)
{
    constexpr std::string_view typeToken = "#shader";
    constexpr std::string_view propertiesToken = "#properties";
    constexpr std::string_view endPropertiesToken = "#endproperties";

    std::stringstream shaderStream;
    std::stringstream propertiesStream;
    ShaderSourceType shaderType = ShaderSourceType::NONE;

    bool readingProperties = false;

    auto flushStream = [&]() {
        if (shaderType != ShaderSourceType::NONE)
        {
            shaderData.shaders[shaderType] = resolveIncludes(shaderStream, 0);
            shaderStream.str(std::string{});
            shaderStream.clear();
        }
    };

    std::string line;
    while (std::getline(input, line))
    {
        if (line.starts_with(typeToken))
        {
            flushStream();
            shaderType = getShaderTypeFromString(line.substr(typeToken.size() + 1));
        }
        else if (line.find(propertiesToken) != std::string::npos) {
            readingProperties = true;
        }
        else if ((line.find(endPropertiesToken) != std::string::npos)) {
            readingProperties = false;
            extractShaderProperties(propertiesStream, shaderData);
        }
        else if (readingProperties) {
            propertiesStream << line << "\n";
        }
        else {
            shaderStream << line << "\n";
        }
    }
    flushStream();
}

std::string resolveIncludes(std::stringstream& input, int level)
{
    if (level > MAX_INCLUDE_DEPTH) {
        logger::logWarning("Header inclusion depth limit reached, might be caused by cyclic header inclusion");
        return "";
    }

    std::regex includeRegex("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
    std::smatch match;

    std::stringstream output;
    std::string line;
    while(std::getline(input, line))
    {
        if (std::regex_search(line, match, includeRegex))
        {
            std::filesystem::path include_file = "resources/shaders";
            include_file /= match[1].str();
            std::optional<std::stringstream> include_string = getStreamFromFile(include_file);

            if (include_string.has_value()) {
                output << resolveIncludes(include_string.value(), level + 1) << "\n";
            }
            else {
                logger::logError("Include file {} not successfully read: ignoring it!", include_file.string());
            }
        }
        else {
            output <<  line << "\n";
        }
    }
    return output.str();
}

void extractShaderProperties(std::stringstream& input, ShaderData& shaderData)
{
    std::regex propertyRegex("^[ ]*(.*?)[ ]+(.*?)[ ]*=[ ]*(.*)");
    std::smatch match;
    std::string line;

    while(std::getline(input, line))
    {
        if (std::regex_search(line, match, propertyRegex))
        {
            const auto& type = match[1];
            const auto& name = match[2];
            const auto& value = match[3];
            if (type == "Color" || type == "Float" || type == "2D") {
                shaderData.shaderProperties.emplace_back(type, name, value);
            }
            // TODO: handle error
            else {
                logger::logError("Invalid property type {} at line {}", type.str(), line);
            }
        }
        else {
            logger::logError("Failed to parse shader property line: {}", line);
        }
    }
}

std::optional<ModelData> loadModel(std::string_view path, bool material)
{
    ModelData data;

    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        logger::logError("Failed to load model {}: {}", path, import.GetErrorString());
        return std::nullopt;
    }

    data.reserve(scene->mNumMeshes);
    processNode(scene->mRootNode, scene, &data, material);

    return data;
}

void processNode(aiNode* node, const aiScene* scene, ModelData* data, bool material)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        data->push_back(processMesh(mesh, scene, material));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, data, material);
    }
}

MeshData processMesh(aiMesh* mesh, const aiScene* scene, bool importMaterial)
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

        if (mesh->mNormals)
        {
            vec.x = mesh->mNormals[i].x;
            vec.y = mesh->mNormals[i].y;
            vec.z = mesh->mNormals[i].z;
        }
        else {
            vec = glm::vec3{ 0.0 };
        }
        vert.normal = vec;

        if (mesh->mTangents)
        {
            vec.x = mesh->mTangents[i].x;
            vec.y = mesh->mTangents[i].y;
            vec.z = mesh->mTangents[i].z;
        }
        else {
            vec = glm::vec3{ 0.0 };
        }
        vert.tangent = vec;

        glm::vec2 uv{ 0.0 };
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
        materialData.index = mesh->mMaterialIndex;

        float opacity = 0;
        if (material->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
            materialData.type = MaterialType::TRANSPARENT;
        }
        else {
            materialData.type = MaterialType::OPAQUE;
        }

        for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); ++i)
        {
            aiString texName;
            material->GetTexture(aiTextureType_DIFFUSE, i, &texName);
            materialData.diffusePath = texName.C_Str();

            #if defined (__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
            std::replace(materialData.diffusePath.begin(), materialData.diffusePath.end(), '\\', '/');
            #endif
        }

        for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_NORMALS); ++i)
        {
            aiString texName;
            material->GetTexture(aiTextureType_NORMALS, i, &texName);
            materialData.normalPath = texName.C_Str();

            #if defined (__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
            std::replace(materialData.normalPath.begin(), materialData.normalPath.end(), '\\', '/');
            #endif
        }
    }

    return { mesh->mName.C_Str(), m, materialData };
}

