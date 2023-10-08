#include "frame_buffer.h"

#include "../log.h"
#include "gfxapi.h"

/*
void FrameBuffer::create3Dbuffer()
{
    // TODO: change 100 to the camera farplane value
    std::vector<float> shadowCascadeLevels{ 100 / 50.0f, 100 / 25.0f, 100 / 10.0f, 100 / 2.0f };
    const unsigned int SHADOW_WIDTH = 512, SHADOW_HEIGHT = 512;

    glGenFramebuffers(1, &m_FrameBuffer);

    glGenTextures(1, &m_TexcolorBuffer);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexcolorBuffer);
    glTexImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            GL_DEPTH_COMPONENT32F,
            SHADOW_WIDTH,
            SHADOW_HEIGHT,
            int(shadowCascadeLevels.size()) + 1,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_TexcolorBuffer, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        throw 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
*/

FrameBuffer::FrameBuffer(int width, int height, GPUResourceManager<OpenGL> rm) 
    : m_width(width), m_height(height), m_rm(rm)
{
}

void FrameBuffer::resizeBuffer(int width, int height)
{
    m_width = width;
    m_height = height;
    deleteBuffer();
    createBuffer();
}

void FrameBuffer::bind()
{
    m_rm.bindFrameBuffer(m_frameBufferID);
}

void FrameBuffer::bind(FrameBufferOp op)
{
    m_rm.bindFrameBuffer(op, m_frameBufferID);
}

void FrameBuffer::unbind()
{
    m_rm.unbindFrameBuffer();
}

unsigned int FrameBuffer::getID()
{
    return m_frameBufferID;
}

ColorDepthStencilBuffer::ColorDepthStencilBuffer(int width, int height, GPUResourceManager<OpenGL> rm)
    : FrameBuffer(width, height, rm)
{
    createBuffer();
}

ColorDepthStencilBuffer::~ColorDepthStencilBuffer()
{
    deleteBuffer();
}

id_t ColorDepthStencilBuffer::getColorAttachmentID()
{
    return m_colorAttachmentID;
}

id_t ColorDepthStencilBuffer::getDepthAttachmentID()
{
    return m_depthAttachmentID;
}

void ColorDepthStencilBuffer::createBuffer()
{ 
    m_frameBufferID = m_rm.createFrameBuffer({
        .debugName = "MainFrameBuffer",
    });
    m_colorAttachmentID = m_rm.createTexture({
        .width = m_width,
        .height = m_height,
        .format = Format::RGB,
        .byteFormat = ByteFormat::RGB,
    });
    m_depthAttachmentID = m_rm.createTexture({
        .width = m_width,
        .height = m_height,
        .format = Format::DEPTH,
        .byteFormat = ByteFormat::DEPTH,
        .filtering = Filtering::POINT,
        .type = TexType::FLOAT,
    });
    m_rm.attachTexture(m_frameBufferID, {
        .attachment = m_colorAttachmentID,
        .type = AttachmentType::COLOR0,
        .target = AttachmentTarget::TEX2D,
    });
    m_rm.attachRenderBuffer(m_frameBufferID, {
        .attachment = m_depthAttachmentID,
        .type = AttachmentType::DEPTH,
        .target = AttachmentTarget::TEX2D,
    });

    if (!m_rm.isFrameBufferComplete(m_frameBufferID))
    {
        LOG_ERROR("ColorDepthStencilBuffer is not complete!");
    }
}

void ColorDepthStencilBuffer::deleteBuffer()
{
    m_rm.deleteTexture(m_colorAttachmentID);
    m_rm.deleteRenderBuffer(m_depthAttachmentID);
    m_rm.deleteFrameBuffer(m_frameBufferID);
}

ColorBuffer::ColorBuffer(int width, int height, GPUResourceManager<OpenGL> rm)
    : FrameBuffer(width, height, rm)
{
    createBuffer();
}

ColorBuffer::~ColorBuffer()
{
    deleteBuffer();
}

id_t ColorBuffer::getColorAttachmentID()
{
    return m_colorAttachmentID;
}

void ColorBuffer::createBuffer()
{ 
    m_frameBufferID = m_rm.createFrameBuffer({
        .debugName = "ColorFrameBuffer",
    });
    m_colorAttachmentID = m_rm.createTexture({
        .width = m_width,
        .height = m_height,
        .format = Format::RED,
        .byteFormat = ByteFormat::RED,
        .filtering = Filtering::POINT,
        .type = TexType::FLOAT,
    });
    m_rm.attachTexture(m_frameBufferID, {
        .attachment = m_colorAttachmentID,
        .type = AttachmentType::COLOR0,
        .target = AttachmentTarget::TEX2D,
    });

    if (!m_rm.isFrameBufferComplete(m_frameBufferID))
    {
        LOG_ERROR("ColorBuffer is not complete!");
    }
}

void ColorBuffer::deleteBuffer()
{
    m_rm.deleteTexture(m_colorAttachmentID);
    m_rm.deleteFrameBuffer(m_frameBufferID);
}

DepthBuffer::DepthBuffer(int width, int height, GPUResourceManager<OpenGL> rm)
    : FrameBuffer(width, height, rm)
{
    createBuffer();
}

DepthBuffer::~DepthBuffer()
{
    deleteBuffer();
}

id_t DepthBuffer::getDepthAttachmentID()
{
    return m_depthAttachmentID;
}

void DepthBuffer::createBuffer()
{
    m_frameBufferID = m_rm.createFrameBuffer({
        .debugName = "ShadowFrameBuffer",
        .hasColorBuffer = false,
    });
    m_depthAttachmentID = m_rm.createTexture({
        .width = m_width,
        .height = m_height,
        .format = Format::DEPTH,
        .byteFormat = ByteFormat::DEPTH,
        .filtering = Filtering::POINT,
        .wrap = Wrap::CLAMPBORDER,
        .type = TexType::FLOAT,
    });
    m_rm.attachTexture(m_frameBufferID, {
        .attachment = m_depthAttachmentID,
        .type = AttachmentType::DEPTH,
        .target = AttachmentTarget::TEX2D,
    });
    
    if (!m_rm.isFrameBufferComplete(m_frameBufferID))
    {
        LOG_ERROR("DepthBuffer is not complete!");
    }
}

void DepthBuffer::deleteBuffer()
{
    m_rm.deleteTexture(m_depthAttachmentID);
    m_rm.deleteFrameBuffer(m_frameBufferID);
}

GBuffer::GBuffer(int width, int height, GPUResourceManager<OpenGL> rm)
    : FrameBuffer(width, height, rm)
{
    createBuffer();
}

GBuffer::~GBuffer()
{
    deleteBuffer();
}

id_t GBuffer::getPositionAttachmentID()
{
    return m_positionAttachmentID;
}

id_t GBuffer::getNormalAttachmentID()
{
    return m_normalAttachmentID;
}

id_t GBuffer::getAlbedoSpecAttachmentID()
{
    return m_albedoSpecAttachmentID;
}

id_t GBuffer::getDepthAttachmentID()
{
    return m_depthAttachmentID;
}

void GBuffer::createBuffer()
{
    m_frameBufferID = m_rm.createFrameBuffer({
        .debugName = "GBuffer",
    });
    m_positionAttachmentID = m_rm.createTexture({
        .debugName = "PositionTex",
        .width = m_width,
        .height = m_height,
        .format = Format::RGBA,
        .byteFormat = ByteFormat::RGBA16F,
        .filtering = Filtering::POINT,
        .wrap = Wrap::CLAMPEDGE,
        .type = TexType::FLOAT,
    });
    m_normalAttachmentID = m_rm.createTexture({
        .debugName = "NormalTex",
        .width = m_width,
        .height = m_height,
        .format = Format::RGBA,
        .byteFormat = ByteFormat::RGBA16F,
        .filtering = Filtering::POINT,
        .type = TexType::FLOAT,
    });
    m_albedoSpecAttachmentID = m_rm.createTexture({
        .debugName = "AlbedoTex",
        .width = m_width,
        .height = m_height,
        .format = Format::RGBA,
        .byteFormat = ByteFormat::RGBA,
        .filtering = Filtering::POINT,
    });
    m_depthAttachmentID = m_rm.createTexture({
        .debugName = "DepthTexture",
        .width = m_width,
        .height = m_height,
        .format = Format::DEPTH,
        .byteFormat = ByteFormat::DEPTH,
        .filtering = Filtering::POINT,
        .type = TexType::FLOAT,
    });
    m_rm.attachTexture(m_frameBufferID, {
        .attachment = m_positionAttachmentID,
        .type = AttachmentType::COLOR0,
        .target = AttachmentTarget::TEX2D,
    });
    m_rm.attachTexture(m_frameBufferID, {
        .attachment = m_normalAttachmentID,
        .type = AttachmentType::COLOR1,
        .target = AttachmentTarget::TEX2D,
    });
    m_rm.attachTexture(m_frameBufferID, {
        .attachment = m_albedoSpecAttachmentID,
        .type = AttachmentType::COLOR2,
        .target = AttachmentTarget::TEX2D,
    });
    m_rm.attachTexture(m_frameBufferID, {
        .attachment = m_depthAttachmentID,
        .type = AttachmentType::DEPTH,
        .target = AttachmentTarget::TEX2D,
    });

    AttachmentType types[3] = { AttachmentType::COLOR0, AttachmentType::COLOR1, AttachmentType::COLOR2 };
    m_rm.setTargetBuffers(m_frameBufferID, {
        .size = 3,
        .types = types,
    });

    if (!m_rm.isFrameBufferComplete(m_frameBufferID))
    {
        LOG_ERROR("DepthBuffer is not complete!");
    }
}

void GBuffer::deleteBuffer()
{
    m_rm.deleteTexture(m_positionAttachmentID);
    m_rm.deleteTexture(m_normalAttachmentID);
    m_rm.deleteTexture(m_albedoSpecAttachmentID);
    m_rm.deleteRenderBuffer(m_depthAttachmentID);
    m_rm.deleteFrameBuffer(m_frameBufferID);
}

