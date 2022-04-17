//
// Created by Diego Azulay on 13/04/22.
//

#ifndef INTERACTIVEGRAPHICS_UNIFORM_BUFFER_OBJECT_H
#define INTERACTIVEGRAPHICS_UNIFORM_BUFFER_OBJECT_H

#include <cstddef>

class UniformBufferObject
{
public:
    UniformBufferObject(const std::size_t bufferSize);
    ~UniformBufferObject();

    void bindBufferToIndex(int index);
    void setBufferData(const std::size_t offset, const std::size_t dataSize, const void* data);

private:
    unsigned int UBO;
    std::size_t bufferSize;
};


#endif //INTERACTIVEGRAPHICS_UNIFORM_BUFFER_OBJECT_H
