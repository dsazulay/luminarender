#pragma once

#include "gfxapi.h"

#include <glad/gl.h>

class OpenGL
{
public:
    id_t createTexture(TextureInfo info);
    id_t createRenderBuffer(RenderBufferInfo info);
    id_t createFrameBuffer(FrameBufferInfo info);
    void deleteTexture(id_t texture);
    void deleteRenderBuffer(id_t renderBuffer);
    void deleteFrameBuffer(id_t frameBuffer);

    void attachTexture(id_t frameBuffer, FrameBufferAttachmentInfo info);
    void attachRenderBuffer(id_t frameBuffer, FrameBufferAttachmentInfo info);
    void setTargetBuffers(id_t frameBuffer, FrameBufferTargetInfo info);

    void bindFrameBuffer(id_t frameBuffer);
    void bindFrameBuffer(FrameBufferOp op, id_t frameBuffer);
    void unbindFrameBuffer();
    bool isFrameBufferComplete(id_t frameBuffer);

    // Commands
    void setViewportSize(int x, int y, int width, int height);
    void setClearColor(float r, float g, float b, float a);
    void clear(ClearMask mask);
    void blit(int width, int height, ClearMask mask, Filtering filtering);

private:
    GLenum getFormat(Format format);
    GLenum getByteFormat(ByteFormat format);
    GLint getFiltering(Filtering filtering);
    GLint getWrap(Wrap wrap);
    GLenum getTexType(TexType type);
    GLenum getAttachmentType(AttachmentType type);
    GLenum getAttachmentTarget(AttachmentTarget target);
    GLbitfield getClearMask(ClearMask mask);
    GLenum getFrameBufferOp(FrameBufferOp op);
};

