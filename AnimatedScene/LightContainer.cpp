#include "LightContainer.h"

LightContainer::LightContainer(GLuint pointLightsBlockBinding, GLint numPointLightsUniform,
	GLuint spotLightsBlockBinding, GLint numSpotLightsUniform)
{
	ResetAll();
	SetupPointLights(pointLightsBlockBinding, numPointLightsUniform);
	SetupSpotLights(spotLightsBlockBinding, numSpotLightsUniform);
}

LightContainer::~LightContainer()
{
	DestroyAll();
}

LightContainer::LightContainer(LightContainer&& lightContainer)
{
	*this = std::move(lightContainer);
}

LightContainer& LightContainer::operator=(LightContainer&& lightContainer)
{
	DestroyAll();
	
	// Point lights
	m_pointLights = std::move(lightContainer.m_pointLights);
	m_pointLightsUBO = lightContainer.m_pointLightsUBO;
	m_numPointLightsUniform = lightContainer.m_numPointLightsUniform;
	m_pointLightsBlockBinding = lightContainer.m_pointLightsBlockBinding;

	// Spotlights
	m_spotLights = std::move(lightContainer.m_spotLights);
	m_spotLightsUBO = lightContainer.m_spotLightsUBO;
	m_numSpotLightsUniform = lightContainer.m_numSpotLightsUniform;
	m_spotLightsBlockBinding = lightContainer.m_spotLightsBlockBinding;

	lightContainer.ResetAll();
	return *this;
}

void LightContainer::ResetAll()
{
	m_pointLights.clear();
	m_spotLights.clear();
	m_pointLightsUBO = 0;
	m_numPointLightsUniform = -1;
	m_pointLightsBlockBinding = -1;
	m_spotLightsUBO = 0;
	m_numSpotLightsUniform = -1;
	m_spotLightsBlockBinding = -1;
}

void LightContainer::DestroyAll()
{
	if (m_pointLightsUBO != 0) {
		glDeleteBuffers(1, &m_pointLightsUBO);
	}
}

void LightContainer::SetupPointLights(GLuint pointLightsBlockBinding, GLint numPointLightsUniform)
{
	m_pointLightsBlockBinding = pointLightsBlockBinding;
	m_numPointLightsUniform = numPointLightsUniform;
	m_pointLights.reserve(MAX_LIGHTS);

	glGenBuffers(1, &m_pointLightsUBO);

	if (m_pointLightsUBO == 0) {
		DestroyAll();
		throw std::runtime_error("Unable to create point lights UBO");
	}

	glBindBuffer(GL_UNIFORM_BUFFER, m_pointLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLight) * m_pointLights.capacity(),
		static_cast<const void*>(m_pointLights.data()), GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightContainer::SetupSpotLights(GLuint spotLightsBlockBinding, GLint numSpotLightsUniform)
{
	m_spotLightsBlockBinding = spotLightsBlockBinding;
	m_numSpotLightsUniform = numSpotLightsUniform;
	m_spotLights.reserve(MAX_LIGHTS);

	glGenBuffers(1, &m_spotLightsUBO);

	if (m_spotLightsUBO == 0) {
		DestroyAll();
		throw std::runtime_error("Unable to create spot lights UBO");
	}

	glBindBuffer(GL_UNIFORM_BUFFER, m_spotLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLight) * m_spotLights.capacity(),
		static_cast<const void*>(m_spotLights.data()), GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightContainer::AddPointLight(const PointLight& light)
{
	if (m_pointLights.size() == MAX_LIGHTS) {
		throw std::runtime_error("Unable to add more point lights. Reached maximum.");
	}
	m_pointLights.push_back(light);
}

void LightContainer::AddSpotLight(const SpotLight& light)
{
	if (m_spotLights.size() == MAX_LIGHTS) {
		throw std::runtime_error("Unable to add more spot lights. Reached maximum.");
	}
	m_spotLights.push_back(light);
}

void LightContainer::SendDataIntoGPU() const
{
	// Point lights
	glBindBuffer(GL_UNIFORM_BUFFER, m_pointLightsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLight) * m_pointLights.size(),
		static_cast<const void*>(m_pointLights.data()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, m_pointLightsBlockBinding, m_pointLightsUBO);

	// Spotlights
	glBindBuffer(GL_UNIFORM_BUFFER, m_spotLightsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SpotLight) * m_spotLights.size(),
		static_cast<const void*>(m_spotLights.data()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, m_spotLightsBlockBinding, m_spotLightsUBO);
}

void LightContainer::SendDataIntoShader() const
{
	glUniform1i(m_numPointLightsUniform, m_pointLights.size());
	glUniform1i(m_numSpotLightsUniform, m_spotLights.size());
}
