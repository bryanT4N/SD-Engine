#pragma once
#include "Game/ChessObject.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/IntVec2.hpp"

#include <vector>

class ChessPiece;
class VertexBuffer;

class ChessBoard : public ChessObject
{
public:
	static constexpr int BOARD_SIZE = 8;

	ChessBoard();
	virtual ~ChessBoard() override;

	virtual void Render() const override;

	void PopulateInitialPieces();
	void Clear();

	ChessPiece* GetPieceAt(IntVec2 const& square) const;
	void SetPieceAt(IntVec2 const& square, ChessPiece* piece);
	void CapturePieceAt(IntVec2 const& square);

	int CountOccupiedSquares() const;

private:
	bool IsSquareInsideBoard(IntVec2 const& square) const;

	void CreateMeshOnCPU(std::vector<Vertex>& out_verts) const;
	void UploadMeshToGPU();

	ChessPiece* m_grid[BOARD_SIZE][BOARD_SIZE] = {};

	VertexBuffer* m_vertexBuffer = nullptr;
	int m_vertexCount = 0;
};
