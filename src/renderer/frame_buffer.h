#pragma once

#include "gpuresourcemanager.h"

class FrameBuffer
{
public:
    enum class Type
    {
        Color,
        Shadow,
    };

    FrameBuffer(int width, int height, Type type);
    ~FrameBuffer();
    void resizeBuffer(int width, int height);
    unsigned int getID();
    unsigned int getTexcolorBufferID();

private:
    unsigned int m_FrameBuffer{};
    unsigned int m_TexcolorBuffer{};
    unsigned int m_RenderBuffer{};

    int m_Width;
    int m_Height;
    Type m_type;

    void createBuffer();
    void createShadowBuffer();
    void create3Dbuffer();
    void deleteBuffer();
};

class NewFrameBuffer
{
public:
    NewFrameBuffer(int width, int height, GPUResourceManager<OpenGL> rm);
    void resizeBuffer(int width, int height);
    void bind();
    void unbind();
    unsigned int getID();

protected:
    virtual void createBuffer() = 0;
    virtual void deleteBuffer() = 0;

    int m_width;
    int m_height;
    unsigned int m_frameBufferID;
    GPUResourceManager<OpenGL> m_rm;
};

class ColorDepthStencilBuffer : public NewFrameBuffer
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

class DepthBuffer : public NewFrameBuffer
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

class GBuffer : public NewFrameBuffer
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

