#ifndef STICKER_H
#define STICKER_H

#define GLEW_STATIC
#include <GL/glew.h>

#include "MaterialShaderUniforms.h"
#include "MatrixShaderUniforms.h"
#include "SurfaceMaterial.h"
#include "Camera.h"

// Generic top-faced sticker used as surface on rubik cube
class Sticker final {
public:

	enum Color {
		WHITE = 0,
		RED,
		BLUE,
		ORANGE,
		GREEN,
		YELLOW
	};

	static const SurfaceMaterial& GetStickerMaterial(Color color);

private:

	GLuint m_verticesAndNormalsVBO;
	GLuint m_verticesAndNormalsCount;
	GLuint m_stickerVAO;

	// Reset all values to zero, do not destroy anything
	void ResetAll();

	// Remove and free it's content
	void DestroyAll();

	// Setup sticker's vbo and vao
	void CreateMesh(GLint positionShaderAttribute, GLint normalShaderAttribute);

public:

	Sticker(GLint positionShaderAttribute, GLint normalShaderAttribute);
	~Sticker();

	Sticker(Sticker&& s);
	Sticker& operator=(Sticker&& s);

	// FYI: OpenGL mesh
	Sticker(const Sticker&) = delete;
	Sticker& operator=(const Sticker&) = delete;

	inline float StickerSize() const { return 1.f; }

	// Draw this sticker on given position
	void Draw(const Camera& camera,
		const glm::mat4& modelMatrix,
		const SurfaceMaterial& surfaceMaterial,
		const MatrixShaderUniforms& matrixUniforms,
		const MaterialShaderUniforms& materialUniforms) const;
};

#endif