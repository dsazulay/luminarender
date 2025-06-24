#pragma once

#include "gfxapi.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

template <class T>
class GPUResourceManager
{
public:
    template <typename U>
    MeshHandles createMesh(MeshInfo<U> info)
    {
        return m_backend.createMesh(info);
    }

    id_t createShader(ShaderInfo info)
    {
        return m_backend.createShader(info);
    }

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

    void attachRenderBuffer(id_t frameBuffer, FrameBufferAttachmentInfo info)
    {
        m_backend.attachRenderBuffer(frameBuffer, info);
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

    void bindShader(id_t shader)
    {
        m_backend.bindShader(shader);
    }

    void setUniform(id_t shader, const char *name, int value)
    {
        m_backend.setUniform(shader, name, value);
    }

    void setUniform(id_t shader, const char *name, float value)
    {
        m_backend.setUniform(shader, name, value);
    }

    void setUniform(id_t shader, const char *name, glm::vec3 value)
    {
        m_backend.setUniform(shader, name, value);
    }

    void setUniform(id_t shader, const char *name, glm::vec4 value)
    {
        m_backend.setUniform(shader, name, value);
    }

    void setUniform(id_t shader, const char *name, glm::mat3 value)
    {
        m_backend.setUniform(shader, name, value);
    }

    void setUniform(id_t shader, const char *name, glm::mat4 value)
    {
        m_backend.setUniform(shader, name, value);
    }

private:
    T m_backend;
};
