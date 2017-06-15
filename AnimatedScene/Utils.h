#ifndef UTILS_H
#define UTILS_H

#include <GL/freeglut.h>
#include <string>
#include <vector>

namespace Utils {

	// Load .obj file
	// This function does not check for bad .obj file format!
	void LoadObjFile(const std::string& filepath,
		std::vector<GLfloat>& verticesArray,
		std::vector<GLfloat>& normalsArray,
		std::vector<GLfloat>& texelsArray);

	// Must be called before LoadTexture is used
	void InitTextureLoader();

	// Load texture from given filename
	GLuint LoadTexture(const std::string& filepath);
}

#endif