#pragma once

#include "gfxapi.h"

#include <glad/gl.h>

class OpenGL
{
public:
    template <typename T>
    MeshHandles createMesh(MeshInfo<T> info);
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
    GLenum getTextureTarget(TextureTarget target);
    GLenum getAttachmentType(AttachmentType type);
    GLenum getAttachmentTarget(AttachmentTarget target);
    GLbitfield getClearMask(ClearMask mask);
    GLenum getFrameBufferOp(FrameBufferOp op);
};

template <typename T>
MeshHandles OpenGL::createMesh(MeshInfo<T> info)
{
    MeshHandles mesh;

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ebo);

    glBindVertexArray(mesh.vao);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, info.vertices.size() * sizeof(T),
                 info.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 info.indices.size() * sizeof(unsigned int),
                 info.indices.data(), GL_STATIC_DRAW);


    int index = 0;
    for (auto& pair : info.amountAndOffset)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, pair.first, GL_FLOAT, GL_FALSE,
                              sizeof(T), (void*)pair.second);
        ++index;
    }

    glBindVertexArray(0);

    return mesh;
}

