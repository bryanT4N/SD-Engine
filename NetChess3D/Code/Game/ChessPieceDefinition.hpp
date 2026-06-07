#pragma once

#include <string>

class VertexBuffer;
class IndexBuffer;

enum class PieceType
{
	INVALID = -1,
	KING,
	QUEEN,
	ROOK,
	BISHOP,
	KNIGHT,
	PAWN,
	NUM
};

class ChessPieceDefinition
{
public:
	static void InitializeAllDefinitions();
	static void DestroyAllDefinitions();
	static ChessPieceDefinition const& GetDefinition(PieceType pieceType);

	char GetLetter() const;
	std::string GetName() const;
	PieceType GetPieceType() const;

private:
	ChessPieceDefinition();

	PieceType m_pieceType = PieceType::INVALID;
	char m_letter = '?';
	std::string m_name;
	VertexBuffer* m_vertexBuffersPerSide[2] = { nullptr, nullptr };
	IndexBuffer* m_indexBuffersPerSide[2] = { nullptr, nullptr };

	static ChessPieceDefinition s_definitions[(int)PieceType::NUM];
};
