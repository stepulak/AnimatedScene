#include "MeshObject.h"
#include "Utils.h"

#include <glm/gtc/type_ptr.hpp>

MeshObject::MeshObject(const std::string& filepath, GLint positionShaderAttribute, GLint normalShaderAttribute, GLint texelShaderAttribute)
{
	ResetAll();
	CreateMeshFromObjFile(filepath, positionShaderAttribute, normalShaderAttribute, texelShaderAttribute);
}

MeshObject::~MeshObject()
{
	DestroyAll();
}

MeshObject::MeshObject(MeshObject&& uc)
{
	*this = std::forward<MeshObject>(uc);
}

MeshObject& MeshObject::operator=(MeshObject&& uc)
{
	DestroyAll();
	m_modelMatrix = uc.m_modelMatrix; // ModelObject::operator=
	m_meshVAO = uc.m_meshVAO;
	m_verticesVBO = uc.m_verticesVBO;
	m_normalsVBO = uc.m_normalsVBO;
	m_texelsVBO = uc.m_texelsVBO;
	m_arraySize = uc.m_arraySize;
	uc.ResetAll();
	return *this;
}

void MeshObject::CreateVerticesVBO(const std::vector<GLfloat>& vertices)
{
	glGenBuffers(1, &m_verticesVBO);
	if (m_verticesVBO == 0) {
		DestroyAll();
		throw std::runtime_error("Unable to create vertices VBO");
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), 
		reinterpret_cast<const void*>(vertices.data()), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshObject::CreateNormalsVBO(const std::vector<GLfloat>& normals)
{
	glGenBuffers(1, &m_normalsVBO);
	if (m_normalsVBO == 0) {
		DestroyAll();
		throw std::runtime_error("Unable to create normals VBO");
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(),
		reinterpret_cast<const void*>(normals.data()), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshObject::CreateTexelsVBO(const std::vector<GLfloat>& texels)
{
	glGenBuffers(1, &m_texelsVBO);
	if (m_texelsVBO == 0) {
		DestroyAll();
		throw std::runtime_error("Unable to create texels VBO");
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_texelsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texels.size(),
		reinterpret_cast<const void*>(texels.data()), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshObject::CreateMeshVAO(GLint positionShaderAttribute, GLint normalShaderAttribute, GLint texelShaderAttribute)
{
	glGenVertexArrays(1, &m_meshVAO);

	if (m_meshVAO == 0) {
		DestroyAll();
		throw std::runtime_error("Unable to create mesh VAO");
	}

	glBindVertexArray(m_meshVAO);

	if (positionShaderAttribute >= 0) { // valid
		glBindBuffer(GL_ARRAY_BUFFER, m_verticesVBO);
		glEnableVertexAttribArray(positionShaderAttribute);
		glVertexAttribPointer(positionShaderAttribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}
	if (normalShaderAttribute >= 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_normalsVBO);
		glEnableVertexAttribArray(normalShaderAttribute);
		glVertexAttribPointer(normalShaderAttribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}
	if (texelShaderAttribute >= 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_texelsVBO);
		glEnableVertexAttribArray(texelShaderAttribute);
		glVertexAttribPointer(texelShaderAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshObject::CreateMeshFromObjFile(const std::string& filepath,
	GLint positionShaderAttribute,
	GLint normalShaderAttribute,
	GLint texelShaderAttribute)
{
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> texels;

	Utils::LoadObjFile(filepath, vertices, normals, texels);
	CreateVerticesVBO(vertices);
	CreateNormalsVBO(normals);
	CreateTexelsVBO(texels);
	CreateMeshVAO(positionShaderAttribute, normalShaderAttribute, texelShaderAttribute);
	m_arraySize = vertices.size() / 3;
}

void MeshObject::ResetAll()
{
	ResetTransformations();
	m_meshVAO = 0;
	m_verticesVBO = 0;
	m_normalsVBO = 0;
	m_texelsVBO = 0;
	m_arraySize = 0;
}

void MeshObject::DestroyAll()
{
	if (m_verticesVBO != 0) {
		glDeleteBuffers(1, &m_verticesVBO);
	}
	if (m_normalsVBO != 0) {
		glDeleteBuffers(1, &m_normalsVBO);
	}
	if (m_texelsVBO != 0) {
		glDeleteBuffers(1, &m_texelsVBO);
	}
	if (m_meshVAO != 0) {
		glDeleteVertexArrays(1, &m_meshVAO);
	}
	ResetAll();
}

void MeshObject::Draw(const Camera& camera,
	const SurfaceMaterial& surfaceMaterial,
	const MatrixShaderUniforms& matrixUniforms,
	const MaterialShaderUniforms& materialUniforms) const
{
	auto&& pvmMatrix = camera.GetMatrix() * m_modelMatrix;

	glUniformMatrix4fv(matrixUniforms.pvmMatrixUniform, 1, GL_FALSE, glm::value_ptr(pvmMatrix));
	glUniformMatrix3fv(matrixUniforms.normalMatrixUniform, 1, GL_FALSE, glm::value_ptr(glm::mat3(GetNormalMatrix())));
	glUniformMatrix4fv(matrixUniforms.modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));

	glUniform3fv(materialUniforms.ambientColorUniform, 1, glm::value_ptr(surfaceMaterial.ambientColor));
	glUniform3fv(materialUniforms.diffuseColorUniform, 1, glm::value_ptr(surfaceMaterial.diffuseColor));
	glUniform3fv(materialUniforms.specularColorUniform, 1, glm::value_ptr(surfaceMaterial.specularColor));
	glUniform1f(materialUniforms.shininessUniform, surfaceMaterial.shininess);

	glBindVertexArray(m_meshVAO);
	glDrawArrays(GL_TRIANGLES, 0, m_arraySize);
	glBindVertexArray(0);
}
