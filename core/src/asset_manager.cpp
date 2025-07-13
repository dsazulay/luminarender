#include "asset_manager.h"

#include "assets/model_types.h"
#include "logger.h"
#include "assets/importer.h"
#include "assets/primitives.h"

#include "renderer/gfxapi.h"

#include <cstddef>
#include <format>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <optional>
#include <sstream>


Texture AssetManager::getTexture2D(std::string_view path)
{
    auto it = m_textures.find(path.data());
    if (it != m_textures.end()) { return it->second; }

    std::optional<TextureData> optionalTexData = loadTextureFromFile(std::string{path});
    if (!optionalTexData.has_value())
    {
        logger::logError("Failed to load texture {}.", path);
        return getTexture2D("resources/textures/default_white.png");
    }

    TextureData& textureData = optionalTexData.value();

    Format format = Format::RGBA;
    ByteFormat byteFormat = ByteFormat::RGBA;
    if (textureData.channels == 1)
    {
        format = Format::RED;
        byteFormat = ByteFormat::RED;
    }
    else if (textureData.channels == 3)
    {
        format = Format::RGB;
        byteFormat = ByteFormat::RGB;
    }

    Texture newTexture;
    newTexture.handle = m_gpurm->createTexture({
        .width = textureData.width,
        .height = textureData.height,
        .format = format,
        .byteFormat = byteFormat,
        .minFiltering = Filtering::LINEAR_MIPMAP_LINEAR,
        .initialData = textureData.data.front(),
    });
    textureData.freeData();

    //m_files.push_back(createAssetFile(path, FileView{ texture }));

    m_textures.insert(std::make_pair(path, newTexture));

    return newTexture;
}

Texture AssetManager::getTextureHDR(std::string_view path)
{
    auto it = m_textures.find(path.data());
    if (it != m_textures.end()) { return it->second; }

    std::optional<TextureData> optionalTexData = loadHDRTextureFromFile(std::string{path});
    if (!optionalTexData.has_value())
    {
        logger::logError("Failed to load HDR texture {}.", path);
        return getTextureHDR("resources/textures/skybox/green_point_park.hdr");
    }

    TextureData& textureData = optionalTexData.value();

    Texture newTexture;
    newTexture.handle = m_gpurm->createTexture({
        .width = textureData.width,
        .height = textureData.height,
        .format = Format::RGB,
        .byteFormat = ByteFormat::RGB16F,
        .wrap = Wrap::CLAMPEDGE,
        .type = TexType::FLOAT,
        .initialData = textureData.data.front(),
    });

    textureData.freeData();

    //m_files.push_back(createAssetFile(path, FileView{ texture }));

    m_textures.insert(std::make_pair(path, newTexture));

    return newTexture;
}

Texture AssetManager::getTextureCM(std::vector<std::string> path)
{
    auto it = m_textures.find(path.front());
    if (it != m_textures.end()) { return it->second; }

    std::optional<TextureData> optionalTexData = loadCubeMapFromFiles(path);

    if (!optionalTexData.has_value())
    {
        logger::logError("Failed to load CubeMap texture {}.", path.front());
        std::vector<std::string> faces = {
            "resources/textures/skybox/right.jpg",
            "resources/textures/skybox/left.jpg",
            "resources/textures/skybox/top.jpg",
            "resources/textures/skybox/bottom.jpg",
            "resources/textures/skybox/front.jpg",
            "resources/textures/skybox/back.jpg",
        };
        return getTextureCM(faces);
    }

    TextureData& textureData = optionalTexData.value();

    Texture newTexture;
    newTexture.handle = m_gpurm->createTexture({
        .width = textureData.width,
        .height = textureData.height,
        .format = Format::RGB,
        .byteFormat = ByteFormat::RGB,
        .target = TextureTarget::CUBEMAP,
        .wrap = Wrap::CLAMPEDGE,
        .arrayOfInitialData = textureData.data,
    });

    textureData.freeData();

    //m_files.push_back(createAssetFile(path, FileView{ texture }));

    m_textures.insert(std::make_pair(path.front(), newTexture));

    return newTexture;
}

Mesh AssetManager::loadMesh(MeshType type, VertexIndexTuple& mesh)
{
    Mesh newMesh;
    MeshHandles handles = m_gpurm->createMesh(MeshInfo<Vertex>{
        .vertices = mesh.vertices,
        .indices = mesh.indices,
        .amountAndOffset = {
            std::make_pair(3, offsetof(Vertex, position)),
            std::make_pair(3, offsetof(Vertex, normal)),
            std::make_pair(2, offsetof(Vertex, texcoord)),
            std::make_pair(3, offsetof(Vertex, tangent)),
        }
    });
    newMesh.handle = handles.vao;
    newMesh.indexCount = mesh.indices.size();

    return newMesh;
}

Mesh AssetManager::getMesh(MeshType type)
{
    auto it = m_nativeModels.find(type);
    if (it != m_nativeModels.end()) { return it->second; }


    VertexIndexTuple meshData = getNativeMeshByType(type);
    Mesh mesh = loadMesh(type, meshData);

    m_nativeModels.insert({type, mesh});

    return mesh;
}

Mesh AssetManager::getMesh(std::string_view name)
{
    auto it = m_importedModels.find(name.data());
    if (it != m_importedModels.end()) { return it->second; }

    // If imported model is not loaded, return a quad
    return getMesh(MeshType::Quad);
}

Model AssetManager::getModel(std::string_view path, bool loadMaterial)
{
    auto it = m_models.find(path.data());
    if (it != m_models.end()) { return it->second; }

    Model newModel;
    std::optional<ModelData> modelData = loadModel(path, loadMaterial);

    if (!modelData.has_value())
    {
        logger::logError("Failed to load model {}.", path);
        // TODO: Return gracefully
        return newModel;
    }

    for (auto& mesh : modelData.value())
    {
        Mesh m = loadMesh(MeshType::Custom, mesh.vertexIndex);

        if (!loadMaterial)
        {
            m_importedModels.insert({mesh.name, m});
            newModel.meshes.push_back({mesh.name, m});

            continue;
        }

        if (mesh.material.name == "") {
            m.importedMatName = std::format("{}_{}", path, mesh.material.index);
        } else {
            m.importedMatName = mesh.material.name;
        }

        m_importedModels.insert({mesh.name, m});
        newModel.meshes.push_back({mesh.name, m});

        Material& mat = createMaterial(m.importedMatName, &getShader("resources/shaders/cook_torrance.glsl"));
        mat.setColor("u_albedo", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        if (mesh.material.diffusePath != "") {
            Texture albedoTex = getTexture2D("resources/sponza_glTF/" + mesh.material.diffusePath);
            mat.setTexture("u_albedoTex", albedoTex.handle);
        }
        if (mesh.material.normalPath != "") {
            Texture normalTex = getTexture2D("resources/sponza_glTF/" + mesh.material.normalPath);
            mat.setTexture("u_normalMap", normalTex.handle);
        }
    }

    const auto& model = m_models.insert(std::make_pair(path, newModel));
    //m_files.push_back(createAssetFile(path, FileView{ model }));

    return model.first->second;
}

Shader& AssetManager::getShader(std::string_view path)
{
    auto it = m_shaders.find(path.data());
    if (it != m_shaders.end()) { return it->second; }

    std::optional<ShaderData> optionalShaderData = loadShader(path);
    if (!optionalShaderData.has_value())
    {
        logger::logError("Failed to load shader file {}", path);
        // TODO: Return a fallback shader
    }

    ShaderData& shaderData = optionalShaderData.value();

    ShaderInfo shaderInfo;
    for (auto& kv : shaderData.shaders)
    {
        switch (kv.first)
        {
            case ShaderSourceType::VERTEX:
                shaderInfo.sources[ShaderType::VERTEX] = kv.second;
                break;
            case ShaderSourceType::FRAGMENT:
                shaderInfo.sources[ShaderType::FRAGMENT] = kv.second;
                break;
            case ShaderSourceType::GEOMETRY:
                shaderInfo.sources[ShaderType::GEOMETRY] = kv.second;
                break;
            case ShaderSourceType::NONE:
                break;
        }
    }

    Shader shader;
    shader.handle = m_gpurm->createShader(shaderInfo);

    for (auto& property : shaderData.shaderProperties)
    {
        if (property.type == "Float") {
            shader.uniformsDefaultValues.floatValues[property.name] = std::stof(property.value);
        }
        else if (property.type == "2D")
        {
            // TODO: create a variable to cache the default texture handle
            if (property.value == "white") {
                shader.uniformsDefaultValues.texValues[property.name] = getTexture2D("resources/textures/default_white.png").handle;
            }
            else if (property.value == "normal") {
                shader.uniformsDefaultValues.texValues[property.name] = getTexture2D("resources/textures/default_normal.png").handle;
            }
        }
        else
        {
            float values[4];
            std::istringstream tokenStream{ property.value };
            std::string token;
            int index = 0;
            while (std::getline(tokenStream, token, ','))
            {
                values[index++] = std::stof(token);
            }
            shader.uniformsDefaultValues.colorValues[property.name] = glm::vec4(values[0], values[1], values[2], values[3]);
        }
    }


    glUniformBlockBinding(shader.handle, glGetUniformBlockIndex(shader.handle, "Matrices"), 0);
    glUniformBlockBinding(shader.handle, glGetUniformBlockIndex(shader.handle, "Lights"), 1);

    auto& s = m_shaders.insert(std::make_pair(path, shader)).first->second;

    m_files.push_back(createAssetFile(path,
        [&](const auto& arg) { AssetManager::reloadShader(arg); }));

    return s;
}

Material& AssetManager::getMaterial(std::string_view name)
{
    auto it = m_materials.find(name.data());
    if (it != m_materials.end()) { return it->second; }

    logger::logError("Material {} doesn't exist", name);

    // TODO: Return fallback material
    return m_materials.begin()->second;
}

Material& AssetManager::createMaterial(std::string_view name, Shader* shader, MaterialType type)
{
    auto it = m_materials.find(name.data());
    if (it != m_materials.end())
    {
        logger::logWarning("Material named {} already exist!", name);
        return it->second;
    }

    Material m;
    m.name = name;
    m.shader = shader;
    m.uniforms = shader->uniformsDefaultValues;
    m.type = type;

    auto& mat = m_materials.insert(std::make_pair(name, m)).first->second;

    return mat;
}

void AssetManager::reloadShader(std::string_view path)
{
    std::optional<ShaderData> optionalShaderData = loadShader(path);
    if (!optionalShaderData.has_value())
    {
        logger::logError("Failed to load shader file {}", path);
        // TODO: Return a fallback shader
    }

    ShaderData& shaderData = optionalShaderData.value();

    ShaderInfo shaderInfo;
    for (auto& kv : shaderData.shaders)
    {
        switch (kv.first)
        {
            case ShaderSourceType::VERTEX:
                shaderInfo.sources[ShaderType::VERTEX] = kv.second;
                break;
            case ShaderSourceType::FRAGMENT:
                shaderInfo.sources[ShaderType::FRAGMENT] = kv.second;
                break;
            case ShaderSourceType::GEOMETRY:
                shaderInfo.sources[ShaderType::GEOMETRY] = kv.second;
                break;
            case ShaderSourceType::NONE:
                break;
        }
    }

    Shader shader;
    shader.handle = m_gpurm->createShader(shaderInfo);

    for (auto& property : shaderData.shaderProperties)
    {
        if (property.type == "Float") {
            shader.uniformsDefaultValues.floatValues[property.name] = std::stof(property.value);
        }
        else if (property.type == "2D")
        {
            // TODO: create a variable to cache the default texture handle
            if (property.value == "white") {
                shader.uniformsDefaultValues.texValues[property.name] = getTexture2D("resources/textures/default_white.png").handle;
            }
        }
        else
        {
            float values[4];
            std::istringstream tokenStream{ property.value };
            std::string token;
            int index = 0;
            while (std::getline(tokenStream, token, ','))
            {
                values[index++] = std::stof(token);
            }
            shader.uniformsDefaultValues.colorValues[property.name] = glm::vec4(values[0], values[1], values[2], values[3]);
        }
    }

    glUniformBlockBinding(shader.handle, glGetUniformBlockIndex(shader.handle, "Matrices"), 0);
    glUniformBlockBinding(shader.handle, glGetUniformBlockIndex(shader.handle, "Lights"), 1);

    m_shaders[std::string{path}] = shader;
    // TODO: Update material uniforms
    // TODO: Delete old shader
}

void AssetManager::loadDefaultResources()
{
    /*
    getMesh(MeshType::Quad);
    getMesh(MeshType::Cube);
    getMesh(MeshType::Sphere);
    getMesh(MeshType::CubeMap);
    getMesh(MeshType::TriangleMap);
    */
}

void AssetManager::checkFileModification()
{
    for (AssetFile &file : m_files) {
        try {
            auto lastWriteTime = std::filesystem::last_write_time(file.path);
            if (file.lastWriteTime != lastWriteTime) {
                logger::logInfo("Reloading file: {}", file.path);
                file.onFileChange(file.path);
                file.lastWriteTime = lastWriteTime;
            }
        } catch (std::filesystem::filesystem_error &e) {
            logger::logError("{}", e.what());
        }
    }
}

AssetFile createAssetFile(std::string_view path, std::function<void(std::string_view)> callback)
{
    return AssetFile{
        .path = std::string{ path },
        .lastWriteTime = std::filesystem::last_write_time(path),
        .onFileChange = callback,
    };
}
