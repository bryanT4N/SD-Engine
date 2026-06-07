#include "Game/ChessPieceDefinition.hpp"

#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include <vector>

ChessPieceDefinition ChessPieceDefinition::s_definitions[(int)PieceType::NUM];

namespace
{
	constexpr float BASE_HEIGHT = 0.10f;
	constexpr float CYLINDER_BASE_RADIUS = 0.30f;
	constexpr float AABB_BASE_HALF_WIDTH = 0.30f;
	constexpr int PIECE_PLAYER_0 = 0;

	void EmitBase(std::vector<Vertex>& out_verts, int playerSide)
	{
		if (playerSide == PIECE_PLAYER_0) {
			Vec3 baseBottom(0.f, 0.f, 0.f);
			Vec3 baseTop(0.f, 0.f, BASE_HEIGHT);
			AddVertsForCylinder3D(out_verts, baseBottom, baseTop, CYLINDER_BASE_RADIUS);
		}
		else {
			AABB3 base(
				-AABB_BASE_HALF_WIDTH, -AABB_BASE_HALF_WIDTH, 0.f,
				AABB_BASE_HALF_WIDTH, AABB_BASE_HALF_WIDTH, BASE_HEIGHT);
			AddVertsForAABB3D(out_verts, base);
		}
	}

	void EmitCylinderSection(
		std::vector<Vertex>& out_verts,
		float radius,
		float bottomZ,
		float topZ)
	{
		Vec3 bottom(0.f, 0.f, bottomZ);
		Vec3 top(0.f, 0.f, topZ);
		AddVertsForCylinder3D(out_verts, bottom, top, radius);
	}

	void EmitOrientedBox(
		std::vector<Vertex>& out_verts,
		Vec3 const& center,
		EulerAngles const& orientation,
		Vec3 const& halfDimensions)
	{
		OBB3 box;
		box.m_center = center;
		box.m_halfDimensions = halfDimensions;
		box.SetOrientationFromEulerAngles(orientation);
		AddVertsForOBB3(out_verts, box);
	}

	void EmitFlatCrossPlate(
		std::vector<Vertex>& out_verts,
		float centerZ,
		float armHalfLength,
		float armHalfThickness,
		float armHalfHeight,
		int numArms)
	{
		Vec3 center(0.f, 0.f, centerZ);
		Vec3 halfDimensions(armHalfLength, armHalfThickness, armHalfHeight);
		float angleStep = 360.f / static_cast<float>(numArms * 2);
		for (int armIndex = 0; armIndex < numArms; ++armIndex) {
			float yawDeg = static_cast<float>(armIndex) * angleStep;
			EulerAngles orientation(yawDeg, 0.f, 0.f);
			EmitOrientedBox(out_verts, center, orientation, halfDimensions);
		}
	}

	void CreateMeshForPawn(std::vector<Vertex>& out_verts, int playerSide)
	{
		EmitBase(out_verts, playerSide);

		float const bodyRadius = 0.18f;
		float const bodyHeight = 0.32f;
		float const bodyTopZ = BASE_HEIGHT + bodyHeight;
		EmitCylinderSection(out_verts, bodyRadius, BASE_HEIGHT, bodyTopZ);

		float const collarRadius = 0.24f;
		float const collarHeight = 0.05f;
		float const collarTopZ = bodyTopZ + collarHeight;
		EmitCylinderSection(out_verts, collarRadius, bodyTopZ, collarTopZ);

		float const sphereRadius = 0.22f;
		float const sphereCenterZ = collarTopZ + sphereRadius * 0.6f;
		Vec3 sphereCenter(0.f, 0.f, sphereCenterZ);
		AddVertsForSphere3D(out_verts, sphereCenter, sphereRadius);
	}

	void CreateMeshForKnight(std::vector<Vertex>& out_verts, int playerSide)
	{
		EmitBase(out_verts, playerSide);

		float const facingSign = (playerSide == PIECE_PLAYER_0) ? 1.f : -1.f;

		float const bodyHalfX = 0.18f;
		float const bodyHalfY = 0.18f;
		float const bodyHalfZ = 0.32f;
		Vec3 bodyCenter(0.f, 0.f, BASE_HEIGHT + bodyHalfZ);
		EulerAngles bodyOrientation(0.f, 0.f, 0.f);
		EmitOrientedBox(out_verts, bodyCenter, bodyOrientation,
			Vec3(bodyHalfX, bodyHalfY, bodyHalfZ));
		float const bodyTopZ = BASE_HEIGHT + bodyHalfZ * 2.f;

		float const headForwardOffset = 0.10f * facingSign;
		float const headTiltPitchDeg = 25.f;
		float const headYawDeg = (playerSide == PIECE_PLAYER_0) ? 90.f : 270.f;
		Vec3 headCenter(0.f, headForwardOffset, bodyTopZ + 0.08f);
		EulerAngles headOrientation(headYawDeg, headTiltPitchDeg, 0.f);
		EmitOrientedBox(out_verts, headCenter, headOrientation,
			Vec3(0.24f, 0.15f, 0.12f));

		float const earBackOffset = -0.10f * facingSign;
		Vec3 earCenter(0.f, earBackOffset, bodyTopZ + 0.14f);
		EulerAngles earOrientation(0.f, 0.f, 0.f);
		EmitOrientedBox(out_verts, earCenter, earOrientation,
			Vec3(0.09f, 0.09f, 0.12f));
	}

	void CreateMeshForRook(std::vector<Vertex>& out_verts, int playerSide)
	{
		EmitBase(out_verts, playerSide);

		float const bodyHalfWidth = 0.20f;
		float const bodyHeight = 0.70f;
		float const bodyTopZ = BASE_HEIGHT + bodyHeight;
		AABB3 body(
			-bodyHalfWidth, -bodyHalfWidth, BASE_HEIGHT,
			bodyHalfWidth, bodyHalfWidth, bodyTopZ);
		AddVertsForAABB3D(out_verts, body);

		float const capHalfWidth = 0.24f;
		float const capHeight = 0.06f;
		AABB3 cap(
			-capHalfWidth, -capHalfWidth, bodyTopZ,
			capHalfWidth, capHalfWidth, bodyTopZ + capHeight);
		AddVertsForAABB3D(out_verts, cap);
	}

	void CreateMeshForBishop(std::vector<Vertex>& out_verts, int playerSide)
	{
		EmitBase(out_verts, playerSide);

		float const bodyRadius = 0.20f;
		float const bodyHeight = 0.55f;
		float const bodyTopZ = BASE_HEIGHT + bodyHeight;
		EmitCylinderSection(out_verts, bodyRadius, BASE_HEIGHT, bodyTopZ);

		float const collarRadius = 0.26f;
		float const collarHeight = 0.06f;
		float const collarTopZ = bodyTopZ + collarHeight;
		EmitCylinderSection(out_verts, collarRadius, bodyTopZ, collarTopZ);

		float const sphereRadius = 0.22f;
		float const sphereCenterZ = collarTopZ + sphereRadius * 0.6f;
		Vec3 sphereCenter(0.f, 0.f, sphereCenterZ);
		AddVertsForSphere3D(out_verts, sphereCenter, sphereRadius);

		float const topDotRadius = 0.06f;
		float const topDotCenterZ = sphereCenterZ + sphereRadius + topDotRadius * 0.5f;
		Vec3 topDotCenter(0.f, 0.f, topDotCenterZ);
		AddVertsForSphere3D(out_verts, topDotCenter, topDotRadius);
	}

	void CreateMeshForQueen(std::vector<Vertex>& out_verts, int playerSide)
	{
		EmitBase(out_verts, playerSide);

		float const bodyRadius = 0.24f;
		float const bodyHeight = 0.80f;
		float const bodyTopZ = BASE_HEIGHT + bodyHeight;
		EmitCylinderSection(out_verts, bodyRadius, BASE_HEIGHT, bodyTopZ);

		float const capRadius = 0.28f;
		float const capHeight = 0.06f;
		float const capTopZ = bodyTopZ + capHeight;
		EmitCylinderSection(out_verts, capRadius, bodyTopZ, capTopZ);

		float const armHalfLength = 0.24f;
		float const armHalfThickness = 0.06f;
		float const armHalfHeight = 0.06f;
		float const crossCenterZ = capTopZ + armHalfHeight;
		EmitFlatCrossPlate(out_verts, crossCenterZ,
			armHalfLength, armHalfThickness, armHalfHeight, 4);
	}

	void CreateMeshForKing(std::vector<Vertex>& out_verts, int playerSide)
	{
		EmitBase(out_verts, playerSide);

		float const bodyRadius = 0.24f;
		float const bodyHeight = 0.95f;
		float const bodyTopZ = BASE_HEIGHT + bodyHeight;
		EmitCylinderSection(out_verts, bodyRadius, BASE_HEIGHT, bodyTopZ);

		float const capRadius = 0.28f;
		float const capHeight = 0.06f;
		float const capTopZ = bodyTopZ + capHeight;
		EmitCylinderSection(out_verts, capRadius, bodyTopZ, capTopZ);

		float const armHalfLength = 0.24f;
		float const armHalfThickness = 0.06f;
		float const armHalfHeight = 0.06f;
		float const crossCenterZ = capTopZ + armHalfHeight;
		EmitFlatCrossPlate(out_verts, crossCenterZ,
			armHalfLength, armHalfThickness, armHalfHeight, 2);
	}

	void CreateMeshForType(PieceType pieceType, std::vector<Vertex>& out_verts, int playerSide)
	{
		switch (pieceType)
		{
		case PieceType::KING:	CreateMeshForKing(out_verts, playerSide);	break;
		case PieceType::QUEEN:	CreateMeshForQueen(out_verts, playerSide);	break;
		case PieceType::ROOK:	CreateMeshForRook(out_verts, playerSide);	break;
		case PieceType::BISHOP:	CreateMeshForBishop(out_verts, playerSide);	break;
		case PieceType::KNIGHT:	CreateMeshForKnight(out_verts, playerSide);	break;
		case PieceType::PAWN:	CreateMeshForPawn(out_verts, playerSide);	break;
		default: break;
		}
	}

	void UploadMeshForType(
		PieceType pieceType,
		VertexBuffer** out_vertexBuffersPerSide,
		IndexBuffer** out_indexBuffersPerSide,
		int* out_indexCountsPerSide)
	{
		if (g_engine == nullptr || g_engine->m_render == nullptr) {
			return;
		}
		Renderer* renderer = g_engine->m_render;

		for (int sideIndex = 0; sideIndex < 2; ++sideIndex) {
			std::vector<Vertex> verts;
			CreateMeshForType(pieceType, verts, sideIndex);
			if (verts.empty()) {
				continue;
			}

			size_t const vertCount = verts.size();
			std::vector<unsigned int> indices(vertCount);
			for (size_t vertIndex = 0; vertIndex < vertCount; ++vertIndex) {
				indices[vertIndex] = static_cast<unsigned int>(vertIndex);
			}

			unsigned int const vertexBytes = static_cast<unsigned int>(vertCount * sizeof(Vertex));
			unsigned int const indexBytes = static_cast<unsigned int>(vertCount * sizeof(unsigned int));

			out_vertexBuffersPerSide[sideIndex] = renderer->CreateVertexBuffer(vertexBytes, sizeof(Vertex));
			renderer->CopyCPUToGPU(verts.data(), vertexBytes, out_vertexBuffersPerSide[sideIndex]);

			out_indexBuffersPerSide[sideIndex] = renderer->CreateIndexBuffer(indexBytes, sizeof(unsigned int));
			renderer->CopyCPUToGPU(indices.data(), indexBytes, out_indexBuffersPerSide[sideIndex]);

			out_indexCountsPerSide[sideIndex] = static_cast<int>(vertCount);
		}
	}
}

ChessPieceDefinition::ChessPieceDefinition() = default;

void ChessPieceDefinition::InitializeAllDefinitions()
{
	s_definitions[(int)PieceType::KING].m_pieceType = PieceType::KING;
	s_definitions[(int)PieceType::KING].m_letter = 'K';
	s_definitions[(int)PieceType::KING].m_name = "king";

	s_definitions[(int)PieceType::QUEEN].m_pieceType = PieceType::QUEEN;
	s_definitions[(int)PieceType::QUEEN].m_letter = 'Q';
	s_definitions[(int)PieceType::QUEEN].m_name = "queen";

	s_definitions[(int)PieceType::ROOK].m_pieceType = PieceType::ROOK;
	s_definitions[(int)PieceType::ROOK].m_letter = 'R';
	s_definitions[(int)PieceType::ROOK].m_name = "rook";

	s_definitions[(int)PieceType::BISHOP].m_pieceType = PieceType::BISHOP;
	s_definitions[(int)PieceType::BISHOP].m_letter = 'B';
	s_definitions[(int)PieceType::BISHOP].m_name = "bishop";

	s_definitions[(int)PieceType::KNIGHT].m_pieceType = PieceType::KNIGHT;
	s_definitions[(int)PieceType::KNIGHT].m_letter = 'N';
	s_definitions[(int)PieceType::KNIGHT].m_name = "knight";

	s_definitions[(int)PieceType::PAWN].m_pieceType = PieceType::PAWN;
	s_definitions[(int)PieceType::PAWN].m_letter = 'P';
	s_definitions[(int)PieceType::PAWN].m_name = "pawn";

	for (int defIndex = 0; defIndex < (int)PieceType::NUM; ++defIndex) {
		UploadMeshForType(
			static_cast<PieceType>(defIndex),
			s_definitions[defIndex].m_vertexBuffersPerSide,
			s_definitions[defIndex].m_indexBuffersPerSide,
			s_definitions[defIndex].m_indexCountsPerSide);
	}
}

void ChessPieceDefinition::DestroyAllDefinitions()
{
	for (int defIndex = 0; defIndex < (int)PieceType::NUM; ++defIndex) {
		for (int sideIndex = 0; sideIndex < 2; ++sideIndex) {
			delete s_definitions[defIndex].m_vertexBuffersPerSide[sideIndex];
			s_definitions[defIndex].m_vertexBuffersPerSide[sideIndex] = nullptr;
			delete s_definitions[defIndex].m_indexBuffersPerSide[sideIndex];
			s_definitions[defIndex].m_indexBuffersPerSide[sideIndex] = nullptr;
			s_definitions[defIndex].m_indexCountsPerSide[sideIndex] = 0;
		}
	}
}

ChessPieceDefinition const& ChessPieceDefinition::GetDefinition(PieceType pieceType)
{
	GUARANTEE_OR_DIE(
		pieceType != PieceType::INVALID && pieceType < PieceType::NUM,
		"Invalid PieceType passed to ChessPieceDefinition::GetDefinition");
	return s_definitions[(int)pieceType];
}
