#pragma once
#include "Game/ChessObject.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"

class ChessPiece : public ChessObject
{
public:
	PieceType m_pieceType = PieceType::INVALID;
	int m_ownerPlayerIdx = -1;
	IntVec2 m_square = IntVec2(-1, -1);

	ChessPiece(PieceType pieceType, int ownerPlayerIdx, IntVec2 const& square);
	virtual ~ChessPiece() override = default;

	virtual void Render() const override;

	char GetDisplayLetter() const;
};
