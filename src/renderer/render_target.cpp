#include "render_target.h"

#include <glad/glad.h>


void RenderTarget::updateViewportDimensions()
{
    glViewport(0, 0, m_viewportWidth, m_viewportHeight);
}

void RenderTarget::bindFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer.getID());
}

void RenderTarget::unbindFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::resizeFrameBuffer(int width, int height)
{
    m_viewportWidth = width;
    m_viewportHeight = height;
    m_frameBuffer.resizeBuffer(m_viewportWidth, m_viewportHeight);
}

void RenderTarget::clearFrameBuffer()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(m_clearMask);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

RenderTarget::RenderTarget(int width, int height, FrameBuffer::Type type) : m_frameBuffer(width, height, type)
{

}

unsigned int RenderTarget::getTextureID()
{
    return m_frameBuffer.getTexcolorBufferID();
}

void RenderTarget::setAsTarget()
{
    bindFrameBuffer();
    updateViewportDimensions();
    clearFrameBuffer();
}

void RenderTarget::unsetAsTarget()
{
    unbindFrameBuffer();
}

void RenderTarget::clearMask(int mask)
{
    m_clearMask = mask;
}
