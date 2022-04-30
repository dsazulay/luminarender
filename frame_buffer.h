#pragma once

class FrameBuffer
{
public:
    FrameBuffer(int width, int height);
    ~FrameBuffer();
    void resizeBuffer(int width, int height);
    unsigned int getID();
    unsigned int getTexcolorBufferID();

private:
    unsigned int m_FrameBuffer{};
    unsigned int m_TexcolorBuffer{};
    unsigned int m_RenderBuffer{};
    unsigned int m_lightDepthMaps{};

    int m_Width;
    int m_Height;

    void createBuffer();
    void create3Dbuffer();
    void deleteBuffer();
};
