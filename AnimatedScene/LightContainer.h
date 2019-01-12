#ifndef LIGHT_CONTAINER_H
#define LIGHT_CONTAINER_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>

#include "PointLight.h"
#include "SpotLight.h"

// Simple container for point and spot lights
class LightContainer final {
private:

	std::vector<PointLight> m_pointLights;
	std::vector<SpotLight> m_spotLights;

	GLuint m_pointLightsUBO;
	GLuint m_pointLightsBlockBinding;
	GLint m_numPointLightsUniform;

	GLuint m_spotLightsUBO;
	GLuint m_spotLightsBlockBinding;
	GLint m_numSpotLightsUniform;

	// Reset all members to initial values
	void ResetAll();

	// Destroy and free all data
	void DestroyAll();

	void SetupPointLights(GLuint pointLightsBlockBinding, GLint numPointLightsUniform);
	void SetupSpotLights(GLuint spotLightsBlockBinding, GLint numSpotLightsUniform);

public:

	static constexpr unsigned int MAX_LIGHTS = 5u;

	LightContainer(GLuint pointLightsBlockBinding, GLint numPointLightsUniform,
		GLuint spotLightsBlockBinding, GLint numSpotLightsUniform);

	~LightContainer();

	LightContainer(const LightContainer&) = delete;
	LightContainer& operator=(const LightContainer&) = delete;

	LightContainer(LightContainer&& lightContainer);
	LightContainer& operator=(LightContainer&& lightContainer);

	unsigned int GetNumberOfPointLights() const { return m_pointLights.size(); }
	unsigned int GetNumberOfSpotLights() const { return m_spotLights.size(); }

	// May throw an exception if number of point lights reaches MAX_LIGHTS
	void AddPointLight(const PointLight& light);

	// May throw an exception if number of spot lights reaches MAX_LIGHTS
	void AddSpotLight(const SpotLight& light);

	// Should be called before shader attachment
	void SendDataIntoGPU() const;

	// Must be called after shader attachment!
	void SendDataIntoShader() const;
};

#endif