#include "Game/ChessBoard.hpp"
#include "Game/ChessPiece.hpp"
#include "Game/ChessPieceDefinition.hpp"

ChessBoard::ChessBoard() = default;

ChessBoard::~ChessBoard()
{
	Clear();
}

void ChessBoard::Render() const
{
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
