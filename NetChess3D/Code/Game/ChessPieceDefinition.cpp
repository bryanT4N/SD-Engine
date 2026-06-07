#include "Game/ChessPieceDefinition.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

ChessPieceDefinition ChessPieceDefinition::s_definitions[(int)PieceType::NUM];

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
}

void ChessPieceDefinition::DestroyAllDefinitions()
{
}

ChessPieceDefinition const& ChessPieceDefinition::GetDefinition(PieceType pieceType)
{
	GUARANTEE_OR_DIE(
		pieceType != PieceType::INVALID && pieceType < PieceType::NUM,
		"Invalid PieceType passed to ChessPieceDefinition::GetDefinition");
	return s_definitions[(int)pieceType];
}

char ChessPieceDefinition::GetLetter() const
{
	return m_letter;
}

std::string ChessPieceDefinition::GetName() const
{
	return m_name;
}

PieceType ChessPieceDefinition::GetPieceType() const
{
	return m_pieceType;
}
