#include "opengl.h"

#include "../log.h"

id_t OpenGL::createTexture(TextureInfo info)
{
    LOG_INFO("OpenGL backend create texture: {}", info.debugName);
    id_t id;
    GLenum format = getFormat(info.format);
    GLint filtering = getFiltering(info.filtering);
    GLint wrap = getWrap(info.wrap);
    GLenum type = getTexType(info.type);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, info.width, info.height,
            0, format, type, info.initialData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    // TODO: pass border color as parameter
    if (info.wrap == Wrap::CLAMPBORDER)
    {
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    // TODO: handle mip mapping
    glBindTexture(GL_TEXTURE_2D, 0);

    return id;
}

id_t OpenGL::createRenderBuffer(RenderBufferInfo info)
{
    LOG_INFO("OpenGL backend create render buffer: {}", info.debugName);
    id_t id;
    GLenum format = getFormat(info.format); 

    glGenRenderbuffers(1, &id);
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    glRenderbufferStorage(GL_RENDERBUFFER, format, info.width, info.height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    return id;
}

id_t OpenGL::createFrameBuffer(FrameBufferInfo info)
{
    LOG_INFO("OpenGL backend create frame buffer: {}", info.debugName);
    id_t id;

    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    if (!info.hasColorBuffer)
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return id;
}

void OpenGL::deleteTexture(id_t texture)
{
    LOG_INFO("OpenGL backend delete texture");
    glDeleteTextures(1, &texture);
}

void OpenGL::deleteRenderBuffer(id_t renderBuffer)
{
    LOG_INFO("OpenGL backend delete render buffer");
    glDeleteRenderbuffers(1, &renderBuffer);
}

void OpenGL::deleteFrameBuffer(id_t frameBuffer)
{
    LOG_INFO("OpenGL backend delete frame buffer");
    glDeleteFramebuffers(1, &frameBuffer);
}

void OpenGL::attachTexture(id_t frameBuffer, FrameBufferAttachmentInfo info)
{
    LOG_INFO("OpenGL backend attach texture: {}", info.debugName);
    GLenum type = getAttachmentType(info.type);
    GLenum target = getAttachmentTarget(info.target);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, type, target, info.attachment, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGL::attachRenderBuffer(id_t frameBuffer, FrameBufferAttachmentInfo info)
{
    LOG_INFO("OpenGL backend attach render buffer: {}", info.debugName);
    GLenum type = getAttachmentType(info.type);
    GLenum target = getAttachmentTarget(info.target);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, type, target, info.attachment);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGL::bindFrameBuffer(id_t frameBuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
}

void OpenGL::unbindFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool OpenGL::isFrameBufferComplete(id_t frameBuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    bool isComplete = glCheckFramebufferStatus(GL_FRAMEBUFFER)
        == GL_FRAMEBUFFER_COMPLETE;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return isComplete;
}

GLenum OpenGL::getFormat(Format format)
{
    switch (format)
    {
        case Format::RGB:
            return GL_RGB;
        case Format::RGBA:
            return GL_RGBA;
        case Format::DEPTH:
            return GL_DEPTH_COMPONENT;
        case Format::DEPTH24_STENCIL8:
            return GL_DEPTH24_STENCIL8;
    }
}

GLint OpenGL::getFiltering(Filtering filtering)
{
    switch (filtering)
    {
        case Filtering::POINT:
            return GL_NEAREST;
        case Filtering::BILINEAR:
            return GL_LINEAR;
    }
}

GLint OpenGL::getWrap(Wrap wrap)
{
    switch (wrap)
    {
        case Wrap::REPEAT:
            return GL_REPEAT;
        case Wrap::MIRROR:
            return GL_MIRRORED_REPEAT;
        case Wrap::CLAMPEDGE:
            return GL_CLAMP_TO_EDGE;
        case Wrap::CLAMPBORDER:
            return GL_CLAMP_TO_BORDER;
    }
}

GLenum OpenGL::getTexType(TexType type)
{
    switch (type)
    {
        case TexType::UBYTE:
            return GL_UNSIGNED_BYTE;
        case TexType::FLOAT:
            return GL_FLOAT;
    }
}

GLenum OpenGL::getAttachmentType(AttachmentType type)
{
    switch (type)
    {
        case AttachmentType::COLOR0:
            return GL_COLOR_ATTACHMENT0;
         case AttachmentType::COLOR1:
            return GL_COLOR_ATTACHMENT1;
         case AttachmentType::COLOR2:
            return GL_COLOR_ATTACHMENT2;
         case AttachmentType::COLOR3:
            return GL_COLOR_ATTACHMENT3;
         case AttachmentType::COLOR4:
            return GL_COLOR_ATTACHMENT4;
        case AttachmentType::DEPTH:
            return GL_DEPTH_ATTACHMENT;
        case AttachmentType::STENCIL:
            return GL_STENCIL_ATTACHMENT;
        case AttachmentType::DEPTHSTENCIL:
            return GL_DEPTH_STENCIL_ATTACHMENT;
    }
}

GLenum OpenGL::getAttachmentTarget(AttachmentTarget target)
{
    switch (target)
    {
        case AttachmentTarget::TEX2D:
            return GL_TEXTURE_2D;
        case AttachmentTarget::RENDERBUFFER:
            return GL_RENDERBUFFER;
    }
}
