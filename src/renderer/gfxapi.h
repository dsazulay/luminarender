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

struct TextureInfo
{
    int width;
    int height;
    Format format;
    Filtering filtering = Filtering::BILINEAR;
    Wrap wrap = Wrap::REPEAT;
    TexType type = TexType::UBYTE;
    unsigned char* initialData = nullptr;
};

struct RenderBufferInfo
{
    int width;
    int height;
    Format format;
};

struct FrameBufferInfo
{
    bool hasColorBuffer = true;
};

struct FrameBufferAttachmentInfo
{
    id_t attachment;
    AttachmentType type;
    AttachmentTarget target;
};


