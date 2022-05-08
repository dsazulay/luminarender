#include "render_system.h"
#include "../pch.h"
#include <glad/glad.h>

void RenderSystem::updateViewportDimensions()
{
    glViewport(0, 0, m_viewportWidth, m_viewportHeight);
}

void RenderSystem::bindFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer.getID());
}

void RenderSystem::unbindFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::clearFrameBuffer()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

RenderSystem::RenderSystem(int width, int height, FrameBuffer::Type type) : m_frameBuffer(width, height, type)
{

}
