#ifndef MATERIAL_SHADER_UNIFORMS_H
#define MATERIAL_SHADER_UNIFORMS_H

#include <GL/freeglut.h>

struct MaterialShaderUniforms {
	GLint ambientColorUniform;
	GLint diffuseColorUniform;
	GLint specularColorUniform;
	GLint shininessUniform;

	MaterialShaderUniforms(GLint ambientColor, GLint diffuseColor, GLint specularColor, GLint shininess) 
		: ambientColorUniform(ambientColor),
		diffuseColorUniform(diffuseColor),
		specularColorUniform(specularColor),
		shininessUniform(shininess)
	{}

	MaterialShaderUniforms() : MaterialShaderUniforms(-1, -1, -1, -1) {}
};

#endif