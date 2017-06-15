#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>

class Texture {
private:

	GLuint m_texture;

	inline void ResetAll() { m_texture = 0; }
	void DestroyAll();

public:

	// Just generate texture
	// Do not set any texture filters or parameters
	Texture();

	// Load texture from file
	Texture(const std::string& filepath);
	virtual ~Texture();

	Texture(Texture&& texture);
	Texture& operator=(Texture&& texture);

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	inline GLuint GetTexture() const { return m_texture; }
	inline void Bind() const { glBindTexture(GL_TEXTURE_2D, m_texture); }
	inline void Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

	void CreateMipmap() const;

	// Set minification and magnification filter to GL_LINEAR
	// Default value when texture is created
	void SetMinMagBilinearFilter() const;
	
	// Set minification and magnification filter to GL_NEAREST
	void SetMinMagNearestFilter() const;

	// Set texture's wrap to repeat in both directions
	// Default value when texture is created
	void SetWrapRepeat() const;

	// Set texture's wrap to clamp to the edge in both directions
	void SetWrapClampToEdge() const;
};

#endif