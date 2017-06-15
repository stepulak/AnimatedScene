#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

struct SpotLight {
	glm::vec4 position;
	glm::vec3 direction;
	float _align1;
	glm::vec3 ambientColor;
	float __align2;
	glm::vec3 diffuseColor;
	float __align3;
	glm::vec3 specularColor;
	float angle;

	SpotLight() {}

	SpotLight(const glm::vec4& position,
		const glm::vec3& direction,
		const glm::vec3& ambientColor, 
		const glm::vec3& diffuseColor,
		const glm::vec3& specularColor,
		float angle)
		: position(position),
		direction(direction),
		ambientColor(ambientColor),
		diffuseColor(diffuseColor),
		specularColor(specularColor),
		angle(angle)
	{}
};

#endif