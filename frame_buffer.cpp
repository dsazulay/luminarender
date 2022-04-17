//
// Created by Diego Azulay on 13/04/22.
//

#include "frame_buffer.h"

#include <glad/glad.h>
#include <iostream>

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
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resizeBuffer(int width, int height)
{
    deleteBuffer();

    m_Width = width;
    m_Height = height;

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

FrameBuffer::FrameBuffer(int width, int height)
{
    m_Width = width;
    m_Height = height;
}

void FrameBuffer::deleteBuffer()
{

}

FrameBuffer::~FrameBuffer()
{

}