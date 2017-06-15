#ifndef MATRIX_SHADER_UNIFORMS_H
#define MATRIX_SHADER_UNIFORMS_H

#include <GL/freeglut.h>

// Transformation matrices and their uniform locations in shaders
struct MatrixShaderUniforms {
	GLint pvmMatrixUniform;
	GLint normalMatrixUniform;
	GLint modelMatrixUniform;

	MatrixShaderUniforms(GLint pvmMatrix, GLint normalMatrix, GLint modelMatrix)
		: pvmMatrixUniform(pvmMatrix),
		normalMatrixUniform(normalMatrix),
		modelMatrixUniform(modelMatrix)
	{}

	MatrixShaderUniforms() : MatrixShaderUniforms(-1, -1, -1) {}
};

#endif