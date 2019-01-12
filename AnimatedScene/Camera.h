#ifndef CAMERA_H
#define CAMERA_H

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/freeglut.h>

// Simple camera with "arcball" mechanism
// Quaternions are overkill for this project since it has no complex animations
class Camera {
private:

	static constexpr float ZOOM = 1.1f;
	static constexpr float ROTATION_Q = 0.005f;
	static constexpr float MAX_RADIUS = 20.f;
	static constexpr float MIN_RADIUS = 1.f;

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

	float GetWindowWidth() const { return m_windowWidth; }
	float GetWindowHeight() const { return m_windowHeight; }

	const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }
	const glm::vec3& GetEyePosition() const { return m_eyePosition; }
	glm::mat4 GetMatrix() const { return m_projectionMatrix * GetViewMatrix(); }
	glm::mat4 GetViewMatrix() const { return glm::lookAt(m_eyePosition, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f)); }

	// Set your own eye position for camera
	// Beware of using camera's transformation methods, they will reset your eye position completely
	void SetEyePositionManual(const glm::vec3& eyePosition) { m_eyePosition = eyePosition; }

	void Resize(float windowWidth, float windowHeight);
	
	Camera& Rotate(float dX, float dY);
	Camera& ZoomIn();
	Camera& ZoomOut();
	Camera& Reset();
};

#endif