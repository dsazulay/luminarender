//
// Created by Diego Azulay on 13/04/22.
//

#ifndef INTERACTIVEGRAPHICS_FRAME_BUFFER_H
#define INTERACTIVEGRAPHICS_FRAME_BUFFER_H


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
    int m_Width;
    int m_Height;

    void createBuffer();
    void deleteBuffer();
};



#endif //INTERACTIVEGRAPHICS_FRAME_BUFFER_H
