#include "Utils.h"

#include <fstream>
#include <array>
#include <glm/glm.hpp>

#ifndef _UNICODE
#define _UNICODE
#include <IL/il.h>
#undef _UNICODE
#endif

#pragma comment(lib, "DevIL.lib")

namespace {
	const auto VERTEX_RESERVE_COUNT = 1024u;
}

void Utils::LoadObjFile(const std::string& filepath,
	std::vector<GLfloat>& verticesArray,
	std::vector<GLfloat>& normalsArray,
	std::vector<GLfloat>& texelsArray)
{
	std::fstream file(filepath, std::fstream::in);

	if (!file.good()) {
		throw std::runtime_error("Unable to open .obj file: " + filepath);
	}

	struct Face {
		GLuint vertexIndex;
		GLuint texelIndex;
		GLuint normalIndex;
	};

	typedef glm::vec3 Vertex;
	typedef glm::vec2 Texel;
	typedef std::array<Face, 3> Triangle;

	std::vector<Vertex> vertices;
	std::vector<Vertex> normals;
	std::vector<Texel> texels;
	std::vector<Triangle> triangles;
	
	vertices.reserve(VERTEX_RESERVE_COUNT);
	normals.reserve(VERTEX_RESERVE_COUNT);
	texels.reserve(VERTEX_RESERVE_COUNT);
	triangles.reserve(VERTEX_RESERVE_COUNT);

	// load file
	while (!file.fail()) {
		std::string type;
		file >> type;

		if (file.fail()) {
			break;
		}

		if (type == "v") {
			Vertex vertex;
			file >> vertex.x >> vertex.y >> vertex.z;
			vertices.push_back(vertex);
		}
		else if (type == "vn") {
			Vertex vertex;
			file >> vertex.x >> vertex.y >> vertex.z;
			normals.push_back(vertex);
		}
		else if (type == "vt") {
			Texel texel;
			file >> texel.x >> texel.y;
			texels.push_back(texel);
		}
		else if (type == "f") {
			triangles.emplace_back();
			auto&& back = triangles.back();

			GLuint vertexIndex, texelIndex, normalIndex;
			char separator;

			for (auto&& face : back) {
				file >> vertexIndex >> separator >> texelIndex >> separator >> normalIndex;

				face.vertexIndex = vertexIndex - 1;
				face.normalIndex = normalIndex - 1;
				face.texelIndex = texelIndex - 1;
			}
		}

		// ignore rest
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	file.close();

	vertices.shrink_to_fit();
	normals.shrink_to_fit();
	texels.shrink_to_fit();
	triangles.shrink_to_fit();

	// parse loaded data into something readable in OpenGL
	verticesArray.clear();
	verticesArray.reserve(triangles.size() * 3 * 3);
	normalsArray.clear();
	normalsArray.reserve(triangles.size() * 3 * 3);
	texelsArray.clear();
	texelsArray.reserve(triangles.size() * 3 * 2);

	for (const auto& triangle : triangles) {
		for (const auto& face : triangle) {
			const auto& vertex = vertices[face.vertexIndex];
			const auto& normal = normals[face.normalIndex];
			const auto& texel = texels[face.texelIndex];

			// vertex
			verticesArray.push_back(vertex.x);
			verticesArray.push_back(vertex.y);
			verticesArray.push_back(vertex.z);

			// normal
			normalsArray.push_back(normal.x);
			normalsArray.push_back(normal.y);
			normalsArray.push_back(normal.z);

			// texel 
			texelsArray.push_back(texel.s);
			texelsArray.push_back(texel.t);
		}
	}

	verticesArray.shrink_to_fit();
	normalsArray.shrink_to_fit();
	texelsArray.shrink_to_fit();
}

void Utils::InitTextureLoader()
{
	ilInit();
}

GLuint Utils::LoadTexture(const std::string& filepath)
{
	auto image = ilGenImage();

	if (image == 0) {
		throw std::runtime_error("Unable to generate image: " + filepath);
	}

	ilBindImage(image);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	static auto freeContent = [&]() {
		ilBindImage(0);
		ilDeleteImage(image);
	};
	
	std::wstring wfilepath(filepath.length(), L' ');
	std::copy(filepath.begin(), filepath.end(), wfilepath.begin());

	if (ilLoadImage(wfilepath.c_str()) == IL_FALSE) {
		freeContent();
		throw std::runtime_error("Unable to load image: " + filepath);
	}

	auto imageW = ilGetInteger(IL_IMAGE_WIDTH);
	auto imageH = ilGetInteger(IL_IMAGE_HEIGHT);
	auto imageFormat = ilGetInteger(IL_IMAGE_FORMAT);
	auto pixelDataType = ilGetInteger(IL_IMAGE_TYPE);
	GLint textureFormat;

	if (imageFormat == IL_RGB) {
		textureFormat = GL_RGB;
	}
	else if (imageFormat == IL_RGBA) {
		textureFormat = GL_RGBA;
	}
	else {
		// screw other formats
		freeContent();
		throw std::runtime_error("Unsupported image format: " + filepath);
	}

	GLuint texture;
	glGenTextures(1, &texture);

	if (texture == 0) {
		freeContent();
		throw std::runtime_error("Unable to generate texture: " + filepath);
	}

	glBindTexture(GL_TEXTURE_2D, texture);

	// FMI: https://www.khronos.org/opengl/wiki/Pixel_Transfer#Pixel_layout
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// ADD glTexParameter*
	glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, imageW, imageH, 0, static_cast<GLenum>(textureFormat),
		static_cast<GLenum>(pixelDataType), reinterpret_cast<const void*>(ilGetData()));

	glBindTexture(GL_TEXTURE_2D, 0);

	freeContent();
	return texture;
}
