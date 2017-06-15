#ifndef MODEL_OBJECT_H
#define MODEL_OBJECT_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Abstract model class for object transformations 
class ModelObject {
protected:

	glm::mat4 m_modelMatrix;

	ModelObject() { ResetTransformations(); }

public:

	virtual ~ModelObject() {}

	inline ModelObject& Translate(const glm::vec3& vec) {
		m_modelMatrix = glm::translate(m_modelMatrix, vec);
		return *this;
	}

	inline ModelObject& Rotate(float angle, const glm::vec3& vec) {
		m_modelMatrix = glm::rotate(m_modelMatrix, angle, vec);
		return *this;
	}

	inline ModelObject& Scale(const glm::vec3& vec) {
		m_modelMatrix = glm::scale(m_modelMatrix, vec);
		return *this;
	}

	inline ModelObject& ApplyTransformations(const glm::mat4& transformationMat) {
		m_modelMatrix = transformationMat * m_modelMatrix;
		return *this;
	}

	inline ModelObject& ResetTransformations() {
		m_modelMatrix = glm::mat4(1.f);
		return *this;
	}

	inline ModelObject& Translate(float x, float y, float z) { return Translate(glm::vec3(x, y, z)); }
	inline ModelObject& Rotate(float angle, float x, float y, float z) { return Rotate(angle, glm::vec3(x, y, z)); }
	inline ModelObject& Scale(float sx, float sy, float sz) { return Scale(glm::vec3(sx, sy, sz)); }

	inline const glm::mat4& GetModelMatrix() const { return m_modelMatrix; }
	inline glm::mat4 GetNormalMatrix() const { return glm::inverse(glm::transpose(glm::mat4(m_modelMatrix))); }
};

#endif