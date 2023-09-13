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
    DEPTHSTENCIL
};

enum class Wrap
{
    REPEAT,
    MIRROR,
    CLAMPBORDER,
    CLAMPEDGE
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
    RENDERBUFFER
};

struct TextureInfo
{
    int width;
    int height;
    Format format;
    Filtering filtering = Filtering::BILINEAR;
    Wrap wrap = Wrap::REPEAT;
    unsigned char* initialData = nullptr;
};

struct RenderBufferInfo
{
    int width;
    int height;
    Format format;
};

struct FrameBufferAttachmentInfo
{
    id_t attachment;
    AttachmentType type;
    AttachmentTarget target;
};


