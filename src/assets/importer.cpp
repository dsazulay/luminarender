#include "importer.h"

#include "model.h"
#include "texture.h"
#include "../log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

Texture* Importer::loadTextureFromFile(const std::string& file, const std::string &directory) {
    std::string filename = directory + '/' + file;

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    Texture* texture = nullptr;
    if (data)
        texture = new Texture(TextureType::Tex2D, width, height, nrChannels, data);
    else
        LOG_ERROR("Failed to load texture");

    stbi_image_free(data);

    return texture;
}

Texture* Importer::loadHDRTextureFromFile(const std::string& file, const std::string &directory)
{
    std::string filename = directory + '/' + file;

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    float* data = stbi_loadf(filename.c_str(), &width, &height, &nrChannels, 0);
    Texture* texture = nullptr;
    if (data)
        texture = new Texture(TextureType::HDR, width, height, data);
    else
        LOG_ERROR("Failed to load texture");

    stbi_image_free(data);

    return texture;
}

Texture* Importer::loadCubeMapFromFiles(std::vector<std::string> faces, const std::string &directory)
{
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    std::vector<unsigned char*> textureVector;

    for (auto & face : faces)
    {
        unsigned char *data = stbi_load((directory + '/' + face).c_str(), &width, &height, &nrChannels, 0);
        if (data)
            textureVector.push_back(data);
        else
            LOG_ERROR("Cubemap tex failed to load at path: {}", face);
    }

    Texture* texture = new Texture(TextureType::CubeMap, width, height, textureVector);

    for (auto texData : textureVector)
        stbi_image_free(texData);

    return texture;
}

Model* Importer::loadModel(const char* path)
{
    Model* model = new Model();

    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_ERROR("ERROR::ASSIMP::{}", import.GetErrorString());
        return model;
    }

    processNode(scene->mRootNode, scene, model);

    return model;
}

void Importer::processNode(aiNode *node, const aiScene *scene, Model* model)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh* newMesh = new Mesh(processMesh(mesh, scene));
        model->m_meshes.push_back(std::make_pair(std::string(mesh->mName.C_Str()), newMesh));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, model);
    }
}

VertexIndexTuple Importer::processMesh(aiMesh *mesh, const aiScene *scene)
{
    VertexIndexTuple m;

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
        vert.texCoords = uv;

        m.vertices.push_back(vert);
    }

//    outIndices.reserve(indicesSize);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            m.indices.push_back(face.mIndices[j]);
        }
    }

    return m;
}
