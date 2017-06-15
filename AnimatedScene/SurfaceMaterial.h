#ifndef SURFACE_MATERIAL_H
#define SURFACE_MATERIAL_H

#include <glm/vec3.hpp>

struct SurfaceMaterial {
	typedef glm::vec3 Rgb;

	Rgb ambientColor;
	Rgb diffuseColor;
	Rgb specularColor;
	float shininess;

	SurfaceMaterial(const Rgb& ambientColor, const Rgb& diffuseColor, const Rgb& specularColor, float shininess)
		: ambientColor(ambientColor),
		diffuseColor(diffuseColor),
		specularColor(specularColor),
		shininess(shininess)
	{}

	SurfaceMaterial() : shininess(0) {}
};
#endif