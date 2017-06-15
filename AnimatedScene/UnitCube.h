#ifndef UNIT_CUBE_H
#define UNIT_CUBE_H

#define GLEW_STATIC
#include <GL/glew.h>

#include "Camera.h"
#include "MatrixShaderUniforms.h"
#include "MaterialShaderUniforms.h"
#include "ModelObject.h"

// Simple unit cube which is used for drawing Rubik's cube parts (after specific transformations ofc.)
class UnitCube final : public ModelObject {
private:

	GLuint m_verticesAndNormalsVBO;
	GLuint m_indicesVBO;
	GLuint m_numIndices;
	GLuint m_cubeVAO;

	// Reset all members to initial values, do not destroy anything
	void ResetAll();

	// Destroy and free content
	void DestroyAll();

	void CreateVerticesAndNormalsVBO();
	void CreateIndicesVBO();
	void CreateCubeVAO(GLint positionShaderAttribute, GLint normalShaderAttribute);

public:

	UnitCube(GLint positionShaderAttribute, GLint normalShaderAttribute);
	~UnitCube();

	UnitCube(UnitCube&& uc);
	UnitCube& operator=(UnitCube&& uc);

	// FYI: OpenGL mesh
	UnitCube(const UnitCube& c) = delete;
	UnitCube& operator=(const UnitCube&) = delete;

	inline float CubeSize() const { return 1.f; }

	void Draw(const Camera& camera,
		const MatrixShaderUniforms& matrixUniforms,
		const MaterialShaderUniforms& materialUniforms) const;
};

#endif
