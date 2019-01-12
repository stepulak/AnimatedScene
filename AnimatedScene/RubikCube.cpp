#include "RubikCube.h"
#include <glm/gtx/transform.hpp>
#include <fstream>

RubikCube::RubikCube(GLint positionShaderAttribute, GLint normalShaderAttribute, unsigned int numStickersEdge)
{
	ResetAll();
	m_unitCube = std::make_unique<UnitCube>(positionShaderAttribute, normalShaderAttribute);
	m_sticker = std::make_unique<Sticker>(positionShaderAttribute, normalShaderAttribute);
	NewCube(numStickersEdge);
}

RubikCube::~RubikCube()
{
	DestroyAll();
}

RubikCube::RubikCube(RubikCube&& r)
{
	*this = std::move(r);
}

RubikCube& RubikCube::operator=(RubikCube&& r)
{
	DestroyAll();
	m_faces = std::move(r.m_faces);
	m_unitCube.swap(r.m_unitCube);
	m_sticker.swap(r.m_sticker);
	r.ResetAll();
	return *this;
}

void RubikCube::ResetAll()
{
	m_rotationType = NONE;
	m_rotationTimer = 0.f;
	m_rotationIndex = 0;
	m_rotationClockwise = false;
}

void RubikCube::DestroyAll()
{
	m_unitCube.reset();
	m_sticker.reset();
	ResetAll();
}

void RubikCube::FillFaceWithColor(FaceIndex face, Sticker::Color c, unsigned int numStickersEdge)
{
	m_faces[face].reserve(numStickersEdge);

	for (unsigned int x = 0; x < numStickersEdge; x++) {
		m_faces[face].push_back(StickerLine());
		m_faces[face].back().reserve(numStickersEdge);

		for (unsigned int y = 0; y < numStickersEdge; y++) {
			m_faces[face].back().push_back(c);
		}
	}
}

void RubikCube::RotateFace(FaceIndex face, bool clockwise)
{
	auto numStickers = GetNumStickersPerEdge();
	auto& f = m_faces[face];

	for (unsigned int i = 0; i < numStickers / 2; i++) {
		for (unsigned int j = i; j < numStickers - i - 1; j++) {
			auto& secondRow = clockwise ? f[numStickers - i - 1][j] : f[i][numStickers - j - 1];
			auto& fourthRow = clockwise ? f[i][numStickers - j - 1] : f[numStickers - i - 1][j];
			ShiftStickerColors(f[j][i], secondRow, f[numStickers - j - 1][numStickers - i - 1], fourthRow);
		}
	}
}

void RubikCube::ShiftStickerColors(Sticker::Color& c1, Sticker::Color& c2, Sticker::Color& c3, Sticker::Color& c4)
{
	Sticker::Color tmp = c1;
	c1 = c2;
	c2 = c3;
	c3 = c4;
	c4 = tmp;
}

void RubikCube::SwapFacesXAxisRotation()
{
	auto numStickers = GetNumStickersPerEdge();

	if (m_rotationIndex == 0) {
		RotateFace(LEFT, !m_rotationClockwise);
	}
	else if (m_rotationIndex == numStickers - 1) {
		RotateFace(RIGHT, m_rotationClockwise);
	}

	auto i = m_rotationIndex;

	for (auto y = 0u; y < numStickers; y++) {
		auto& second = m_rotationClockwise ? m_faces[BACK][i][y] : m_faces[FRONT][i][y];
		auto& fourth = m_rotationClockwise ? m_faces[FRONT][i][y] : m_faces[BACK][i][y];
		ShiftStickerColors(m_faces[TOP][i][y], second, m_faces[BOTTOM][i][y], fourth);
	}
}

void RubikCube::SwapFacesYAxisRotation()
{
	auto numStickers = GetNumStickersPerEdge();

	if (m_rotationIndex == 0) {
		RotateFace(BOTTOM, !m_rotationClockwise);
	}
	else if (m_rotationIndex == numStickers - 1) {
		RotateFace(TOP, m_rotationClockwise);
	}

	auto i = m_rotationIndex;

	for (auto x = 0u; x < numStickers; x++) {
		auto& left = m_faces[LEFT][i][x];
		auto& right = m_faces[RIGHT][numStickers - i - 1][numStickers - x - 1];
		auto& second = m_rotationClockwise ? left : right;
		auto& fourth = m_rotationClockwise ? right : left;

		ShiftStickerColors(m_faces[FRONT][x][numStickers - i - 1], second, m_faces[BACK][numStickers - x - 1][i], fourth);
	}
}

void RubikCube::SwapFacesZAxisRotation()
{
	auto numStickers = GetNumStickersPerEdge();

	if (m_rotationIndex == 0) {
		RotateFace(BACK, !m_rotationClockwise);
	}
	else if (m_rotationIndex == numStickers - 1) {
		RotateFace(FRONT, m_rotationClockwise);
	}

	auto i = m_rotationIndex;

	for (auto x = 0u; x < numStickers; x++) {
		auto& second = m_rotationClockwise ? m_faces[RIGHT][x][i] : m_faces[LEFT][x][i];
		auto& fourth = m_rotationClockwise ? m_faces[LEFT][x][i] : m_faces[RIGHT][x][i];
		ShiftStickerColors(m_faces[TOP][x][i], second, m_faces[BOTTOM][numStickers - x - 1][numStickers - i - 1], fourth);
	}
}

void RubikCube::DrawFace(FaceIndex face,
	const glm::mat4& rotationMatrix,
	unsigned int startX, unsigned int startY,
	unsigned int endX, unsigned int endY,
	const Camera& camera,
	const MatrixShaderUniforms& matrixUniforms,
	const MaterialShaderUniforms& materialUniforms) const
{
	auto numStickers = GetNumStickersPerEdge();
	
	if (startX < 0 || startY < 0) {
		throw std::runtime_error("DrawFace: start coordinates cannot be negative");
	}
	if (endX > numStickers || endY > numStickers) {
		throw std::runtime_error("DrawFace: end coordinates cannot be larger than cube proportions");
	}
	if (startX >= endX || startY >= endY) {
		return; // no throw
	}

	auto pi = glm::pi<float>();
	glm::mat4 rotationMat;

	switch (face) {
	case TOP:
		// Sticker's default face is top
		break;
	case BOTTOM:
		rotationMat = glm::rotate(pi, glm::vec3(1.f, 0.f, 0.f));
		break;
	case LEFT:
		rotationMat = glm::rotate(pi / 2.f, glm::vec3(0.f, 0.f, 1.f));
		break;
	case RIGHT:
		rotationMat = glm::rotate(-pi / 2.f, glm::vec3(0.f, 0.f, 1.f));
		break;
	case FRONT:
		rotationMat = glm::rotate(pi / 2.f, glm::vec3(1.f, 0.f, 0.f));
		break;
	case BACK:
		rotationMat = glm::rotate(-pi / 2.f, glm::vec3(1.f, 0.f, 0.f));
		break;
	}

	auto stickerSize = GetStickerSize();
	auto scaleMat = glm::scale(glm::vec3(stickerSize*0.9f, 1.f, stickerSize*0.9f));
	
	for (auto x = startX; x < endX; x++) {
		for (auto y = startY; y < endY; y++) {
			auto&& surfaceMaterial = Sticker::GetStickerMaterial(m_faces[face][x][y]);

			float translateX = -stickerSize * numStickers / 2.f + stickerSize / 2.f + x * stickerSize;
			float translateZ = -stickerSize * numStickers / 2.f + stickerSize / 2.f + y * stickerSize;

			auto&& translationMat = glm::translate(glm::vec3(translateX, 0.001f, translateZ));
			auto&& finalTransform = m_userTransformations * rotationMatrix * rotationMat * translationMat * scaleMat;

			m_sticker->Draw(camera, finalTransform, surfaceMaterial, matrixUniforms, materialUniforms);
		}
	}
}

void RubikCube::DrawCubeNoRotation(const Camera& camera,
	const MatrixShaderUniforms& matrixUniforms,
	const MaterialShaderUniforms& materialUniforms) const
{
	m_unitCube->ApplyTransformations(m_userTransformations);
	m_unitCube->Draw(camera, matrixUniforms, materialUniforms);
	m_unitCube->ResetTransformations();

	auto numStickers = GetNumStickersPerEdge();

	for (auto face = 0u; face < m_faces.size(); face++) {
		DrawFace(static_cast<FaceIndex>(face), glm::mat4(1.f), 0, 0,
			numStickers, numStickers, camera, matrixUniforms, materialUniforms);
	}
}

void RubikCube::DrawCubeXAxisRotation(const Camera& camera,
	const MatrixShaderUniforms& matrixUniforms,
	const MaterialShaderUniforms& materialUniforms) const
{
	auto numStickers = GetNumStickersPerEdge();
	glm::vec3 rotationVec(1.f, 0.f, 0.f);
	glm::mat4 identityMat(1.f);
	auto rotationMat = glm::rotate(GetRotationAngle(), rotationVec);

	DrawUnitCubeGenericRotation(camera, rotationVec, matrixUniforms, materialUniforms);

	DrawFace(LEFT, m_rotationIndex == 0 ? rotationMat : identityMat, 0, 0,
		numStickers, numStickers, camera, matrixUniforms, materialUniforms);
	
	DrawFace(RIGHT, (m_rotationIndex == numStickers - 1) ? rotationMat : identityMat, 0, 0,
		numStickers, numStickers, camera, matrixUniforms, materialUniforms);
	
	for (auto face : { TOP, BACK, FRONT, BOTTOM }) {
		auto i = m_rotationIndex;
		DrawFace(face, rotationMat, i, 0, i + 1, numStickers, camera, matrixUniforms, materialUniforms);
		DrawFace(face, identityMat, 0, 0, i, numStickers, camera, matrixUniforms, materialUniforms);
		DrawFace(face, identityMat, i + 1, 0, numStickers, numStickers, camera, matrixUniforms, materialUniforms);
	}
}

void RubikCube::DrawCubeYAxisRotation(const Camera& camera,
	const MatrixShaderUniforms& matrixUniforms,
	const MaterialShaderUniforms& materialUniforms) const
{
	auto numStickers = GetNumStickersPerEdge();
	glm::vec3 rotationVec(0.f, 1.f, 0.f);
	glm::mat4 identityMat(1.f);
	auto rotationMat = glm::rotate(GetRotationAngle(), rotationVec);

	DrawUnitCubeGenericRotation(camera, rotationVec, matrixUniforms, materialUniforms);

	DrawFace(BOTTOM, m_rotationIndex == 0 ? rotationMat : identityMat, 0, 0,
		numStickers, numStickers, camera, matrixUniforms, materialUniforms);

	DrawFace(TOP, (m_rotationIndex == numStickers - 1) ? rotationMat : identityMat, 0, 0,
		numStickers, numStickers, camera, matrixUniforms, materialUniforms);

	// Unlike in rotation around X axis, [0, 0] points of faces aren't in straight line there
	for (auto face : { FRONT, RIGHT, BACK, LEFT }) {
		auto i = (face == FRONT || face == RIGHT) ? numStickers - m_rotationIndex - 1 : m_rotationIndex;

		if (face == FRONT || face == BACK) {
			DrawFace(face, rotationMat, 0, i, numStickers, i + 1, camera, matrixUniforms, materialUniforms);
			DrawFace(face, identityMat, 0, 0, numStickers, i, camera, matrixUniforms, materialUniforms);
			DrawFace(face, identityMat, 0, i + 1, numStickers, numStickers, camera, matrixUniforms, materialUniforms);
		}
		else {
			DrawFace(face, rotationMat, i, 0, i + 1, numStickers, camera, matrixUniforms, materialUniforms);
			DrawFace(face, identityMat, 0, 0, i, numStickers, camera, matrixUniforms, materialUniforms);
			DrawFace(face, identityMat, i + 1, 0, numStickers, numStickers, camera, matrixUniforms, materialUniforms);
		}
	}
}

void RubikCube::DrawCubeZAxisRotation(const Camera& camera,
	const MatrixShaderUniforms& matrixUniforms,
	const MaterialShaderUniforms& materialUniforms) const
{
	auto numStickers = GetNumStickersPerEdge();
	glm::vec3 rotationVec(0.f, 0.f, 1.f);
	glm::mat4 identityMat(1.f);
	auto rotationMat = glm::rotate(GetRotationAngle(), rotationVec);

	DrawUnitCubeGenericRotation(camera, rotationVec, matrixUniforms, materialUniforms);

	DrawFace(BACK, m_rotationIndex == 0 ? rotationMat : identityMat, 0, 0,
		numStickers, numStickers, camera, matrixUniforms, materialUniforms);

	DrawFace(FRONT, (m_rotationIndex == numStickers - 1) ? rotationMat : identityMat, 0, 0,
		numStickers, numStickers, camera, matrixUniforms, materialUniforms);

	for (auto face : { TOP, RIGHT, LEFT, BOTTOM }) {
		auto i = (face == BOTTOM) ? numStickers - m_rotationIndex - 1 : m_rotationIndex;

		DrawFace(face, rotationMat, 0, i, numStickers, i + 1, camera, matrixUniforms, materialUniforms);
		DrawFace(face, identityMat, 0, 0, numStickers, i, camera, matrixUniforms, materialUniforms);
		DrawFace(face, identityMat, 0, i + 1, numStickers, numStickers, camera, matrixUniforms, materialUniforms);
	}
}

void RubikCube::DrawUnitCubeGenericRotation(const Camera& camera,
	const glm::vec3& transformationVec,
	const MatrixShaderUniforms& matrixUniforms,
	const MaterialShaderUniforms& materialUniforms) const
{
	// Apply this function after transformationVec multiplication!
	// We have no other option than pass zeros on specific axes where we don't wanna apply scaling
	// and then reset these zeros back to one
	static auto resetZerosScale = [](glm::vec3& scale) -> auto& {
		scale.x = scale.x == 0.f ? 1.f : scale.x;
		scale.y = scale.y == 0.f ? 1.f : scale.y;
		scale.z = scale.z == 0.f ? 1.f : scale.z;
		return scale;
	};

	auto numStickers = GetNumStickersPerEdge();
	auto stickerSize = GetStickerSize();
	auto cubeSize = m_unitCube->CubeSize();

	// Rotating part
	m_unitCube->ApplyTransformations(m_userTransformations);
	m_unitCube->Translate(transformationVec * (stickerSize / 2.f - cubeSize / 2.f + m_rotationIndex * stickerSize));
	m_unitCube->Rotate(GetRotationAngle(), transformationVec);
	m_unitCube->Scale(resetZerosScale(transformationVec * stickerSize));
	m_unitCube->Draw(camera, matrixUniforms, materialUniforms);
	m_unitCube->ResetTransformations();

	// Static "left" side
	if (m_rotationIndex > 0) {
		m_unitCube->ApplyTransformations(m_userTransformations);
		m_unitCube->Translate(transformationVec * (m_rotationIndex * stickerSize / 2.f - cubeSize / 2.f));
		m_unitCube->Scale(resetZerosScale(transformationVec * (m_rotationIndex * stickerSize)));
		m_unitCube->Draw(camera, matrixUniforms, materialUniforms);
		m_unitCube->ResetTransformations();
	}

	// Static "right" side
	if (m_rotationIndex < numStickers - 1) {
		m_unitCube->ApplyTransformations(m_userTransformations);
		m_unitCube->Translate(transformationVec * (cubeSize / 2.f - (numStickers - m_rotationIndex - 1) * stickerSize / 2.f));
		m_unitCube->Scale(resetZerosScale(transformationVec * ((numStickers - m_rotationIndex - 1) * stickerSize)));
		m_unitCube->Draw(camera, matrixUniforms, materialUniforms);
		m_unitCube->ResetTransformations();
	}
}

void RubikCube::NewCube(unsigned int numStickersEdge)
{
	if (numStickersEdge == 0) {
		throw std::runtime_error("Number of stickers per edge cannot be zero");
	}
	if (numStickersEdge > MAX_STICKERS_PER_LINE) {
		throw std::runtime_error("Reached maximum number of stickers per line");
	}
	ResetAll();

	for (auto& face : m_faces) {
		face.clear();
	}
	
	FillFaceWithColor(TOP, Sticker::WHITE, numStickersEdge);
	FillFaceWithColor(BOTTOM, Sticker::YELLOW, numStickersEdge);
	FillFaceWithColor(FRONT, Sticker::RED, numStickersEdge);
	FillFaceWithColor(BACK, Sticker::ORANGE, numStickersEdge);
	FillFaceWithColor(LEFT, Sticker::GREEN, numStickersEdge);
	FillFaceWithColor(RIGHT, Sticker::BLUE, numStickersEdge);
}

bool RubikCube::Rotate(RubikCube::RotationType rotationType, unsigned int rotationIndex, bool rotationClockwise)
{
	if (rotationIndex >= GetNumStickersPerEdge()) {
		throw std::runtime_error("Rotation index is larger than number of stickers!");
	}
	if (m_rotationType != NONE) {
		return false;
	}

	m_rotationType = rotationType;
	m_rotationIndex = rotationIndex;
	m_rotationClockwise = rotationClockwise;
	m_rotationTimer = 0.f;

	return true;
}

void RubikCube::Update(float deltaTime)
{
	if (m_rotationType != NONE) {
		m_rotationTimer += deltaTime;

		if (m_rotationTimer >= ROTATION_TIME) {
			if (m_rotationType == X_AXIS) {
				SwapFacesXAxisRotation();
			}
			else if (m_rotationType == Y_AXIS) {
				SwapFacesYAxisRotation();
			}
			else {
				SwapFacesZAxisRotation();
			}
			m_rotationType = NONE;
		}
	}
}

void RubikCube::Draw(const Camera& camera,
	const MatrixShaderUniforms& matrixUniforms,
	const MaterialShaderUniforms& materialUniforms) const
{
	switch (m_rotationType) {
	case NONE:
		DrawCubeNoRotation(camera, matrixUniforms, materialUniforms);
		break;
	case X_AXIS:
		DrawCubeXAxisRotation(camera, matrixUniforms, materialUniforms);
		break;
	case Y_AXIS:
		DrawCubeYAxisRotation(camera, matrixUniforms, materialUniforms);
		break;
	case Z_AXIS:
		DrawCubeZAxisRotation(camera, matrixUniforms, materialUniforms);
		break;
	}
}
