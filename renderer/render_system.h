#pragma once

#include "../frame_buffer.h"

class RenderSystem
{
public:
    RenderSystem(int width, int height, FrameBuffer::Type type);
    void resizeFrameBuffer(int width, int height);
    unsigned int getTextureID();

protected:
    void updateViewportDimensions();
    void bindFrameBuffer();
    void unbindFrameBuffer();
    virtual void clearFrameBuffer();

    FrameBuffer m_frameBuffer;
    int m_viewportWidth;
    int m_viewportHeight;
};