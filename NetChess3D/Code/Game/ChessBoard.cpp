#include "Game/ChessBoard.hpp"
#include "Game/ChessPiece.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

ChessBoard::ChessBoard()
{
	UploadMeshToGPU();
}

ChessBoard::~ChessBoard()
{
	Clear();
	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;
}

void ChessBoard::Render() const
{
	if (g_engine == nullptr || g_engine->m_render == nullptr) {
		return;
	}
	if (m_vertexBuffer == nullptr || m_vertexCount == 0) {
		return;
	}
	Renderer* renderer = g_engine->m_render;
	renderer->SetModelCBO(Mat44(), Rgba8::WHITE);
	renderer->BindTexture(renderer->CreateOrGetTextureFromFile("Data/Images/woodfloor_d.png"), 0);
	renderer->BindTexture(renderer->CreateOrGetTextureFromFile("Data/Images/woodfloor_n.png"), 1);
	renderer->DrawVertexBuffer(m_vertexBuffer, static_cast<unsigned int>(m_vertexCount));
}

void ChessBoard::Clear()
{
	for (int file = 0; file < BOARD_SIZE; ++file) {
		for (int rank = 0; rank < BOARD_SIZE; ++rank) {
			delete m_grid[file][rank];
			m_grid[file][rank] = nullptr;
		}
	}
}

bool ChessBoard::IsSquareInsideBoard(IntVec2 const& square) const
{
	return square.x >= 0 && square.x < BOARD_SIZE
		&& square.y >= 0 && square.y < BOARD_SIZE;
}

ChessPiece* ChessBoard::GetPieceAt(IntVec2 const& square) const
{
	if (!IsSquareInsideBoard(square)) {
		return nullptr;
	}
	return m_grid[square.x][square.y];
}

void ChessBoard::SetPieceAt(IntVec2 const& square, ChessPiece* piece)
{
	if (!IsSquareInsideBoard(square)) {
		return;
	}
	m_grid[square.x][square.y] = piece;
}

void ChessBoard::CapturePieceAt(IntVec2 const& square)
{
	if (!IsSquareInsideBoard(square)) {
		return;
	}
	ChessPiece* victim = m_grid[square.x][square.y];
	if (victim != nullptr) {
		delete victim;
		m_grid[square.x][square.y] = nullptr;
	}
}

int ChessBoard::CountOccupiedSquares() const
{
	int occupiedCount = 0;
	for (int file = 0; file < BOARD_SIZE; ++file) {
		for (int rank = 0; rank < BOARD_SIZE; ++rank) {
			if (m_grid[file][rank] != nullptr) {
				++occupiedCount;
			}
		}
	}
	return occupiedCount;
}

void ChessBoard::PopulateInitialPieces()
{
	Clear();

	PieceType const backRankOrder[BOARD_SIZE] = {
		PieceType::ROOK,
		PieceType::KNIGHT,
		PieceType::BISHOP,
		PieceType::QUEEN,
		PieceType::KING,
		PieceType::BISHOP,
		PieceType::KNIGHT,
		PieceType::ROOK
	};

	int const whitePlayerIdx = 0;
	int const blackPlayerIdx = 1;
	int const whiteBackRank = 0;
	int const whitePawnRank = 1;
	int const blackPawnRank = 6;
	int const blackBackRank = 7;

	for (int file = 0; file < BOARD_SIZE; ++file) {
		m_grid[file][whiteBackRank] = new ChessPiece(
			backRankOrder[file], whitePlayerIdx, IntVec2(file, whiteBackRank));
		m_grid[file][whitePawnRank] = new ChessPiece(
			PieceType::PAWN, whitePlayerIdx, IntVec2(file, whitePawnRank));
		m_grid[file][blackPawnRank] = new ChessPiece(
			PieceType::PAWN, blackPlayerIdx, IntVec2(file, blackPawnRank));
		m_grid[file][blackBackRank] = new ChessPiece(
			backRankOrder[file], blackPlayerIdx, IntVec2(file, blackBackRank));
	}
}

void ChessBoard::CreateMeshOnCPU(std::vector<Vertex>& out_verts) const
{
	float const boardMinXY = -BOARD_MARGIN_SIZE;
	float const boardMaxXY = static_cast<float>(BOARD_SIZE) * BOARD_SQUARE_SIZE + BOARD_MARGIN_SIZE;
	float const topZ = 0.f;
	float const marginTopZ = -BOARD_TILE_RECESS_DEPTH;
	float const bottomZ = -BOARD_THICKNESS;

	int const tileQuadVertCount = 36;
	int const totalAABBCount = 64 + 1;
	out_verts.reserve(totalAABBCount * tileQuadVertCount);

	for (int rank = 0; rank < BOARD_SIZE; ++rank) {
		for (int file = 0; file < BOARD_SIZE; ++file) {
			float fileMin = static_cast<float>(file) * BOARD_SQUARE_SIZE;
			float fileMax = fileMin + BOARD_SQUARE_SIZE;
			float rankMin = static_cast<float>(rank) * BOARD_SQUARE_SIZE;
			float rankMax = rankMin + BOARD_SQUARE_SIZE;
			bool isLightSquare = ((file + rank) % 2 == 1);
			Rgba8 tileColor = isLightSquare ? LIGHT_SQUARE_COLOR : DARK_SQUARE_COLOR;

			AABB3 tileBounds(fileMin, rankMin, marginTopZ, fileMax, rankMax, topZ);
			AddVertsForAABB3D(out_verts, tileBounds, tileColor);
		}
	}

	AABB3 frameBounds(boardMinXY, boardMinXY, bottomZ, boardMaxXY, boardMaxXY, marginTopZ);
	AddVertsForAABB3D(out_verts, frameBounds, BOARD_FRAME_COLOR);
}

void ChessBoard::UploadMeshToGPU()
{
	if (g_engine == nullptr || g_engine->m_render == nullptr) {
		return;
	}
	Renderer* renderer = g_engine->m_render;

	std::vector<Vertex> verts;
	CreateMeshOnCPU(verts);
	if (verts.empty()) {
		return;
	}

	unsigned int vertexBytes = static_cast<unsigned int>(verts.size() * sizeof(Vertex));
	m_vertexBuffer = renderer->CreateVertexBuffer(vertexBytes, sizeof(Vertex));
	renderer->CopyCPUToGPU(verts.data(), vertexBytes, m_vertexBuffer);

	m_vertexCount = static_cast<int>(verts.size());
}
