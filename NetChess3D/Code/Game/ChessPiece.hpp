#pragma once
#include "Game/ChessObject.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"

class ChessPiece : public ChessObject
{
public:
	PieceType m_pieceType = PieceType::INVALID;
	int m_ownerPlayerIdx = -1;
	IntVec2 m_square = IntVec2(-1, -1);
	bool m_hasEverMoved = false;

	bool m_isAnimating = false;
	Vec3 m_animStartPos;
	Vec3 m_animEndPos;
	float m_animElapsed = 0.f;
	bool m_animIsHop = false;

	ChessPiece(PieceType pieceType, int ownerPlayerIdx, IntVec2 const& square);
	virtual ~ChessPiece() override = default;

	virtual void Render() const override;

	void StartMoveAnimation(IntVec2 const& fromSquare, IntVec2 const& toSquare, bool isHop);
	void UpdateAnimation(float deltaSeconds);

	char GetDisplayLetter() const;
};
