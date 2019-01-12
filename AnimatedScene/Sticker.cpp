#include "Sticker.h"

#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <array>

namespace {
	std::array<SurfaceMaterial, 6> stickerMaterials = {
		// White
		SurfaceMaterial(
			glm::vec3(.05f, .05f, .05f),
			glm::vec3(.5f, .5f, .5f),
			glm::vec3(.7f, .7f, .7f), 4),

		// Red
		SurfaceMaterial(
			glm::vec3(.05f, .0f, 0.f),
			glm::vec3(.5f, .4f,	.4f),
			glm::vec3(.7f, .04f, .04f),	4),

		// Blue
		SurfaceMaterial(
			glm::vec3(.0f, .0f, .05f),
			glm::vec3(.4f, .4f,	.5f),
			glm::vec3(.04f, .04f, .7f),	4),
		
		// Orange
		SurfaceMaterial(
			glm::vec3(.05f, .02f, 0.f),
			glm::vec3(.5f, .25f, .0f),
			glm::vec3(.7f, .35f, .04f),	4),
		
		// Green
		SurfaceMaterial(
			glm::vec3(.0f, .05f, .0f),
			glm::vec3(.4f, .5f,	.4f),
			glm::vec3(.04f, .7f, .04f),	4),
		
		// Yellow
		SurfaceMaterial(
			glm::vec3(.05f, .05f, 0.f),
			glm::vec3(.5f, .5f,	.4f),
			glm::vec3(.7f, .7f, .04f), 4),
	};
}

const SurfaceMaterial& Sticker::GetStickerMaterial(Sticker::Color color)
{
	return stickerMaterials[static_cast<unsigned int>(color)];
}

Sticker::Sticker(GLint positionShaderAttribute, GLint normalShaderAttribute)
{
	ResetAll();
	CreateMesh(positionShaderAttribute, normalShaderAttribute);
}

Sticker::~Sticker()
{
	DestroyAll();
}

Sticker::Sticker(Sticker&& s)
{
	*this = std::move(s);
}

Sticker& Sticker::operator=(Sticker&& s)
{
	DestroyAll();
	m_verticesAndNormalsVBO = s.m_verticesAndNormalsVBO;
	m_verticesAndNormalsCount = s.m_verticesAndNormalsCount;
	m_stickerVAO = s.m_stickerVAO;
	s.ResetAll();
	return *this;
}

void Sticker::ResetAll()
{
	m_stickerVAO = 0;
	m_verticesAndNormalsVBO = 0;
	m_verticesAndNormalsCount = 0;
}

void Sticker::DestroyAll()
{
	if (m_verticesAndNormalsVBO > 0) {
		glDeleteBuffers(1, &m_verticesAndNormalsVBO);
	}
	if (m_stickerVAO > 0) {
		glDeleteVertexArrays(1, &m_stickerVAO);
	}
	ResetAll();
}

void Sticker::CreateMesh(GLint positionShaderAttribute, GLint normalShaderAttribute)
{
	// VBO
	glGenBuffers(1, &m_verticesAndNormalsVBO);
	
	if (m_verticesAndNormalsVBO == 0) {
		DestroyAll();
		throw std::runtime_error("Unable to create sticker vbo");
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_verticesAndNormalsVBO);

	auto s = StickerSize() / 2.f;

	float vertices[] = {
		s, s, s, 0.f, 1.f, 0.f,
		-s, s, s, 0.f, 1.f, 0.f,
		-s, s, -s, 0.f, 1.f, 0.f,
		s, s, -s, 0.f, 1.f, 0.f
	};

	m_verticesAndNormalsCount = sizeof(vertices) / sizeof(*vertices);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// VAO
	glGenVertexArrays(1, &m_stickerVAO);

	if (m_stickerVAO == 0) {
		throw std::runtime_error("Unable to create sticker VAO");
	}
	glBindVertexArray(m_stickerVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_verticesAndNormalsVBO);

	// Bind shader attributes to VBO data
	if (positionShaderAttribute >= 0) {
		glEnableVertexAttribArray(positionShaderAttribute);
		glVertexAttribPointer(positionShaderAttribute, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 6, nullptr);
	}
	if (normalShaderAttribute >= 0) {
		glEnableVertexAttribArray(normalShaderAttribute);
		glVertexAttribPointer(normalShaderAttribute, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 6,
			reinterpret_cast<const void*>(sizeof(float) * 3));
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_verticesAndNormalsVBO);
}

void Sticker::Draw(const Camera& camera, 
	const glm::mat4& modelMatrix,
	const SurfaceMaterial& surfaceMaterial,
	const MatrixShaderUniforms& matrixUniforms,
	const MaterialShaderUniforms& materialUniforms) const
{
	auto pvmMatrix = camera.GetMatrix() * modelMatrix;
	auto normalMatrix = glm::mat3(glm::inverse(glm::transpose(modelMatrix)));

	glUniformMatrix4fv(matrixUniforms.pvmMatrixUniform, 1, GL_FALSE, glm::value_ptr(pvmMatrix));
	glUniformMatrix3fv(matrixUniforms.normalMatrixUniform, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(matrixUniforms.modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glUniform3fv(materialUniforms.ambientColorUniform, 1, glm::value_ptr(surfaceMaterial.ambientColor));
	glUniform3fv(materialUniforms.diffuseColorUniform, 1, glm::value_ptr(surfaceMaterial.diffuseColor));
	glUniform3fv(materialUniforms.specularColorUniform, 1, glm::value_ptr(surfaceMaterial.specularColor));
	glUniform1f(materialUniforms.shininessUniform, surfaceMaterial.shininess);

	glBindVertexArray(m_stickerVAO);
	glDrawArrays(GL_QUADS, 0, m_verticesAndNormalsCount);
	glBindVertexArray(0);
}
