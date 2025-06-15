#include "importer.h"

#include "../logger.h"

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


void processNode(aiNode *node, const aiScene *scene, ModelData* data, bool material);
MeshData processMesh(aiMesh *mesh, const aiScene *scene, bool importMaterial);

void TextureData::freeData()
{
    for (auto &d : data) {
        stbi_image_free(d);
    }
}

TextureData loadTextureFromFile(const std::string &file)
{
    TextureData textureData;
    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load(file.c_str(),
                                    &textureData.width,
                                    &textureData.height,
                                    &textureData.channels, 0);
    if (!data) {
        logger::logError("Failed to load texture {}", file);
    }

    textureData.data.push_back(data);

    return textureData;
}

TextureData loadHDRTextureFromFile(const std::string &file)
{
    TextureData textureData;
    stbi_set_flip_vertically_on_load(true);

    float *data = stbi_loadf(file.c_str(),
                             &textureData.width,
                             &textureData.height,
                             &textureData.channels, 0);
    if (!data) {
        logger::logError("Failed to load HDR texture {}", file);
    }

    textureData.data.push_back(data);


    return textureData;
}

TextureData loadCubeMapFromFiles(std::vector<std::string> &faces)
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
            logger::logError("Failed to load CubeMap texture {}", face);
        }

        textureData.data.push_back(data);
    }

    return textureData;
}

ModelData* loadModel(std::string_view path, bool material)
{
    ModelData *data = new ModelData();

    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path.data(), aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        logger::logError("Failed to load model {}: {}", path, import.GetErrorString());
        delete data;
        return nullptr;
    }

    processNode(scene->mRootNode, scene, data, material);

    return data;
}

void processNode(aiNode *node, const aiScene *scene, ModelData *data, bool material)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        // TODO: Check copy of mesh data
        MeshData meshData = processMesh(mesh, scene, material);
        data->push_back(meshData);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
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
    // outIndices.reserve(indicesSize);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            m.indices.push_back(face.mIndices[j]);
        }
    }

    if (!importMaterial)
        return { mesh->mName.C_Str(), m, {} };

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
            std::string correctName = texName.C_Str();
            // TODO: check OS before change it
            correctName.replace(8, 1, "/");
            materialData.diffusePath = correctName;
        }
    }

    return { mesh->mName.C_Str(), m, materialData };
}

