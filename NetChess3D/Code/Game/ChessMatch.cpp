#include "Game/ChessMatch.hpp"

#include "Game/ChessBoard.hpp"
#include "Game/ChessPiece.hpp"
#include "Game/ChessPlayer.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/IntVec2.hpp"

#include <string>

ChessMatch::ChessMatch()
{
	Rgba8 const whitePlayerTint(255, 64, 64, 255);
	Rgba8 const blackPlayerTint(64, 255, 64, 255);
	m_players[0] = new ChessPlayer(0, whitePlayerTint);
	m_players[1] = new ChessPlayer(1, blackPlayerTint);

	m_board = new ChessBoard();
	m_board->PopulateInitialPieces();
}

ChessMatch::~ChessMatch()
{
	delete m_board;
	m_board = nullptr;
	delete m_players[0];
	m_players[0] = nullptr;
	delete m_players[1];
	m_players[1] = nullptr;
}

ChessBoard* ChessMatch::GetBoard() const
{
	return m_board;
}

ChessGameState ChessMatch::GetCurrentState() const
{
	return m_currentState;
}

int ChessMatch::GetCurrentPlayerIdx() const
{
	if (m_currentState == ChessGameState::FIRST_PLAYER_TURN) {
		return 0;
	}
	if (m_currentState == ChessGameState::SECOND_PLAYER_TURN) {
		return 1;
	}
	return -1;
}

ChessPlayer* ChessMatch::GetPlayer(int playerIdx) const
{
	if (playerIdx < 0 || playerIdx > 1) {
		return nullptr;
	}
	return m_players[playerIdx];
}

void ChessMatch::PrintBoardToDevConsole() const
{
	if (g_engine == nullptr || g_engine->m_devConsole == nullptr || m_board == nullptr) {
		return;
	}

	std::string boardBlock;
	boardBlock += "  ABCDEFGH\n";
	boardBlock += " +--------+\n";
	for (int rank = ChessBoard::BOARD_SIZE - 1; rank >= 0; --rank) {
		char rankChar = static_cast<char>('1' + rank);
		boardBlock += rankChar;
		boardBlock += '|';
		for (int file = 0; file < ChessBoard::BOARD_SIZE; ++file) {
			ChessPiece* piece = m_board->GetPieceAt(IntVec2(file, rank));
			boardBlock += (piece != nullptr) ? piece->GetDisplayLetter() : '.';
		}
		boardBlock += '|';
		boardBlock += rankChar;
		boardBlock += '\n';
	}
	boardBlock += " +--------+\n";
	boardBlock += "  ABCDEFGH";

	float const boardCellAspectOverride = 1.0f;
	g_engine->m_devConsole->AddLine(
		DevConsole::LOG_COLOR_INFO_MINOR, boardBlock, boardCellAspectOverride);
}

IntVec2 ChessMatch::ParseSquareNotation(std::string const& notation)
{
	if (notation.length() != 2) {
		return IntVec2(-1, -1);
	}
	char fileChar = notation[0];
	char rankChar = notation[1];

	int fileIndex = -1;
	if (fileChar >= 'a' && fileChar <= 'h') {
		fileIndex = fileChar - 'a';
	}
	int rankIndex = -1;
	if (rankChar >= '1' && rankChar <= '8') {
		rankIndex = rankChar - '1';
	}

	if (fileIndex < 0 || rankIndex < 0) {
		return IntVec2(-1, -1);
	}
	return IntVec2(fileIndex, rankIndex);
}

bool ChessMatch::TryExecuteMove(IntVec2 const& fromSquare, IntVec2 const& toSquare, std::string& out_errorMessage)
{
	if (m_board == nullptr) {
		out_errorMessage = "Internal error: board is null";
		return false;
	}

	if (fromSquare.x == toSquare.x && fromSquare.y == toSquare.y) {
		out_errorMessage = Stringf("Illegal move: 'from' and 'to' are the same square (%c%c).",
			static_cast<char>('a' + fromSquare.x),
			static_cast<char>('1' + fromSquare.y));
		return false;
	}

	ChessPiece* movingPiece = m_board->GetPieceAt(fromSquare);
	if (movingPiece == nullptr) {
		out_errorMessage = Stringf("Illegal move: there is no piece at %c%c.",
			static_cast<char>('A' + fromSquare.x),
			static_cast<char>('1' + fromSquare.y));
		return false;
	}

	int currentPlayerIdx = GetCurrentPlayerIdx();
	if (movingPiece->GetOwnerPlayerIdx() != currentPlayerIdx) {
		ChessPieceDefinition const& movingDef =
			ChessPieceDefinition::GetDefinition(movingPiece->GetPieceType());
		out_errorMessage = Stringf(
			"Illegal move: the %s at %c%c belongs to player %d, but it is currently player %d's turn.",
			movingDef.GetName().c_str(),
			static_cast<char>('A' + fromSquare.x),
			static_cast<char>('1' + fromSquare.y),
			movingPiece->GetOwnerPlayerIdx(),
			currentPlayerIdx);
		return false;
	}

	ChessPiece* destPiece = m_board->GetPieceAt(toSquare);
	if (destPiece != nullptr && destPiece->GetOwnerPlayerIdx() == currentPlayerIdx) {
		ChessPieceDefinition const& destDef =
			ChessPieceDefinition::GetDefinition(destPiece->GetPieceType());
		out_errorMessage = Stringf(
			"Illegal move: %c%c is occupied by your own %s.",
			static_cast<char>('A' + toSquare.x),
			static_cast<char>('1' + toSquare.y),
			destDef.GetName().c_str());
		return false;
	}

	if (destPiece != nullptr) {
		m_board->SetPieceAt(toSquare, nullptr);
		delete destPiece;
	}

	m_board->SetPieceAt(fromSquare, nullptr);
	movingPiece->SetSquare(toSquare);
	m_board->SetPieceAt(toSquare, movingPiece);

	if (m_currentState == ChessGameState::FIRST_PLAYER_TURN) {
		m_currentState = ChessGameState::SECOND_PLAYER_TURN;
	}
	else if (m_currentState == ChessGameState::SECOND_PLAYER_TURN) {
		m_currentState = ChessGameState::FIRST_PLAYER_TURN;
	}

	return true;
}

void ChessMatch::PrintGameStateToDevConsole() const
{
	if (g_engine == nullptr || g_engine->m_devConsole == nullptr) {
		return;
	}
	char const* stateText = "Unknown";
	switch (m_currentState)
	{
	case ChessGameState::FIRST_PLAYER_TURN:
		stateText = "First Player's Turn";
		break;
	case ChessGameState::SECOND_PLAYER_TURN:
		stateText = "Second Player's Turn";
		break;
	case ChessGameState::GAME_OVER_PLAYER_0_WINS:
		stateText = "Game Over - First Player Wins";
		break;
	case ChessGameState::GAME_OVER_PLAYER_1_WINS:
		stateText = "Game Over - Second Player Wins";
		break;
	}
	g_engine->m_devConsole->AddLine(
		DevConsole::LOG_COLOR_INFO_MINOR,
		Stringf("GameState: %s", stateText));
}
