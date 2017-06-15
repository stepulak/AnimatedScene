#include "Texture.h"
#include "Utils.h"

Texture::Texture()
{
	glGenTextures(1, &m_texture);
	if (m_texture == 0) {
		throw std::runtime_error("Unable to generate texture");
	}
}

Texture::Texture(const std::string& filepath)
	: m_texture(Utils::LoadTexture(filepath))
{
	SetMinMagBilinearFilter();
	SetWrapRepeat();
}

Texture::~Texture()
{
	DestroyAll();
}

Texture::Texture(Texture&& texture)
{
	*this = std::forward<Texture>(texture);
}

Texture& Texture::operator=(Texture&& texture)
{
	DestroyAll();
	m_texture = texture.m_texture;
	texture.ResetAll();
	return *this;
}

void Texture::DestroyAll()
{
	if (m_texture != 0) {
		glDeleteTextures(1, &m_texture);
	}
}

void Texture::CreateMipmap() const
{
	Bind();
	glGenerateMipmap(GL_TEXTURE_2D);
	Unbind();
}

void Texture::SetMinMagBilinearFilter() const
{
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	Unbind();
}

void Texture::SetMinMagNearestFilter() const
{
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	Unbind();
}

void Texture::SetWrapRepeat() const
{
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	Unbind();
}

void Texture::SetWrapClampToEdge() const
{
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	Unbind();
}
