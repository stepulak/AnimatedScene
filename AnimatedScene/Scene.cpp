#include "Scene.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

namespace {

	// Scene materials
	enum MaterialTypes {
		BRONZE = 0,
		SILVER,
		WOOD,
		METAL,
		WALL,
		PLASTIC,
		DARK_PLASTIC,
		GLASS,
		END,
	};

	std::array<SurfaceMaterial, END> materials = {
		// Bronze
		SurfaceMaterial(
			glm::vec3(0.2125f, 0.1275f, 0.054f),
			glm::vec3(0.714f, 0.4284f, 0.18144f),
			glm::vec3(0.393548f, 0.271906f, 0.166721f), 0.2f),

		// Silver
		SurfaceMaterial(
			glm::vec3(0.19225f, 0.19225f, 0.19225f),
			glm::vec3(0.50754f, 0.50754f, 0.50754f),
			glm::vec3(0.508273f, 0.508273f, 0.508273f), 1.f),

		// Wood
		SurfaceMaterial(
			glm::vec3(0.4f, 0.4f, 0.4f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			glm::vec3(1.f, 1.f, 1.f), 32.f),

		// Metal
		SurfaceMaterial(
			glm::vec3(0.4f, 0.4f, 0.4f),
			glm::vec3(0.9f, 0.9f, 0.9f),
			glm::vec3(0.974597f, 0.974597f, 0.974597f), 70.f),

		// Wall
		SurfaceMaterial(
			glm::vec3(0.1f, 0.1f, 0.1f),
			glm::vec3(1.f, 1.f, 1.f),
			glm::vec3(1.f, 1.f, 1.f), 2.f),

		// Plastic
		SurfaceMaterial(
			glm::vec3(0.4f, 0.4f, 0.4f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			glm::vec3(.8f, .8f, .8f), 1.f),

		// Dark platic
		SurfaceMaterial(
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.1f, 0.1f, 0.1f),
			glm::vec3(0.1f, 0.1f, 0.1f), 1.f),

		// Glass
		SurfaceMaterial(
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(1.f, 1.f, 1.f),
			glm::vec3(1.f, 1.f, 1.f), 128.f),
	};
}

Scene::Scene()
{
	ResetAll();
	m_shader = std::make_unique<ShaderProgram>("VertexShader.glsl", "FragmentShader.glsl");
	InitAttribsAndUniforms();
	InitSceneObjects();
	InitSceneTextures();
	CreateLightContainerAndLights();
	m_mirror = std::make_unique<Mirror>(300, 300);
}

Scene::~Scene()
{
}

Scene::Scene(Scene&& scene)
{
	*this = std::forward<Scene>(scene);
}

Scene& Scene::operator=(Scene&& scene)
{
	ResetAll();

	m_rubikCube = std::move(scene.m_rubikCube);
	m_wallMesh = std::move(scene.m_wallMesh);
	m_binMesh = std::move(scene.m_binMesh);
	m_boxMesh = std::move(scene.m_boxMesh);
	m_chairMesh = std::move(scene.m_chairMesh);
	m_tableMesh = std::move(scene.m_tableMesh);
	m_shelvesWithMiniTableMesh = std::move(scene.m_shelvesWithMiniTableMesh);
	m_doorMesh = std::move(scene.m_doorMesh);
	m_sphereMesh = std::move(scene.m_sphereMesh);
	m_cubeMesh = std::move(scene.m_cubeMesh);
	m_notebookMesh = std::move(scene.m_notebookMesh);
	m_notebookDisplayMesh = std::move(scene.m_notebookDisplayMesh);
	m_clockMesh = std::move(scene.m_clockMesh);
	m_clockHandMesh = std::move(scene.m_clockHandMesh);
	m_lampMesh = std::move(scene.m_lampMesh);
	m_bulbMesh = std::move(scene.m_bulbMesh);

	m_binTexture = std::move(scene.m_binTexture);
	m_birchwoodTexture = std::move(scene.m_birchwoodTexture);
	m_doorwoodTexture = std::move(scene.m_doorwoodTexture);
	m_metalTexture = std::move(scene.m_metalTexture);
	m_boxTexture = std::move(scene.m_boxTexture);
	m_wallTexture = std::move(scene.m_wallTexture);
	m_notebookDisplayContentTexture = std::move(scene.m_notebookDisplayContentTexture);

	m_shader = std::move(scene.m_shader);

	m_materialUniforms = scene.m_materialUniforms;
	m_matrixUniforms = scene.m_matrixUniforms;

	m_positionAttribute = scene.m_positionAttribute;
	m_normalAttribute = scene.m_normalAttribute;
	m_texelAttribute = scene.m_texelAttribute;
	m_eyePositionUniform = scene.m_eyePositionUniform;
	m_textureSamplerUniform = scene.m_textureSamplerUniform;
	m_textureTypeUniform = scene.m_textureTypeUniform;

	m_mirror = std::move(scene.m_mirror);
	m_lightContainer = std::move(scene.m_lightContainer);

	// Two point and two spot lights are used in this scene
	m_pointLightsPositions = std::move(scene.m_pointLightsPositions);
	m_spotLightsPositions = std::move(scene.m_spotLightsPositions);

	scene.ResetAll();

	return *this;
}

void Scene::ResetAll()
{
	m_rubikCubeDirection = 1.f;
	m_rubikCubeHeightOffset = 0.f;
	m_rubikCubeAngle = 0.f;
	m_bouncingBallDirection = 1.f;
	m_bouncingBallHeightOffset = 0.f;
	m_bouncingBallVelocity = 0.f;
	m_bouncingBallBounceScale = 1.f;
}

void Scene::InitAttribsAndUniforms()
{
	m_positionAttribute = m_shader->GetAttribLocation("position");
	m_normalAttribute = m_shader->GetAttribLocation("normal");
	m_texelAttribute = m_shader->GetAttribLocation("texel");

	m_eyePositionUniform = m_shader->GetUniformLocation("eye_position");
	m_textureSamplerUniform = m_shader->GetUniformLocation("texture_sampler");
	m_textureTypeUniform = m_shader->GetUniformLocation("texture_type");

	// matrices
	m_matrixUniforms.pvmMatrixUniform = m_shader->GetUniformLocation("pvm_matrix");
	m_matrixUniforms.normalMatrixUniform = m_shader->GetUniformLocation("normal_matrix");
	m_matrixUniforms.modelMatrixUniform = m_shader->GetUniformLocation("model_matrix");

	// materials
	m_materialUniforms.ambientColorUniform = m_shader->GetUniformLocation("material_ambient_color");
	m_materialUniforms.diffuseColorUniform = m_shader->GetUniformLocation("material_diffuse_color");
	m_materialUniforms.specularColorUniform = m_shader->GetUniformLocation("material_specular_color");
	m_materialUniforms.shininessUniform = m_shader->GetUniformLocation("material_shininess");
}

// Bad ugly macro functions :-)
#define LOAD_MESH(path) std::make_unique<MeshObject>(path, m_positionAttribute, m_normalAttribute, m_texelAttribute)

void Scene::InitSceneObjects()
{
	m_rubikCube = std::make_unique<RubikCube>(m_positionAttribute, m_normalAttribute, 3);
	
	m_wallMesh = LOAD_MESH("Data/Wall.obj");
	m_binMesh = LOAD_MESH("Data/Bin.obj");
	m_boxMesh = LOAD_MESH("Data/Box.obj");
	m_chairMesh = LOAD_MESH("Data/Chair.obj");
	m_tableMesh = LOAD_MESH("Data/Table.obj");
	m_shelvesWithMiniTableMesh = LOAD_MESH("Data/ShelvesWithMiniTable.obj");
	m_doorMesh = LOAD_MESH("Data/Door.obj");
	m_sphereMesh = LOAD_MESH("Data/Sphere.obj");
	m_cubeMesh = LOAD_MESH("Data/Cube.obj");
	m_notebookMesh = LOAD_MESH("Data/Notebook.obj");
	m_notebookDisplayMesh = LOAD_MESH("Data/NotebookDisplay.obj");
	m_clockMesh = LOAD_MESH("Data/Clock.obj");
	m_clockHandMesh = LOAD_MESH("Data/ClockHand.obj");
	m_lampMesh = LOAD_MESH("Data/Lamp.obj");
	m_bulbMesh = LOAD_MESH("Data/Bulb.obj");
}

void Scene::InitSceneTextures()
{
	m_binTexture = std::make_unique<Texture>("Data/Bin.png");
	m_birchwoodTexture = std::make_unique<Texture>("Data/Birchwood.png");
	m_metalTexture = std::make_unique<Texture>("Data/Metal.png");
	m_boxTexture = std::make_unique<Texture>("Data/Box.png");
	m_wallTexture = std::make_unique<Texture>("Data/Wall.png");
	m_doorwoodTexture = std::make_unique<Texture>("Data/Doorwood.png");
	m_notebookDisplayContentTexture = std::make_unique<Texture>("Data/NotebookDisplayContent.png");
}

void Scene::CreateLightContainerAndLights()
{
	auto pointLightsBlockIndex = m_shader->GetUniformBlockIndex("point_lights_data");
	auto numPointLightsUniform = m_shader->GetUniformLocation("num_point_lights");
	auto spotLightsBlockIndex = m_shader->GetUniformBlockIndex("spot_lights_data");
	auto numSpotLightsUniform = m_shader->GetUniformLocation("num_spot_lights");

	m_shader->UniformBlockBinding(pointLightsBlockIndex, 0);
	m_shader->UniformBlockBinding(spotLightsBlockIndex, 1);

	m_lightContainer = std::make_unique<LightContainer>(0, numPointLightsUniform, 1, numSpotLightsUniform);

	// Point lights
	m_pointLightsPositions[0] = glm::vec3(15.f, ROOM_HEIGHT / 2.f - .5f, 0.f);

	m_lightContainer->AddPointLight(PointLight(
		glm::vec4(m_pointLightsPositions[0], 1.f),
		glm::vec3(0.3f, 0.3f, 0.3f),
		glm::vec3(1.f, 1.f, 1.f),
		glm::vec3(1.f, 1.f, 1.f)));

	m_pointLightsPositions[1] = glm::vec3(-15.f, ROOM_HEIGHT / 2.f - .5f, 0.f);

	m_lightContainer->AddPointLight(PointLight(
		glm::vec4(m_pointLightsPositions[1], 1.f),
		glm::vec3(0.3f, 0.3f, 0.3f),
		glm::vec3(1.f, 1.f, 1.f),
		glm::vec3(1.f, 1.f, 1.f)));

	// Spot lights
	m_spotLightsPositions[0] = glm::vec3(3.f, -3.5f, 12.f);

	m_lightContainer->AddSpotLight(SpotLight(
		glm::vec4(m_spotLightsPositions[0], 1.f),
		glm::vec3(1.f, -1.f, 0.5f),
		glm::vec3(0.3f, 0.3f, 0.3f),
		glm::vec3(1.f, 1.f, 1.f),
		glm::vec3(1.f, 1.f, 1.f),
		glm::quarter_pi<float>() / 2.f));

	m_spotLightsPositions[1] = glm::vec3(4.f, 0.5f, -12.f);

	m_lightContainer->AddSpotLight(SpotLight(
		glm::vec4(m_spotLightsPositions[1], 1.f),
		glm::vec3(1.f, -1.f, 0.5f),
		glm::vec3(0.3f, 0.3f, 0.3f),
		glm::vec3(1.f, 1.f, 1.f),
		glm::vec3(1.f, 1.f, 1.f),
		glm::quarter_pi<float>() / 2.f));
}

void Scene::UpdateLevitatingRubikCube(float deltaTime)
{
	static const float velocity = .5f;
	static const float maxHeightOffset = 2.f;

	m_rubikCubeAngle += deltaTime;

	if (m_rubikCubeAngle >= glm::two_pi<float>()) {
		m_rubikCubeAngle = 0.f;
	}

	m_rubikCubeHeightOffset += deltaTime * velocity * m_rubikCubeDirection;

	if (fabs(m_rubikCubeHeightOffset) > maxHeightOffset) {
		m_rubikCubeHeightOffset = maxHeightOffset * m_rubikCubeDirection;
		m_rubikCubeDirection *= -1.f;
	}

	if (!m_rubikCube->IsRotating()) {
		m_rubikCube->Rotate(static_cast<RubikCube::RotationType>(rand() % 3),
			static_cast<unsigned int>(rand() % 3), rand() % 2 == 1);
	}
}

void Scene::UpdateBouncingBall(float deltaTime)
{
	static constexpr float acceleration = 9.81f;
	static constexpr float bounceScaleMax = 0.7f;
	static constexpr float bounceDistance = 0.5f;
	static constexpr float maxHeightOffset = 7.5f;

	m_bouncingBallHeightOffset += deltaTime * m_bouncingBallVelocity * m_bouncingBallDirection;
	m_bouncingBallVelocity += m_bouncingBallDirection * acceleration * deltaTime;

	// Make sure that the ball will not increase it's jump height everytime when it bounces
	if (m_bouncingBallVelocity > acceleration) {
		m_bouncingBallVelocity = acceleration;
	}

	m_bouncingBallBounceScale = 1.f;

	if (m_bouncingBallHeightOffset > maxHeightOffset) {
		// Bounce effect
		m_bouncingBallBounceScale = (bounceDistance - m_bouncingBallHeightOffset + maxHeightOffset) / (bounceDistance);
		
		if (m_bouncingBallBounceScale < bounceScaleMax) {
			m_bouncingBallBounceScale = bounceScaleMax;
		}

		if (m_bouncingBallHeightOffset - bounceDistance > maxHeightOffset) {
			m_bouncingBallDirection = -1.f;
			m_bouncingBallHeightOffset = maxHeightOffset + bounceDistance;
		}
	}
	else if (m_bouncingBallVelocity < 0.f) {
		m_bouncingBallDirection = 1.f;
		m_bouncingBallVelocity = 0.f;
	}
}

void Scene::Update(float deltaTime)
{
	m_rubikCube->Update(deltaTime);
	UpdateLevitatingRubikCube(deltaTime);
	UpdateBouncingBall(deltaTime);
}

void Scene::SetTextureTypeFragmentShader(TextureTypeFragmentShader type) const
{
	glUniform1i(m_textureTypeUniform, static_cast<GLint>(type));

	if (type == LOADED_GL_TEXTURE) {
		glUniform1i(m_textureSamplerUniform, 0);
		glActiveTexture(GL_TEXTURE0);
	}
}

void Scene::DrawRoom(const Camera& camera) const
{
	// "Room"
	SetTextureTypeFragmentShader(PROCEDURAL_BRICKS_TEXTURE);
	m_cubeMesh->Scale(ROOM_WIDTH, ROOM_HEIGHT, ROOM_LENGTH);
	m_cubeMesh->Draw(camera, materials[WALL], m_matrixUniforms, m_materialUniforms);
	m_cubeMesh->ResetTransformations();

	// Floor
	SetTextureTypeFragmentShader(PROCEDURAL_CARPET_TEXTURE);
	m_wallMesh->Translate(0.f, -ROOM_HEIGHT / 2.f + 0.01f, 0.f);
	m_wallMesh->Scale(ROOM_WIDTH / 2.f, 1.f, ROOM_LENGTH / 2.f);
	m_wallMesh->Draw(camera, materials[WALL], m_matrixUniforms, m_materialUniforms);
	m_wallMesh->ResetTransformations();

	// Ceiling
	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_wallTexture->Bind();
	m_wallMesh->Rotate(glm::pi<float>(), 1.f, 0.f, 0.f);
	m_wallMesh->Translate(0.f, -ROOM_HEIGHT / 2.f + 0.01f, 0.f);
	m_wallMesh->Scale(ROOM_WIDTH / 2.f, 1.f, ROOM_LENGTH / 1.f);
	m_wallMesh->Draw(camera, materials[WALL], m_matrixUniforms, m_materialUniforms);
	m_wallMesh->ResetTransformations();
	m_wallTexture->Unbind();
}

void Scene::DrawClock(const Camera& camera) const
{
	// Clock with actual local time
	SetTextureTypeFragmentShader(NO_TEXTURE);
	m_clockMesh->Rotate(glm::pi<float>(), 0.f, 1.f, 0.f);
	m_clockMesh->Translate(0.f, 4.f, -ROOM_LENGTH / 2.f);
	m_clockMesh->Scale(0.05f, 0.05f, 0.05f);
	m_clockMesh->Draw(camera, materials[PLASTIC], m_matrixUniforms, m_materialUniforms);
	m_clockMesh->ResetTransformations();

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	float hourhandAngle = (glm::two_pi<float>() / 12.f) * static_cast<float>(12 - sysTime.wHour % 12);
	float minutehandAngle = (glm::two_pi<float>() / 60.f) * static_cast<float>(60 - sysTime.wMinute);
	float secondhandAngle = (glm::two_pi<float>() / 60.f) * static_cast<float>(60 - sysTime.wSecond);

	// Hours
	SetTextureTypeFragmentShader(NO_TEXTURE);
	m_clockHandMesh->Rotate(glm::pi<float>(), 0.f, 1.f, 0.f);
	m_clockHandMesh->Translate(0.f, 4.f, -ROOM_LENGTH / 2.f);
	m_clockHandMesh->Rotate(hourhandAngle, 0.f, 0.f, 1.f);
	m_clockHandMesh->Translate(0.f, 0.15f, 0.f);
	m_clockHandMesh->Scale(0.05f, 0.05f, 0.05f);
	m_clockHandMesh->Draw(camera, materials[DARK_PLASTIC], m_matrixUniforms, m_materialUniforms);
	m_clockHandMesh->ResetTransformations();

	// Minutes
	SetTextureTypeFragmentShader(NO_TEXTURE);
	m_clockHandMesh->Rotate(glm::pi<float>(), 0.f, 1.f, 0.f);
	m_clockHandMesh->Translate(0.f, 4.f, -ROOM_LENGTH / 2.f);
	m_clockHandMesh->Rotate(minutehandAngle, 0.f, 0.f, 1.f);
	m_clockHandMesh->Translate(0.f, 0.15f, 0.f);
	m_clockHandMesh->Scale(0.05f, 0.06f, 0.05f);
	m_clockHandMesh->Draw(camera, materials[DARK_PLASTIC], m_matrixUniforms, m_materialUniforms);
	m_clockHandMesh->ResetTransformations();

	// Seconds
	SetTextureTypeFragmentShader(NO_TEXTURE);
	m_clockHandMesh->Rotate(glm::pi<float>(), 0.f, 1.f, 0.f);
	m_clockHandMesh->Translate(0.f, 4.f, -ROOM_LENGTH / 2.f);
	m_clockHandMesh->Rotate(secondhandAngle, 0.f, 0.f, 1.f);
	m_clockHandMesh->Translate(0.f, 0.15f, 0.f);
	m_clockHandMesh->Scale(0.02f, 0.09f, 0.05f);
	m_clockHandMesh->Draw(camera, materials[DARK_PLASTIC], m_matrixUniforms, m_materialUniforms);
	m_clockHandMesh->ResetTransformations();
}

void Scene::DrawShelvesWithMiniTable(const Camera& camera) const
{
	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_birchwoodTexture->Bind();
	m_shelvesWithMiniTableMesh->Translate(4.f, -ROOM_HEIGHT / 2.f, 2.5f);
	m_shelvesWithMiniTableMesh->Scale(4.f, 4.f, 4.f);
	m_shelvesWithMiniTableMesh->Draw(camera, materials[WOOD], m_matrixUniforms, m_materialUniforms);
	m_shelvesWithMiniTableMesh->ResetTransformations();
	m_birchwoodTexture->Unbind();
}

void Scene::DrawLaptop(const Camera& camera) const
{
	// Notebook + Display + Display Content
	SetTextureTypeFragmentShader(NO_TEXTURE);
	m_notebookMesh->Rotate(glm::pi<float>(), 0.f, 1.f, 0.f);
	m_notebookMesh->Translate(0.f, -3.5f, -12.f);
	m_notebookMesh->Scale(0.05f, 0.05f, 0.05f);
	m_notebookMesh->Draw(camera, materials[PLASTIC], m_matrixUniforms, m_materialUniforms);
	m_notebookMesh->ResetTransformations();

	SetTextureTypeFragmentShader(NO_TEXTURE);
	m_notebookDisplayMesh->Rotate(glm::pi<float>(), 0.f, 1.f, 0.f);
	m_notebookDisplayMesh->Translate(0.f, -3.5f, -12.f);
	m_notebookDisplayMesh->Scale(0.05f, 0.05f, 0.05f);
	m_notebookDisplayMesh->Draw(camera, materials[PLASTIC], m_matrixUniforms, m_materialUniforms);
	m_notebookDisplayMesh->ResetTransformations();

	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_notebookDisplayContentTexture->Bind();
	m_wallMesh->Rotate(glm::pi<float>(), 0.f, 1.f, 0.f);
	m_wallMesh->Translate(0.f, -3.5f, -12.f);
	m_wallMesh->Rotate(glm::quarter_pi<float>()* 1.45f, 1.f, 0.f, 0.f);
	m_wallMesh->Translate(0.f, 0.f, -0.62f);
	m_wallMesh->Scale(0.85f, 1.f, .58f);
	m_wallMesh->Draw(camera, materials[PLASTIC], m_matrixUniforms, m_materialUniforms);
	m_wallMesh->ResetTransformations();
	m_notebookDisplayContentTexture->Unbind();
}

void Scene::DrawBin(const Camera& camera) const
{
	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_binTexture->Bind();
	m_binMesh->Translate(ROOM_WIDTH / 2.f - 2.f, -ROOM_HEIGHT / 2.f, -ROOM_LENGTH / 2.f + 2.f);
	m_binMesh->Scale(0.08f, 0.08f, 0.08f);
	m_binMesh->Draw(camera, materials[SILVER], m_matrixUniforms, m_materialUniforms);
	m_binMesh->ResetTransformations();
	m_binTexture->Unbind();
}

void Scene::DrawDoor(const Camera& camera) const
{
	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_doorwoodTexture->Bind();
	m_doorMesh->Translate(ROOM_WIDTH / 2.f, -ROOM_HEIGHT / 2.f, 0.f);
	m_doorMesh->Rotate(glm::half_pi<float>(), 0.f, 1.f, 0.f);
	m_doorMesh->Scale(8.f, 5.f, 4.f);
	m_doorMesh->Draw(camera, materials[WOOD], m_matrixUniforms, m_materialUniforms);
	m_doorMesh->ResetTransformations();
	m_doorwoodTexture->Unbind();
}

void Scene::DrawTable(const Camera& camera) const
{
	SetTextureTypeFragmentShader(PROCEDURAL_WOOD_TEXTURE);
	m_tableMesh->Translate(0.f, -ROOM_HEIGHT / 2.f, ROOM_LENGTH / 2.f - 3.f);
	m_tableMesh->Scale(0.07f, 0.07f, 0.07f);
	m_tableMesh->Draw(camera, materials[WOOD], m_matrixUniforms, m_materialUniforms);
	m_tableMesh->ResetTransformations();
}

void Scene::DrawChairs(const Camera& camera) const
{
	// Chair 1
	SetTextureTypeFragmentShader(PROCEDURAL_WOOD_TEXTURE);
	m_chairMesh->Translate(0.f, -ROOM_HEIGHT / 2.f + 1.5f, 3.f);
	m_chairMesh->Rotate(-glm::half_pi<float>(), 0.f, 1.f, 0.f);
	m_chairMesh->Scale(1.8f, 1.8f, 1.8f);
	m_chairMesh->Draw(camera, materials[WOOD], m_matrixUniforms, m_materialUniforms);
	m_chairMesh->ResetTransformations();

	// Chair 2
	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_doorwoodTexture->Bind();
	m_chairMesh->Translate(-6.f, -ROOM_HEIGHT / 2.f + 1.5f, 5.f);
	m_chairMesh->Rotate(-glm::quarter_pi<float>(), 0.f, 1.f, 0.f);
	m_chairMesh->Scale(1.8f, 1.8f, 1.8f);
	m_chairMesh->Draw(camera, materials[WOOD], m_matrixUniforms, m_materialUniforms);
	m_chairMesh->ResetTransformations();
	m_doorwoodTexture->Unbind();

	// Chair 3
	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_birchwoodTexture->Bind();
	m_chairMesh->Translate(6.f, -ROOM_HEIGHT / 2.f + 1.5f, 5.f);
	m_chairMesh->Rotate(-glm::pi<float>() + glm::quarter_pi<float>(), 0.f, 1.f, 0.f);
	m_chairMesh->Scale(1.8f, 1.8f, 1.8f);
	m_chairMesh->Draw(camera, materials[WOOD], m_matrixUniforms, m_materialUniforms);
	m_chairMesh->ResetTransformations();
	m_birchwoodTexture->Unbind();
}

void Scene::DrawBoxes(const Camera& camera) const
{
	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_boxTexture->Bind();
	m_boxMesh->Translate(-ROOM_WIDTH / 2.f + 3.f, -ROOM_HEIGHT / 2.f, ROOM_LENGTH / 2.f - 3.f);
	m_boxMesh->Rotate(glm::quarter_pi<float>(), 0.f, 1.f, 0.f);
	m_boxMesh->Scale(0.07f, 0.07f, 0.07f);
	m_boxMesh->Draw(camera, materials[WOOD], m_matrixUniforms, m_materialUniforms);
	m_boxMesh->ResetTransformations();
	
	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_boxMesh->Translate(ROOM_WIDTH / 2.f - 3.f, -ROOM_HEIGHT / 2.f, ROOM_LENGTH / 2.f - 3.f);
	m_boxMesh->Rotate(glm::quarter_pi<float>(), 0.f, 1.f, 0.f);
	m_boxMesh->Scale(0.04f, 0.04f, 0.04f);
	m_boxMesh->Draw(camera, materials[WOOD], m_matrixUniforms, m_materialUniforms);
	m_boxMesh->ResetTransformations();
	m_boxTexture->Unbind();

	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_birchwoodTexture->Bind();
	m_boxMesh->Translate(ROOM_WIDTH / 2.f - 3.f, -ROOM_HEIGHT / 2.f + 2.5f, ROOM_LENGTH / 2.f - 3.f);
	m_boxMesh->Rotate(glm::half_pi<float>(), 0.f, 1.f, 0.f);
	m_boxMesh->Scale(0.03f, 0.03f, 0.04f);
	m_boxMesh->Draw(camera, materials[WOOD], m_matrixUniforms, m_materialUniforms);
	m_boxMesh->ResetTransformations();
	m_birchwoodTexture->Unbind();

	SetTextureTypeFragmentShader(PROCEDURAL_WOOD_TEXTURE);
	m_boxMesh->Translate(ROOM_WIDTH / 2.f - 3.f, -ROOM_HEIGHT / 2.f, ROOM_LENGTH / 2.f - 8.f);
	m_boxMesh->Rotate(glm::half_pi<float>() + 1.f, 0.f, 1.f, 0.f);
	m_boxMesh->Scale(0.05f, 0.05f, 0.05f);
	m_boxMesh->Draw(camera, materials[WOOD], m_matrixUniforms, m_materialUniforms);
	m_boxMesh->ResetTransformations();

	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_doorwoodTexture->Bind();
	m_boxMesh->Translate(ROOM_WIDTH / 2.f - 3.f, -ROOM_HEIGHT / 2.f + 3.f, ROOM_LENGTH / 2.f - 7.f);
	m_boxMesh->Rotate(glm::quarter_pi<float>(), 0.f, 1.f, 0.f);
	m_boxMesh->Scale(0.06f, 0.06f, 0.06f);
	m_boxMesh->Draw(camera, materials[WOOD], m_matrixUniforms, m_materialUniforms);
	m_boxMesh->ResetTransformations();
	m_doorwoodTexture->Bind();
}

void Scene::DrawBulb(const Camera& camera, const glm::vec3& bulbPosition) const
{
	SetTextureTypeFragmentShader(NO_TEXTURE);
	m_bulbMesh->Translate(bulbPosition);
	m_bulbMesh->Scale(0.02f, 0.02f, 0.02f);
	m_bulbMesh->Draw(camera, materials[GLASS], m_matrixUniforms, m_materialUniforms);
	m_bulbMesh->ResetTransformations();
}

void Scene::DrawLamp(const Camera& camera, const glm::vec3& lampPosition) const
{
	// Lamp + bulb (which is a little bit rotated and translated for our needs, so we cannot use DrawBulb() method
	SetTextureTypeFragmentShader(NO_TEXTURE);
	m_bulbMesh->Translate(lampPosition);
	m_bulbMesh->Rotate(glm::pi<float>() - .5f, glm::vec3(0.f, 1.f, 0.f));
	m_bulbMesh->Translate(-0.5f, 1.7f, 0.f);
	m_bulbMesh->Rotate(-0.5f, glm::vec3(0.f, 0.f, 1.f));
	m_bulbMesh->Scale(0.02f, 0.02f, 0.02f);
	m_bulbMesh->Draw(camera, materials[GLASS], m_matrixUniforms, m_materialUniforms);
	m_bulbMesh->ResetTransformations();

	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_binTexture->Bind();
	m_lampMesh->Translate(lampPosition);
	m_lampMesh->Rotate(glm::pi<float>() - .5f, glm::vec3(0.f, 1.f, 0.f));
	m_lampMesh->Scale(0.1f, 0.1f, 0.1f);
	m_lampMesh->Draw(camera, materials[BRONZE], m_matrixUniforms, m_materialUniforms);
	m_lampMesh->ResetTransformations();
	m_binTexture->Unbind();
}

void Scene::DrawLevitatingRubikCube(const Camera& camera) const
{
	SetTextureTypeFragmentShader(NO_TEXTURE);
	auto&& translateVec = glm::vec3(-ROOM_WIDTH / 2.f + 3.f, m_rubikCubeHeightOffset, ROOM_LENGTH / 2.f - 3.f);
	auto&& rotateVec = glm::vec3(1.f, 1.f, 1.f);
	auto&& scaleVec = glm::vec3(2.f, 2.f, 2.f);

	auto transformationMat = glm::translate(glm::mat4(1.f), translateVec);
	transformationMat = glm::rotate(transformationMat, m_rubikCubeAngle, rotateVec);
	transformationMat = glm::scale(transformationMat, scaleVec);

	m_rubikCube->SetUserTransformationMatrix(transformationMat);
	m_rubikCube->Draw(camera, m_matrixUniforms, m_materialUniforms);
	m_rubikCube->SetUserTransformationMatrix();
}

void Scene::DrawBouncingBalls(const Camera& camera) const
{
	SetTextureTypeFragmentShader(NO_TEXTURE);
	m_sphereMesh->Translate(-ROOM_WIDTH / 4.f, -m_bouncingBallHeightOffset, -ROOM_LENGTH / 2.f + 2.f);
	m_sphereMesh->Scale(1.f, m_bouncingBallBounceScale, 1.f);
	m_sphereMesh->Draw(camera, materials[BRONZE], m_matrixUniforms, m_materialUniforms);
	m_sphereMesh->ResetTransformations();

	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_binTexture->Bind();
	m_sphereMesh->Translate(-ROOM_WIDTH / 6.f, -m_bouncingBallHeightOffset, -ROOM_LENGTH / 2.f + 2.f);
	m_sphereMesh->Scale(1.f, m_bouncingBallBounceScale, 1.f);
	m_sphereMesh->Draw(camera, materials[BRONZE], m_matrixUniforms, m_materialUniforms);
	m_sphereMesh->ResetTransformations();
	m_binTexture->Bind();

	SetTextureTypeFragmentShader(PROCEDURAL_WOOD_TEXTURE);
	m_binTexture->Bind();
	m_sphereMesh->Translate(-ROOM_WIDTH / 10.f, -m_bouncingBallHeightOffset, -ROOM_LENGTH / 2.f + 2.f);
	m_sphereMesh->Scale(1.f, m_bouncingBallBounceScale, 1.f);
	m_sphereMesh->Draw(camera, materials[WOOD], m_matrixUniforms, m_materialUniforms);
	m_sphereMesh->ResetTransformations();
	m_binTexture->Bind();
}

void Scene::DrawMirror(const Camera& camera) const
{
	SetTextureTypeFragmentShader(LOADED_GL_TEXTURE);
	m_mirror->BindMirrorAsTexture();
	m_wallMesh->Translate(-10.f, 0.f, ROOM_LENGTH / 2.f - 0.5f);
	m_wallMesh->Rotate(glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f));
	m_wallMesh->Scale(5.f, 1.f, 3.f);
	m_wallMesh->Draw(camera, materials[GLASS], m_matrixUniforms, m_materialUniforms);
	m_wallMesh->ResetTransformations();
	m_mirror->UnbindMirrorAsTexture();
}

void Scene::DrawSceneWithoutMirror(const Camera& camera) const
{
	DrawRoom(camera);
	DrawShelvesWithMiniTable(camera);
	DrawBin(camera);
	DrawDoor(camera);
	DrawTable(camera);
	DrawChairs(camera);
	DrawLaptop(camera);
	DrawClock(camera);
	DrawBoxes(camera);
	DrawLamp(camera, m_spotLightsPositions[0]);
	DrawLamp(camera, m_spotLightsPositions[1]);
	DrawBulb(camera, m_pointLightsPositions[0]);
	DrawBulb(camera, m_pointLightsPositions[1]);
	DrawLevitatingRubikCube(camera);
	DrawBouncingBalls(camera);
}

void Scene::Draw(const Camera& camera) const
{	
	m_lightContainer->SendDataIntoGPU();
	m_shader->SetActive();
	m_lightContainer->SendDataIntoShader();
	
	// Send eye position into shader
	glUniform3fv(m_eyePositionUniform, 1, glm::value_ptr(camera.GetEyePosition()));

	// Mirrored scene
	m_mirror->SetActive();
	DrawSceneWithoutMirror(m_mirror->GetReflectedCamera(camera, glm::vec3(1.f, 1.f, -1.f)));
	m_mirror->SetInactive();

	// Normal scene
	DrawSceneWithoutMirror(camera);
	DrawMirror(camera);
	
	m_shader->SetInactive();
}
