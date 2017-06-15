#include "UnitCube.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

UnitCube::UnitCube(GLint positionShaderAttribute, GLint normalShaderAttribute)
{
	ResetAll();
	CreateVerticesAndNormalsVBO();
	CreateIndicesVBO();
	CreateCubeVAO(positionShaderAttribute, normalShaderAttribute);
}

UnitCube::~UnitCube()
{
	DestroyAll();
}

UnitCube::UnitCube(UnitCube&& uc)
{
	*this = std::forward<UnitCube>(uc);
}

UnitCube& UnitCube::operator=(UnitCube&& uc)
{
	DestroyAll();
	m_cubeVAO = uc.m_cubeVAO;
	m_indicesVBO = uc.m_indicesVBO;
	m_numIndices = uc.m_numIndices;
	m_verticesAndNormalsVBO = uc.m_verticesAndNormalsVBO;
	uc.ResetAll();
	return *this;
}

void UnitCube::ResetAll()
{
	m_modelMatrix = glm::mat4();
	m_cubeVAO = 0;
	m_indicesVBO = 0;
	m_verticesAndNormalsVBO = 0;
	m_numIndices = 0;
}

void UnitCube::DestroyAll()
{
	if (m_verticesAndNormalsVBO != 0) {
		glDeleteBuffers(1, &m_verticesAndNormalsVBO);
	}
	if (m_indicesVBO != 0) {
		glDeleteBuffers(1, &m_indicesVBO);
	}
	if (m_cubeVAO != 0) {
		glDeleteVertexArrays(1, &m_cubeVAO);
	}
	ResetAll();
}

void UnitCube::CreateVerticesAndNormalsVBO()
{
	glGenBuffers(1, &m_verticesAndNormalsVBO);

	if (m_verticesAndNormalsVBO == 0) {
		DestroyAll();
		throw std::runtime_error("Unable to create vertices vbo");
	}

	auto cs = CubeSize() / 2.f;

	float verticesAndNormals[] = {
		// face + normal
		// top
		cs, cs, cs, 0.f, 1.f, 0.f,
		cs, cs, -cs, 0.f, 1.f, 0.f,
		-cs, cs, -cs, 0.f, 1.f, 0.f,
		-cs, cs, cs, 0.f, 1.f, 0.f,
		// bottom
		cs, -cs, cs, 0.f, -1.f, 0.f,
		cs, -cs, -cs, 0.f, -1.f, 0.f,
		-cs, -cs, -cs, 0.f, -1.f, 0.f,
		-cs, -cs, cs, 0.f, -1.f, 0.f,
		// front
		cs, -cs, cs, 0.f, 0.f, 1.f,
		cs, cs, cs, 0.f, 0.f, 1.f,
		-cs, cs, cs, 0.f, 0.f, 1.f,
		-cs, -cs, cs, 0.f, 0.f, 1.f,
		// back
		cs, -cs, -cs, 0.f, 0.f, -1.f,
		cs, cs, -cs, 0.f, 0.f, -1.f,
		-cs, cs, -cs, 0.f, 0.f, -1.f,
		-cs, -cs, -cs, 0.f, 0.f, -1.f,
		// right
		cs, cs, cs, 1.f, 0.f, 0.f,
		cs, cs, -cs, 1.f, 0.f, 0.f,
		cs, -cs, -cs, 1.f, 0.f, 0.f,
		cs, -cs, cs, 1.f, 0.f, 0.f,
		// left
		-cs, cs, cs, -1.f, 0.f, 0.f,
		-cs, cs, -cs, -1.f, 0.f, 0.f,
		-cs, -cs, -cs, -1.f, 0.f, 0.f,
		-cs, -cs, cs, -1.f, 0.f, 0.f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_verticesAndNormalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAndNormals), verticesAndNormals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void UnitCube::CreateIndicesVBO()
{
	glGenBuffers(1, &m_indicesVBO);
	
	if (m_indicesVBO == 0) {
		DestroyAll();
		throw std::runtime_error("Unable to create indices vbo");
	}

	unsigned int indices[] = {
		0, 1, 2, 3, // TOP FACE
		4, 5, 6, 7, // BOTTOM FACE
		8, 9, 10, 11, // FRONT FACE
		12, 13, 14, 15, // BACK FACE
		16, 17, 18, 19, // RIGHT FACE
		20, 21, 22, 23, // LEFT FACE
	};
	// for (int i = 0; i < 24; i++) indices[i] = i;

	m_numIndices = sizeof(indices) / sizeof(*indices);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void UnitCube::CreateCubeVAO(GLint positionShaderAttribute, GLint normalShaderAttribute)
{
	glGenVertexArrays(1, &m_cubeVAO);

	if (m_cubeVAO == 0) {
		DestroyAll();
		throw std::runtime_error("Unable to create cube vao");
	}

	glBindVertexArray(m_cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_verticesAndNormalsVBO);

	// make sure the vbo data are accessible in shaders
	
	if (positionShaderAttribute >= 0) { // position attribute is used
		glEnableVertexAttribArray(positionShaderAttribute);
		glVertexAttribPointer(positionShaderAttribute, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 6, nullptr);
	}
	if (normalShaderAttribute >= 0) { // normal attribute is used
		glEnableVertexAttribArray(normalShaderAttribute);
		glVertexAttribPointer(normalShaderAttribute, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 6,
			reinterpret_cast<const void*>(sizeof(float) * 3));
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void UnitCube::Draw(const Camera& camera, 
	const MatrixShaderUniforms& matrixUniforms,
	const MaterialShaderUniforms& materialUniforms) const
{
	auto&& pvmMatrix = camera.GetMatrix() * m_modelMatrix;
	
	glUniformMatrix4fv(matrixUniforms.pvmMatrixUniform, 1, GL_FALSE, glm::value_ptr(pvmMatrix));
	glUniformMatrix3fv(matrixUniforms.normalMatrixUniform, 1, GL_FALSE, glm::value_ptr(glm::mat3(GetNormalMatrix())));
	glUniformMatrix4fv(matrixUniforms.modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));

	glUniform3f(materialUniforms.ambientColorUniform, .1f, .1f, .1f);
	glUniform3f(materialUniforms.diffuseColorUniform, .3f, .3f, .3f);
	glUniform3f(materialUniforms.specularColorUniform, .8f, .8f, .8f);
	glUniform1f(materialUniforms.shininessUniform, 32.f);

	glBindVertexArray(m_cubeVAO);
	glDrawElements(GL_QUADS, m_numIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
