#pragma once

#include <unordered_map>
#include <vector>

typedef unsigned int id_t;

struct MeshHandles
{
    id_t vao;
    id_t vbo;
    id_t ebo;
};

enum class ShaderType
{
    VERTEX,
    FRAGMENT,
    GEOMETRY,
};

enum class Filtering
{
    POINT,
    BILINEAR,
    LINEAR_MIPMAP_LINEAR,
};

enum class Format
{
    RED,
    RGB,
    RGBA,
    DEPTH,
};

enum class ByteFormat
{
    RED,
    RGB,
    RGBA,
    RGB16F,
    RGBA16F,
    RGBA32F,
    DEPTH,
    DEPTH24_STENCIL8,
};

enum class TexType
{
    UBYTE,
    FLOAT,
};

enum class TextureTarget
{
    TEX2D,
    CUBEMAP,
};

enum class Wrap
{
    REPEAT,
    MIRROR,
    CLAMPBORDER,
    CLAMPEDGE,
};

enum class AttachmentType
{
    COLOR0,
    COLOR1,
    COLOR2,
    COLOR3,
    COLOR4,
    DEPTH,
    STENCIL,
    DEPTHSTENCIL,
};

enum class AttachmentTarget
{
    TEX2D,
    RENDERBUFFER,
};

enum class ClearMask
{
    COLOR,
    DEPTH,
    STENCIL,
    COLORDEPTH,
};

enum class FrameBufferOp
{
    READ,
    WRITE,
    READWRITE,
};

template <typename T>
struct MeshInfo
{
    const char* debugName = "Mesh";
    std::vector<T> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::pair<int, unsigned long>> amountAndOffset;
};

struct ShaderInfo
{
    const char* shaderName = "Shader";
    std::unordered_map<ShaderType, std::string> sources;
};

struct TextureInfo
{
    const char* debugName = "Texture";
    int width;
    int height;
    Format format;
    ByteFormat byteFormat;
    TextureTarget target = TextureTarget::TEX2D;
    Filtering filtering = Filtering::BILINEAR;
    Filtering minFiltering = Filtering::BILINEAR;
    Wrap wrap = Wrap::REPEAT;
    TexType type = TexType::UBYTE;
    void* initialData = nullptr;
    std::vector<void *> arrayOfInitialData = {};
};

struct RenderBufferInfo
{
    const char* debugName = "RenderBuffer";
    int width;
    int height;
    ByteFormat byteFormat;
};

struct FrameBufferInfo
{
    const char* debugName = "FrameBuffer";
    bool hasColorBuffer = true;
};

struct FrameBufferAttachmentInfo
{
    const char* debugName = "FrameBufferAttachment";
    id_t attachment;
    AttachmentType type;
    AttachmentTarget target;
};

struct FrameBufferTargetInfo
{
    const char* debugName = "FrameBufferTarget";
    int size;
    AttachmentType* types;
};

