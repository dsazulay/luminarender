#pragma once

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
