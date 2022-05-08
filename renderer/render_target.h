#pragma once

#include "../frame_buffer.h"

class RenderTarget
{
public:
    RenderTarget(int width, int height, FrameBuffer::Type type);
    void resizeFrameBuffer(int width, int height);
    unsigned int getTextureID();
    void setAsTarget();
    void unsetAsTarget();
    void clearMask(int mask);

private:
    void updateViewportDimensions();
    void bindFrameBuffer();
    void unbindFrameBuffer();
    virtual void clearFrameBuffer();

    FrameBuffer m_frameBuffer;
    int m_viewportWidth;
    int m_viewportHeight;
    int m_clearMask;
};