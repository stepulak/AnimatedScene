#include "Camera.h"

#include <stdexcept>

Camera::Camera(float windowWidth, float windowHeight)
{
	Resize(windowWidth, windowHeight);
}

void Camera::RecalculateEyePosition()
{
	auto cy = cosf(m_yAngle);
	auto cx = cosf(m_xAngle);
	auto sy = sinf(m_yAngle);
	auto sx = sinf(m_xAngle);

	m_eyePosition.x = m_radius * cy * -sx;
	m_eyePosition.y = m_radius * sy;
	m_eyePosition.z = m_radius * cx * cy;
}

void Camera::Resize(float windowWidth, float windowHeight)
{
	if (windowWidth <= 0) {
		throw std::runtime_error("Window width must be positive");
	}
	if (windowHeight <= 0) {
		throw std::runtime_error("Window height must be positive");
	}
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
	m_projectionMatrix = glm::perspective(glm::radians(45.f), windowWidth / windowHeight, 1.f, 100.f);
	Reset();
}

Camera& Camera::Rotate(float dX, float dY)
{
	m_xAngle += dX * ROTATION_Q;
	m_yAngle += dY * ROTATION_Q;

	auto angleLimit = glm::half_pi<float>() - 0.1f;

	if (fabsf(m_yAngle) > angleLimit) {
		m_yAngle = angleLimit * (m_yAngle < 0 ? -1.f : 1.f);
	}

	RecalculateEyePosition();
	return *this;
}

Camera& Camera::ZoomIn()
{
	m_radius -= ZOOM;
	if (m_radius < MIN_RADIUS) {
		m_radius = MIN_RADIUS;
	}
	RecalculateEyePosition();
	return *this;
}

Camera& Camera::ZoomOut()
{
	m_radius += ZOOM;
	if (m_radius > MAX_RADIUS) {
		m_radius = MAX_RADIUS;
	}
	RecalculateEyePosition();
	return *this;
}

Camera& Camera::Reset()
{
	m_radius = 6.f;
	m_xAngle = m_yAngle = glm::quarter_pi<float>();
	RecalculateEyePosition();
	return *this;
}

