#ifndef MESH_OBJECT_H
#define MESH_OBJECT_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ModelObject.h"
#include "Camera.h"
#include "MaterialShaderUniforms.h"
#include "MatrixShaderUniforms.h"
#include "SurfaceMaterial.h"
#include <string>
#include <vector>

// OpenGL Mesh object loaded from .obj (obj wavefront) file
class MeshObject : public ModelObject {
private:

	GLuint m_verticesVBO;
	GLuint m_normalsVBO;
	GLuint m_texelsVBO;
	GLuint m_arraySize;
	GLuint m_meshVAO;

	void CreateVerticesVBO(const std::vector<GLfloat>& vertices);
	void CreateNormalsVBO(const std::vector<GLfloat>& normals);
	void CreateTexelsVBO(const std::vector<GLfloat>& texels);
	void CreateMeshVAO(GLint positionShaderAttribute, GLint normalShaderAttribute, GLint texelShaderAttribute);

	void CreateMeshFromObjFile(const std::string& filepath,
		GLint positionShaderAttribute,
		GLint normalShaderAttribute,
		GLint texelShaderAttribute);

	// Reset all members to initial values, do not destroy anything
	void ResetAll();

	// Destroy and free content
	void DestroyAll();

public:
	
	MeshObject(const std::string& filepath,
		GLint positionShaderAttribute,
		GLint normalShaderAttribute,
		GLint texelShaderAttribute);

	virtual ~MeshObject();

	MeshObject(MeshObject&& uc);
	MeshObject& operator=(MeshObject&& uc);

	// Do not copy VBOs, VAO etc.
	MeshObject(const MeshObject& c) = delete;
	MeshObject& operator=(const MeshObject&) = delete;

	void Draw(const Camera& camera,
		const SurfaceMaterial& surfaceMaterial,
		const MatrixShaderUniforms& matrixUniforms,
		const MaterialShaderUniforms& materialUniforms) const;
};

#endif