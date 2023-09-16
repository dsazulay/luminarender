#pragma once

typedef unsigned int id_t;

enum class Filtering
{
    POINT,
    BILINEAR,
};

enum class Format
{
    RGB,
    RGBA,
    DEPTH,
};

enum class ByteFormat
{
    RGB,
    RGBA,
    RGBA16F,
    DEPTH,
    DEPTH24_STENCIL8,
};

enum class TexType
{
    UBYTE,
    FLOAT,
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

struct TextureInfo
{
    const char* debugName = "Texture";
    int width;
    int height;
    Format format;
    ByteFormat byteFormat;
    Filtering filtering = Filtering::BILINEAR;
    Wrap wrap = Wrap::REPEAT;
    TexType type = TexType::UBYTE;
    unsigned char* initialData = nullptr;
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

