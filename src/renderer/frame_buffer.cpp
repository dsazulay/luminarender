#include "frame_buffer.h"

#include "../log.h"

#include <glad/glad.h>

#include <vector>


void FrameBuffer::createBuffer()
{
    glGenFramebuffers(1, &m_FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

    glGenTextures(1, &m_TexcolorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_TexcolorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TexcolorBuffer, 0);

    glGenRenderbuffers(1, &m_RenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resizeBuffer(int width, int height)
{
    deleteBuffer();

    m_Width = width;
    m_Height = height;

    if (m_type == Type::Shadow)
        createShadowBuffer();
    else
        createBuffer();
}

unsigned int FrameBuffer::getID()
{
    return m_FrameBuffer;
}

unsigned int FrameBuffer::getTexcolorBufferID()
{
    return m_TexcolorBuffer;
}

FrameBuffer::FrameBuffer(int width, int height, Type type)
{
    m_Width = width;
    m_Height = height;
    m_type = type;
}

void FrameBuffer::deleteBuffer()
{
    glDeleteTextures(1, &m_TexcolorBuffer);
    if (m_type != Type::Shadow)
        glDeleteRenderbuffers(1, &m_RenderBuffer);
    glDeleteFramebuffers(1, &m_FrameBuffer);
}

FrameBuffer::~FrameBuffer()
{

}

void FrameBuffer::create3Dbuffer()
{
    // TODO: change 100 to the camera farplane value
    std::vector<float> shadowCascadeLevels{ 100 / 50.0f, 100 / 25.0f, 100 / 10.0f, 100 / 2.0f };
    const unsigned int SHADOW_WIDTH = 512, SHADOW_HEIGHT = 512;

    glGenFramebuffers(1, &m_FrameBuffer);

    glGenTextures(1, &m_TexcolorBuffer);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexcolorBuffer);
    glTexImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            GL_DEPTH_COMPONENT32F,
            SHADOW_WIDTH,
            SHADOW_HEIGHT,
            int(shadowCascadeLevels.size()) + 1,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_TexcolorBuffer, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        throw 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::createShadowBuffer()
{
    glGenFramebuffers(1, &m_FrameBuffer);

    // create depth texture
    glGenTextures(1, &m_TexcolorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_TexcolorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TexcolorBuffer, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        throw 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
