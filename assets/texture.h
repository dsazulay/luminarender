//
// Created by Diego Azulay on 23/04/22.
//

#ifndef INTERACTIVEGRAPHICS_TEXTURE_H
#define INTERACTIVEGRAPHICS_TEXTURE_H

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
    Texture(TextureType type, int width, int height, int nrChannels, unsigned char* data);
    Texture(TextureType type, int width, int height, float* data);
    Texture(TextureType type, int width, int height, std::vector<unsigned char*> data);
    unsigned int ID();
    TextureType type();

private:
    unsigned int m_ID;
    TextureType m_type;
};


#endif //INTERACTIVEGRAPHICS_TEXTURE_H
