#ifndef CAMERA_H
#define CAMERA_H

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/freeglut.h>

// Simple camera with "arcball" mechanism
// Quaternions are overkill for this project since it has no complex animations
class Camera {
private:

	static const float ZOOM;
	static const float ROTATION_Q;
	static const float MAX_RADIUS;
	static const float MIN_RADIUS;

	glm::mat4 m_projectionMatrix;
	glm::vec3 m_eyePosition;
	float m_xAngle;
	float m_yAngle;
	float m_radius;
	float m_windowWidth;
	float m_windowHeight;

	void RecalculateEyePosition();

public:

	Camera(float windowWidth, float windowHeight);
	virtual ~Camera() {}

	inline float GetWindowWidth() const { return m_windowWidth; }
	inline float GetWindowHeight() const { return m_windowHeight; }

	inline const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }
	inline const glm::vec3& GetEyePosition() const { return m_eyePosition; }
	inline glm::mat4 GetMatrix() const { return m_projectionMatrix * GetViewMatrix(); }
	inline glm::mat4 GetViewMatrix() const { return glm::lookAt(m_eyePosition, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f)); }

	// Set your own eye position for camera
	// Beware of using camera's transformation methods, they will reset your eye position completely
	inline void SetEyePositionManual(const glm::vec3& eyePosition) { m_eyePosition = eyePosition; }

	void Resize(float windowWidth, float windowHeight);
	
	Camera& Rotate(float dX, float dY);
	Camera& ZoomIn();
	Camera& ZoomOut();
	Camera& Reset();
};

#endif