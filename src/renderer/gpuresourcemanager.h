#pragma once

#include "opengl.h"

template <class T>
class GPUResourceManager
{
public:
    id_t createTexture(TextureInfo info)
    {
        return m_backend.createTexture(info);
    }

    id_t createRenderBuffer(RenderBufferInfo info)
    {
        return m_backend.createRenderBuffer(info);
    }

    id_t createFrameBuffer(FrameBufferInfo info)
    {
        return m_backend.createFrameBuffer(info);
    }

    void deleteTexture(id_t texture)
    {
        m_backend.deleteTexture(texture);
    }

    void deleteRenderBuffer(id_t renderBuffer)
    {
        m_backend.deleteRenderBuffer(renderBuffer);
    }

    void deleteFrameBuffer(id_t frameBuffer)
    {
        m_backend.deleteFrameBuffer(frameBuffer);
    }

    void attachTexture(id_t frameBuffer, FrameBufferAttachmentInfo info)
    {
        m_backend.attachTexture(frameBuffer, info);
    }

    void attachRenderBuffer(id_t frameBuffer, FrameBufferAttachmentInfo indo)
    {
        m_backend.attachRenderBuffer(frameBuffer, indo);
    }

    void setTargetBuffers(id_t frameBuffer, FrameBufferTargetInfo info)
    {
        m_backend.setTargetBuffers(frameBuffer, info);
    }

    void bindFrameBuffer(id_t frameBuffer)
    {
        m_backend.bindFrameBuffer(frameBuffer);
    }

    void bindFrameBuffer(FrameBufferOp op, id_t frameBuffer)
    {
        m_backend.bindFrameBuffer(op, frameBuffer);
    }

    void unbindFrameBuffer()
    {
        m_backend.unbindFrameBuffer();
    }

    bool isFrameBufferComplete(id_t frameBuffer)
    {
        return m_backend.isFrameBufferComplete(frameBuffer);
    }

private:
    T m_backend;
};
