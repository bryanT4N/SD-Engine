#pragma once
#include "Game/ChessObject.hpp"
#include "Engine/Math/IntVec2.hpp"

class ChessPiece;

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

	int CountOccupiedSquares() const;

private:
	bool IsSquareInsideBoard(IntVec2 const& square) const;

	ChessPiece* m_grid[BOARD_SIZE][BOARD_SIZE] = {};
};
