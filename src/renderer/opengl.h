#pragma once

#include "gfxapi.h"

#include <glad/glad.h>

class OpenGL
{
public:
    id_t createTexture(TextureInfo info);
    id_t createRenderBuffer(RenderBufferInfo info);
    id_t createFrameBuffer();
    void deleteTexture(id_t texture);
    void deleteRenderBuffer(id_t renderBuffer);
    void deleteFrameBuffer(id_t frameBuffer);

    void attachTexture(id_t frameBuffer, FrameBufferAttachmentInfo info);
    void attachRenderBuffer(id_t frameBuffer, FrameBufferAttachmentInfo info);

    void bindFrameBuffer(id_t frameBuffer);
    void unbindFrameBuffer();

private:
    GLenum getFormat(Format format);
    GLint getFiltering(Filtering filtering);
    GLint getWrap(Wrap wrap);
    GLenum getAttachmentType(AttachmentType type);
    GLenum getAttachmentTarget(AttachmentTarget target);
};

