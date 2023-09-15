#pragma once

#include "gpuresourcemanager.h"


class FrameBuffer
{
public:
    FrameBuffer(int width, int height, GPUResourceManager<OpenGL> rm);
    void resizeBuffer(int width, int height);
    void bind();
    void unbind();
    id_t getID();

protected:
    virtual void createBuffer() = 0;
    virtual void deleteBuffer() = 0;

    int m_width;
    int m_height;
    unsigned int m_frameBufferID;
    GPUResourceManager<OpenGL> m_rm;
};

class ColorDepthStencilBuffer : public FrameBuffer
{
public:
    ColorDepthStencilBuffer(int width, int height, GPUResourceManager<OpenGL> rm);
    ~ColorDepthStencilBuffer();
    id_t getColorAttachmentID();

private:
    void createBuffer() override;
    void deleteBuffer() override;

    id_t m_colorAttachmentID;
    id_t m_depthStencilAttachmentID;
};

class DepthBuffer : public FrameBuffer
{
public:
    DepthBuffer(int width, int height, GPUResourceManager<OpenGL> rm);
    ~DepthBuffer();
    id_t getDepthAttachmentID();

private:
    void createBuffer() override;
    void deleteBuffer() override;

    id_t m_depthAttachmentID;
};

class GBuffer : public FrameBuffer
{
public:
    GBuffer(int width, int height, GPUResourceManager<OpenGL> rm);
    ~GBuffer();

private:
    void createBuffer() override;
    void deleteBuffer() override;

    id_t m_positionAttachmentID;
    id_t m_normalAttachmentID;
    id_t m_albedoSpecAttachmentID;
    id_t m_depthStencilAttachmentID;
};

