#include "uniform_buffer_object.h"

#include <glad/glad.h>


UniformBufferObject::UniformBufferObject(const std::size_t bufferSize)
{
    this->bufferSize = bufferSize;
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBufferObject::~UniformBufferObject()
{

}

void UniformBufferObject::bindBufferToIndex(int index)
{
    glBindBufferRange(GL_UNIFORM_BUFFER, index, UBO, 0, bufferSize);
}

void UniformBufferObject::setBufferData(const std::size_t offset,
                                        const std::size_t dataSize, const void* data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
