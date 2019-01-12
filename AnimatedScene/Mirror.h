#ifndef MIRROR_H
#define MIRROR_H

#include "Texture.h"
#include "MeshObject.h"

// Flat "magnifying" mirror
class Mirror final {
private:

	Texture m_colorTexture;
	Texture m_depthTexture;
	GLuint m_framebuffer;
	unsigned int m_width;
	unsigned int m_height;

	void ResizeTextures();
	void DestroyFramebuffer();

public:

	Mirror(unsigned int width, unsigned int height);
	~Mirror();

	Mirror(Mirror&& mirror);
	Mirror& operator=(Mirror&& mirror);

	Mirror(const Mirror&) = delete;
	Mirror& operator=(const Mirror&) = delete;

	inline unsigned int GetWidth() const { return m_width; }
	inline unsigned int GetHeight() const { return m_height; }

	// Call this to enable rendering of GL's content into mirror's framebuffer
	void SetActive() const;

	// Call this to disable rendering of GL's content into mirror's framebuffer
	void SetInactive() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	void BindMirrorAsTexture() const { m_colorTexture.Bind(); }
	void UnbindMirrorAsTexture() const { m_colorTexture.Unbind(); }

	Camera GetReflectedCamera(const Camera& camera, const glm::vec3& reflection) const;
};

#endif