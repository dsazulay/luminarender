#pragma once

#include "../frame_buffer.h"

class RenderSystem
{
public:
    RenderSystem(int width, int height, FrameBuffer::Type type);
protected:
    void updateViewportDimensions();
    void bindFrameBuffer();
    void unbindFrameBuffer();
    virtual void clearFrameBuffer();

    FrameBuffer m_frameBuffer;
    float m_viewportWidth;
    float m_viewportHeight;
};