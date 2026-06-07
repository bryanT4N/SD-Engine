#pragma once
#include "Game/ChessObject.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"

class ChessPiece : public ChessObject
{
public:
	ChessPiece(PieceType pieceType, int ownerPlayerIdx, IntVec2 const& square);
	virtual ~ChessPiece() override = default;

	virtual void Render() const override;

	PieceType GetPieceType() const;
	int GetOwnerPlayerIdx() const;
	IntVec2 GetSquare() const;
	char GetDisplayLetter() const;

	void SetSquare(IntVec2 const& square);

private:
	PieceType m_pieceType = PieceType::INVALID;
	int m_ownerPlayerIdx = -1;
	IntVec2 m_square = IntVec2(-1, -1);
};
