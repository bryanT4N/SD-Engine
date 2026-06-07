#include "Game/ChessMatch.hpp"

#include "Game/ChessBoard.hpp"
#include "Game/ChessPiece.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Game/ChessPlayer.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/IntVec2.hpp"

#include <string>

static char const* TURN_SEPARATOR =
	"=================================================================";
static char const* VICTORY_SEPARATOR =
	"#################################################################";

ChessMatch::ChessMatch()
{
	m_players[0] = new ChessPlayer(0, PLAYER_TINT_GREEN, "Green");
	m_players[1] = new ChessPlayer(1, PLAYER_TINT_RED, "Red");

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

void ChessMatch::Render() const
{
	if (m_board == nullptr) {
		return;
	}
	m_board->Render();

	for (int file = 0; file < ChessBoard::BOARD_SIZE; ++file) {
		for (int rank = 0; rank < ChessBoard::BOARD_SIZE; ++rank) {
			ChessPiece* piece = m_board->GetPieceAt(IntVec2(file, rank));
			if (piece != nullptr) {
				piece->Render();
			}
		}
	}
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

bool ChessMatch::TryExecuteMove(
	IntVec2 const& fromSquare,
	IntVec2 const& toSquare,
	std::string& out_errorMessage,
	std::string* out_moveAnnouncement,
	std::string* out_captureAnnouncement,
	std::string* out_victoryAnnouncement)
{
	if (m_board == nullptr) {
		out_errorMessage = "Internal error: board is null";
		return false;
	}

	if (m_currentState == ChessGameState::GAME_OVER_PLAYER_0_WINS
		|| m_currentState == ChessGameState::GAME_OVER_PLAYER_1_WINS) {
		out_errorMessage = "Illegal move: game is over, no moves allowed.";
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
	if (movingPiece->m_ownerPlayerIdx != currentPlayerIdx) {
		ChessPieceDefinition const& movingDef =
			ChessPieceDefinition::GetDefinition(movingPiece->m_pieceType);
		out_errorMessage = Stringf(
			"Illegal move: the %s at %c%c belongs to player %d, but it is currently player %d's turn.",
			movingDef.m_name.c_str(),
			static_cast<char>('A' + fromSquare.x),
			static_cast<char>('1' + fromSquare.y),
			movingPiece->m_ownerPlayerIdx,
			currentPlayerIdx);
		return false;
	}

	ChessPiece* destPiece = m_board->GetPieceAt(toSquare);
	if (destPiece != nullptr && destPiece->m_ownerPlayerIdx == currentPlayerIdx) {
		ChessPieceDefinition const& destDef =
			ChessPieceDefinition::GetDefinition(destPiece->m_pieceType);
		out_errorMessage = Stringf(
			"Illegal move: %c%c is occupied by your own %s.",
			static_cast<char>('A' + toSquare.x),
			static_cast<char>('1' + toSquare.y),
			destDef.m_name.c_str());
		return false;
	}

	ChessPieceDefinition const& movingDef =
		ChessPieceDefinition::GetDefinition(movingPiece->m_pieceType);
	if (out_moveAnnouncement != nullptr) {
		*out_moveAnnouncement = Stringf(
			"Moved %s's %s from %c%c to %c%c",
			m_players[currentPlayerIdx]->GetDisplayName().c_str(),
			movingDef.m_name.c_str(),
			static_cast<char>('A' + fromSquare.x),
			static_cast<char>('1' + fromSquare.y),
			static_cast<char>('A' + toSquare.x),
			static_cast<char>('1' + toSquare.y));
	}

	bool capturedKingThisMove = false;
	if (destPiece != nullptr) {
		int victimPlayerIdx = destPiece->m_ownerPlayerIdx;
		ChessPieceDefinition const& capturedDef =
			ChessPieceDefinition::GetDefinition(destPiece->m_pieceType);
		if (out_captureAnnouncement != nullptr) {
			*out_captureAnnouncement = Stringf(
				"%s captured %s's %s at %c%c",
				m_players[currentPlayerIdx]->GetDisplayName().c_str(),
				m_players[victimPlayerIdx]->GetDisplayName().c_str(),
				capturedDef.m_name.c_str(),
				static_cast<char>('A' + toSquare.x),
				static_cast<char>('1' + toSquare.y));
		}
		if (destPiece->m_pieceType == PieceType::KING) {
			capturedKingThisMove = true;
		}
		m_board->CapturePieceAt(toSquare);
	}

	m_board->SetPieceAt(fromSquare, nullptr);
	movingPiece->m_square = toSquare;
	m_board->SetPieceAt(toSquare, movingPiece);

	if (capturedKingThisMove) {
		m_currentState = (currentPlayerIdx == 0)
			? ChessGameState::GAME_OVER_PLAYER_0_WINS
			: ChessGameState::GAME_OVER_PLAYER_1_WINS;
		if (out_victoryAnnouncement != nullptr) {
			*out_victoryAnnouncement = Stringf(
				"%s has won the match!",
				m_players[currentPlayerIdx]->GetDisplayName().c_str());
		}
	}
	else if (m_currentState == ChessGameState::FIRST_PLAYER_TURN) {
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
	case ChessGameState::GAME_OVER_PLAYER_1_WINS:
		stateText = "Match Completed";
		break;
	}
	g_engine->m_devConsole->AddLine(
		DevConsole::LOG_COLOR_INFO_MAJOR,
		Stringf("Game state is: %s", stateText));
}

void ChessMatch::PrintTurnHeaderToDevConsole() const
{
	if (g_engine == nullptr || g_engine->m_devConsole == nullptr) {
		return;
	}
	DevConsole* devConsole = g_engine->m_devConsole;

	devConsole->AddLine(DevConsole::LOG_COLOR_WARNING, TURN_SEPARATOR);

	int currentIdx = GetCurrentPlayerIdx();
	if (currentIdx >= 0 && currentIdx < 2 && m_players[currentIdx] != nullptr) {
		devConsole->AddLine(
			DevConsole::LOG_COLOR_WARNING,
			Stringf("%s -- it's your move!", m_players[currentIdx]->GetDisplayName().c_str()));
	}
	PrintGameStateToDevConsole();
	PrintBoardToDevConsole();
}

void ChessMatch::PrintVictoryHeaderToDevConsole(std::string const& victoryAnnouncement) const
{
	if (g_engine == nullptr || g_engine->m_devConsole == nullptr) {
		return;
	}
	DevConsole* devConsole = g_engine->m_devConsole;

	devConsole->AddLine(DevConsole::LOG_COLOR_WARNING, VICTORY_SEPARATOR);
	devConsole->AddLine(DevConsole::LOG_COLOR_WARNING, victoryAnnouncement);
	devConsole->AddLine(DevConsole::LOG_COLOR_WARNING, VICTORY_SEPARATOR);
}
