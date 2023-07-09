#pragma once

class FrameBuffer
{
public:
    enum class Type
    {
        Color,
        Shadow
    };

    FrameBuffer(int width, int height, Type type);
    ~FrameBuffer();
    void resizeBuffer(int width, int height);
    unsigned int getID();
    unsigned int getTexcolorBufferID();
    unsigned int m_lightDepthMaps{};

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
