#pragma once

#include <vector>

enum TextureType
{
    Tex2D,
    CubeMap,
    HDR
};

class Texture
{
public:
    Texture() = default;
    Texture(TextureType type, int width, int height, int nrChannels, unsigned char* data);
    Texture(TextureType type, int width, int height, float* data);
    Texture(TextureType type, int width, int height, std::vector<unsigned char*> data);
    unsigned int ID();
    TextureType type();

    unsigned int m_ID;
    TextureType m_type;
};
