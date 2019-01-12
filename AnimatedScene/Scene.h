#ifndef SCENE_H
#define SCENE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <memory>
#include "Camera.h"
#include "RubikCube.h"
#include "MeshObject.h"
#include "ShaderProgram.h"
#include "MaterialShaderUniforms.h"
#include "SurfaceMaterial.h"
#include "MaterialShaderUniforms.h"
#include "Texture.h"
#include "LightContainer.h"
#include "Mirror.h"

class Scene final {
private:
	
	static constexpr float ROOM_WIDTH = 50.f;
	static constexpr float ROOM_HEIGHT = 17.f;
	static constexpr float ROOM_LENGTH = 30.f;

	// In-Scene objects (ugly solution)
	std::unique_ptr<RubikCube> m_rubikCube;
	std::unique_ptr<MeshObject> m_wallMesh;
	std::unique_ptr<MeshObject> m_binMesh;
	std::unique_ptr<MeshObject> m_boxMesh;
	std::unique_ptr<MeshObject> m_chairMesh;
	std::unique_ptr<MeshObject> m_tableMesh;
	std::unique_ptr<MeshObject> m_shelvesWithMiniTableMesh;
	std::unique_ptr<MeshObject> m_doorMesh;
	std::unique_ptr<MeshObject> m_sphereMesh;
	std::unique_ptr<MeshObject> m_cubeMesh;
	std::unique_ptr<MeshObject> m_notebookMesh;
	std::unique_ptr<MeshObject> m_notebookDisplayMesh;
	std::unique_ptr<MeshObject> m_clockMesh;
	std::unique_ptr<MeshObject> m_clockHandMesh;
	std::unique_ptr<MeshObject> m_lampMesh;
	std::unique_ptr<MeshObject> m_bulbMesh;

	// In-Scene textures (ugly solution)
	std::unique_ptr<Texture> m_binTexture;
	std::unique_ptr<Texture> m_birchwoodTexture;
	std::unique_ptr<Texture> m_doorwoodTexture;
	std::unique_ptr<Texture> m_metalTexture;
	std::unique_ptr<Texture> m_boxTexture;
	std::unique_ptr<Texture> m_wallTexture;
	std::unique_ptr<Texture> m_notebookDisplayContentTexture;

	// Our shader and it's variables
	std::unique_ptr<ShaderProgram> m_shader;

	MaterialShaderUniforms m_materialUniforms;
	MatrixShaderUniforms m_matrixUniforms;

	GLint m_positionAttribute;
	GLint m_normalAttribute;
	GLint m_texelAttribute;
	GLint m_eyePositionUniform;
	GLint m_textureSamplerUniform;
	GLint m_textureTypeUniform;
	
	std::unique_ptr<Mirror> m_mirror;
	std::unique_ptr<LightContainer> m_lightContainer;

	// Two point and two spot lights are used in this scene
	std::array<glm::vec3, 2> m_pointLightsPositions;
	std::array<glm::vec3, 2> m_spotLightsPositions;

	// Levitating Rubik's Cube animation parameters
	float m_rubikCubeDirection;
	float m_rubikCubeHeightOffset;
	float m_rubikCubeAngle;

	// Bouncing balls animation parameters
	float m_bouncingBallHeightOffset;
	float m_bouncingBallDirection;
	float m_bouncingBallVelocity;
	float m_bouncingBallBounceScale;

	// Which texture will be used during texture mapping?
	enum TextureTypeFragmentShader {
		LOADED_GL_TEXTURE = 1,
		PROCEDURAL_WOOD_TEXTURE,
		PROCEDURAL_BRICKS_TEXTURE,
		PROCEDURAL_CARPET_TEXTURE,
		NO_TEXTURE
	};

	void SetTextureTypeFragmentShader(TextureTypeFragmentShader type) const;

	// Initialization
	void ResetAll();
	void InitAttribsAndUniforms();
	void InitSceneObjects();
	void InitSceneTextures();
	void CreateLightContainerAndLights();

	void UpdateLevitatingRubikCube(float deltaTime);
	void UpdateBouncingBall(float deltaTime);

	// Draw methods (ugly solution)
	void DrawRoom(const Camera& camera) const;
	void DrawClock(const Camera& camera) const;
	void DrawShelvesWithMiniTable(const Camera& camera) const;
	void DrawLaptop(const Camera& camera) const;
	void DrawBin(const Camera& camera) const;
	void DrawDoor(const Camera& camera) const;
	void DrawTable(const Camera& camera) const;
	void DrawChairs(const Camera& camera) const;
	void DrawBoxes(const Camera& camera) const;
	void DrawBulb(const Camera& camera, const glm::vec3& bulbPosition) const;
	void DrawLamp(const Camera& camera, const glm::vec3& lampPosition) const;
	void DrawLevitatingRubikCube(const Camera& camera) const;
	void DrawBouncingBalls(const Camera& camera) const;
	void DrawMirror(const Camera& camera) const;

	void DrawSceneWithoutMirror(const Camera& camera) const;

public:

	Scene();
	~Scene();

	// Copy constructor not allowed due to unique_ptrs
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	Scene(Scene&& scene);
	Scene& operator=(Scene&& scene);

	void Update(float deltaTime);
	void Draw(const Camera& camera) const;
};

#endif