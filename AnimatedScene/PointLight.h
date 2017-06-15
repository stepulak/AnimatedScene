#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct PointLight {
	glm::vec4 position;
	glm::vec3 ambientColor;
	float __align1;
	glm::vec3 diffuseColor;
	float __align2;
	glm::vec3 specularColor;
	float __align3;

	PointLight()
	{}

	PointLight(const glm::vec4& position, 
		const glm::vec3& ambientColor,
		const glm::vec3& diffuseColor,
		const glm::vec3& specularColor)
		: position(position),
		ambientColor(ambientColor),
		diffuseColor(diffuseColor),
		specularColor(specularColor)
	{}
};

#endif