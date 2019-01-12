#include "Mirror.h"

Mirror::Mirror(unsigned int width, unsigned int height)
	: m_width(width),
	m_height(height)
{
	glGenFramebuffers(1, &m_framebuffer);

	if (m_framebuffer == 0) {
		throw std::runtime_error("Unable to create framebuffer");
	}

	ResizeTextures();
	m_colorTexture.SetWrapClampToEdge();
	m_colorTexture.SetMinMagBilinearFilter();

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture.GetTexture(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture.GetTexture(), 0);
	
	GLenum drawBuffer = GL_COLOR_ATTACHMENT0;

	// Bind color attachment with this framebuffer
	glDrawBuffers(1, &drawBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		DestroyFramebuffer();
		throw std::runtime_error("Unable to attach framebuffer with color and depth textures");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Mirror::~Mirror()
{
	DestroyFramebuffer();
}

Mirror::Mirror(Mirror&& mirror)
{
	*this = std::move(mirror);
}

Mirror& Mirror::operator=(Mirror&& mirror)
{
	DestroyFramebuffer();
	m_width = mirror.m_width;
	m_height = mirror.m_height;
	m_framebuffer = mirror.m_framebuffer;
	m_colorTexture = std::move(mirror.m_colorTexture);
	m_depthTexture = std::move(mirror.m_depthTexture);
	mirror.m_framebuffer = 0;
	return *this;
}

void Mirror::SetActive() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Mirror::ResizeTextures()
{
	m_colorTexture.Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	m_colorTexture.Unbind();
	m_depthTexture.Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_width, m_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	m_depthTexture.Unbind();
}

void Mirror::DestroyFramebuffer()
{
	if (m_framebuffer != 0) {
		glDeleteFramebuffers(1, &m_framebuffer);
	}
}

Camera Mirror::GetReflectedCamera(const Camera& camera, const glm::vec3& reflection) const
{
	auto cameraCopy = camera;
	cameraCopy.SetEyePositionManual(camera.GetEyePosition() * reflection);
	return cameraCopy; // RVO C++
}
