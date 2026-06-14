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

static bool TryParseBoardChar(char asciiPiece, PieceType& out_type, int& out_owner)
{
	bool isBlack = (asciiPiece >= 'a' && asciiPiece <= 'z');
	out_owner = isBlack ? 1 : 0;
	char upper = isBlack ? static_cast<char>(asciiPiece - 'a' + 'A') : asciiPiece;
	switch (upper)
	{
	case 'K':	out_type = PieceType::KING;		return true;
	case 'Q':	out_type = PieceType::QUEEN;		return true;
	case 'R':	out_type = PieceType::ROOK;		return true;
	case 'B':	out_type = PieceType::BISHOP;	return true;
	case 'N':	out_type = PieceType::KNIGHT;	return true;
	case 'P':	out_type = PieceType::PAWN;		return true;
	default:									return false;
	}
}

static bool TryParsePromoteToType(std::string const& value, PieceType& out_type)
{
	std::string lower;
	lower.reserve(value.size());
	for (char c : value) {
		lower.push_back((c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c);
	}
	if (lower == "queen")	{ out_type = PieceType::QUEEN;	return true; }
	if (lower == "rook")	{ out_type = PieceType::ROOK;	return true; }
	if (lower == "bishop")	{ out_type = PieceType::BISHOP;	return true; }
	if (lower == "knight")	{ out_type = PieceType::KNIGHT;	return true; }
	return false;
}

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
	bool isTeleport,
	std::string const& promoteTo,
	std::string& out_errorMessage,
	std::string* out_moveAnnouncement,
	std::string* out_captureAnnouncement,
	std::string* out_victoryAnnouncement)
{
	if (m_board == nullptr) {
		out_errorMessage = "Illegal move: no board.";
		return false;
	}

	if (m_currentState == ChessGameState::GAME_OVER_PLAYER_0_WINS
		|| m_currentState == ChessGameState::GAME_OVER_PLAYER_1_WINS) {
		out_errorMessage = "Illegal move: game is over.";
		return false;
	}

	if (fromSquare.x == toSquare.x && fromSquare.y == toSquare.y) {
		out_errorMessage = "Illegal move: same square.";
		return false;
	}

	ChessPiece* movingPiece = m_board->GetPieceAt(fromSquare);
	if (movingPiece == nullptr) {
		out_errorMessage = Stringf("Illegal move: no piece at %c%c.",
			static_cast<char>('A' + fromSquare.x),
			static_cast<char>('1' + fromSquare.y));
		return false;
	}

	int currentPlayerIdx = GetCurrentPlayerIdx();
	if (movingPiece->m_ownerPlayerIdx != currentPlayerIdx) {
		out_errorMessage = "Illegal move: not your turn.";
		return false;
	}

	ChessPiece* destPiece = m_board->GetPieceAt(toSquare);
	if (destPiece != nullptr && destPiece->m_ownerPlayerIdx == currentPlayerIdx) {
		out_errorMessage = "Illegal move: your own piece is there.";
		return false;
	}

	if (!isTeleport) {
		std::string geometryError;
		if (!IsMoveGeometryLegalForPiece(movingPiece, fromSquare, toSquare, geometryError)) {
			out_errorMessage = geometryError;
			return false;
		}

		PieceType movingType = movingPiece->m_pieceType;
		bool isSlidingPiece =
			movingType == PieceType::ROOK ||
			movingType == PieceType::BISHOP ||
			movingType == PieceType::QUEEN;
		if (isSlidingPiece) {
			std::string blockedError;
			if (!IsSlidingPathClear(fromSquare, toSquare, blockedError)) {
				out_errorMessage = blockedError;
				return false;
			}
		}

		if (movingType == PieceType::KING) {
			std::string kingDistanceError;
			if (!IsKingDistanceLegal(movingPiece, toSquare, kingDistanceError)) {
				out_errorMessage = kingDistanceError;
				return false;
			}
		}
	}

	bool isPromotion = false;
	PieceType promoteToType = PieceType::INVALID;
	if (movingPiece->m_pieceType == PieceType::PAWN) {
		bool reachesFarRank =
			(movingPiece->m_ownerPlayerIdx == 0 && toSquare.y == ChessBoard::BOARD_SIZE - 1) ||
			(movingPiece->m_ownerPlayerIdx == 1 && toSquare.y == 0);
		if (reachesFarRank) {
			if (promoteTo.empty()) {
				out_errorMessage = "Illegal move: need promoteTo= (queen/rook/bishop/knight).";
				return false;
			}
			if (!TryParsePromoteToType(promoteTo, promoteToType)) {
				out_errorMessage = Stringf("Illegal move: bad promoteTo: %s", promoteTo.c_str());
				return false;
			}
			isPromotion = true;
		}
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
		if (isPromotion) {
			*out_moveAnnouncement += Stringf(
				" and promoted to %s",
				ChessPieceDefinition::GetDefinition(promoteToType).m_name.c_str());
		}
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
	movingPiece->m_hasEverMoved = true;
	m_board->SetPieceAt(toSquare, movingPiece);
	if (isPromotion) {
		movingPiece->m_pieceType = promoteToType;
	}

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

bool ChessMatch::ApplyOverride(std::string const& board64, std::string& out_error)
{
	if (m_board == nullptr) {
		out_error = "Illegal ChessOverride: no board.";
		return false;
	}

	int const squareCount = ChessBoard::BOARD_SIZE * ChessBoard::BOARD_SIZE;
	if (static_cast<int>(board64.length()) != squareCount) {
		out_error = Stringf(
			"Illegal ChessOverride: board must be 64 chars (got %d).",
			static_cast<int>(board64.length()));
		return false;
	}

	for (int index = 0; index < squareCount; ++index) {
		char asciiPiece = board64[index];
		PieceType parsedType;
		int parsedOwner;
		if (asciiPiece != '.' && !TryParseBoardChar(asciiPiece, parsedType, parsedOwner)) {
			out_error = Stringf("Illegal ChessOverride: bad char '%c' at %d.", asciiPiece, index);
			return false;
		}
	}

	for (int index = 0; index < squareCount; ++index) {
		int file = index % ChessBoard::BOARD_SIZE;
		int rank = index / ChessBoard::BOARD_SIZE;
		IntVec2 square(file, rank);
		char desired = board64[index];

		ChessPiece* existing = m_board->GetPieceAt(square);
		char current = (existing != nullptr) ? existing->GetDisplayLetter() : '.';
		if (current == desired) {
			continue;
		}

		if (existing != nullptr) {
			m_board->CapturePieceAt(square);
		}
		if (desired != '.') {
			PieceType parsedType;
			int parsedOwner;
			TryParseBoardChar(desired, parsedType, parsedOwner);
			m_board->SetPieceAt(square, new ChessPiece(parsedType, parsedOwner, square));
		}
	}

	for (int file = 0; file < ChessBoard::BOARD_SIZE; ++file) {
		for (int rank = 0; rank < ChessBoard::BOARD_SIZE; ++rank) {
			ChessPiece* piece = m_board->GetPieceAt(IntVec2(file, rank));
			if (piece != nullptr) {
				piece->m_hasEverMoved = false;
			}
		}
	}

	return true;
}

bool ChessMatch::IsMoveGeometryLegalForPiece(
	ChessPiece const* movingPiece,
	IntVec2 const& fromSquare,
	IntVec2 const& toSquare,
	std::string& out_errorMessage) const
{
	int deltaFile = toSquare.x - fromSquare.x;
	int deltaRank = toSquare.y - fromSquare.y;
	int absFile = (deltaFile < 0) ? -deltaFile : deltaFile;
	int absRank = (deltaRank < 0) ? -deltaRank : deltaRank;
	bool isDiagonal = (absFile == absRank) && (absFile != 0);
	bool isAxisAligned = (deltaFile == 0) != (deltaRank == 0);

	switch (movingPiece->m_pieceType)
	{
	case PieceType::BISHOP:
		if (isDiagonal) {
			return true;
		}
		out_errorMessage = "Illegal move: bishops move diagonally.";
		return false;
	case PieceType::ROOK:
		if (isAxisAligned) {
			return true;
		}
		out_errorMessage = "Illegal move: rooks move straight.";
		return false;
	case PieceType::QUEEN:
		if (isDiagonal || isAxisAligned) {
			return true;
		}
		out_errorMessage = "Illegal move: queens move straight or diagonally.";
		return false;
	case PieceType::KNIGHT:
		if ((absFile == 1 && absRank == 2) || (absFile == 2 && absRank == 1)) {
			return true;
		}
		out_errorMessage = "Illegal move: knights move in an L.";
		return false;
	case PieceType::KING:
		if (absFile <= 1 && absRank <= 1) {
			return true;
		}
		out_errorMessage = "Illegal move: kings move one square.";
		return false;
	case PieceType::PAWN:
		return IsPawnMoveLegal(movingPiece, fromSquare, toSquare, out_errorMessage);
	default:
		return true;
	}
}

bool ChessMatch::IsSlidingPathClear(
	IntVec2 const& fromSquare,
	IntVec2 const& toSquare,
	std::string& out_errorMessage) const
{
	int deltaFile = toSquare.x - fromSquare.x;
	int deltaRank = toSquare.y - fromSquare.y;
	int stepFile = (deltaFile > 0) - (deltaFile < 0);
	int stepRank = (deltaRank > 0) - (deltaRank < 0);

	IntVec2 cursor(fromSquare.x + stepFile, fromSquare.y + stepRank);
	while (cursor.x != toSquare.x || cursor.y != toSquare.y) {
		if (m_board->GetPieceAt(cursor) != nullptr) {
			out_errorMessage = Stringf(
				"Illegal move: path blocked at %c%c.",
				static_cast<char>('A' + cursor.x),
				static_cast<char>('1' + cursor.y));
			return false;
		}
		cursor.x += stepFile;
		cursor.y += stepRank;
	}
	return true;
}

bool ChessMatch::IsPawnMoveLegal(
	ChessPiece const* movingPiece,
	IntVec2 const& fromSquare,
	IntVec2 const& toSquare,
	std::string& out_errorMessage) const
{
	int forwardDir = (movingPiece->m_ownerPlayerIdx == 0) ? 1 : -1;
	int deltaFile = toSquare.x - fromSquare.x;
	int deltaRank = toSquare.y - fromSquare.y;
	int absFile = (deltaFile < 0) ? -deltaFile : deltaFile;
	ChessPiece* destPiece = m_board->GetPieceAt(toSquare);

	if (deltaFile == 0) {
		if (deltaRank == forwardDir) {
			if (destPiece != nullptr) {
				out_errorMessage = "Illegal move: pawns can't capture forward.";
				return false;
			}
			return true;
		}
		if (deltaRank == 2 * forwardDir) {
			if (movingPiece->m_hasEverMoved) {
				out_errorMessage = "Illegal move: pawn already moved.";
				return false;
			}
			IntVec2 middleSquare(fromSquare.x, fromSquare.y + forwardDir);
			if (m_board->GetPieceAt(middleSquare) != nullptr || destPiece != nullptr) {
				out_errorMessage = "Illegal move: path blocked.";
				return false;
			}
			return true;
		}
		out_errorMessage = "Illegal move: pawns move forward.";
		return false;
	}

	if (absFile == 1 && deltaRank == forwardDir) {
		if (destPiece != nullptr) {
			return true;
		}
		out_errorMessage = "Illegal move: pawns capture diagonally.";
		return false;
	}

	out_errorMessage = "Illegal move: bad pawn move.";
	return false;
}

bool ChessMatch::IsKingDistanceLegal(
	ChessPiece const* movingKing,
	IntVec2 const& toSquare,
	std::string& out_errorMessage) const
{
	int enemyOwner = 1 - movingKing->m_ownerPlayerIdx;
	for (int file = 0; file < ChessBoard::BOARD_SIZE; ++file) {
		for (int rank = 0; rank < ChessBoard::BOARD_SIZE; ++rank) {
			ChessPiece* piece = m_board->GetPieceAt(IntVec2(file, rank));
			if (piece == nullptr || piece->m_pieceType != PieceType::KING
				|| piece->m_ownerPlayerIdx != enemyOwner) {
				continue;
			}
			int deltaFile = toSquare.x - file;
			int deltaRank = toSquare.y - rank;
			int absFile = (deltaFile < 0) ? -deltaFile : deltaFile;
			int absRank = (deltaRank < 0) ? -deltaRank : deltaRank;
			int chebyshevDistance = (absFile > absRank) ? absFile : absRank;
			if (chebyshevDistance < 2) {
				out_errorMessage = "Illegal move: kings can't be adjacent.";
				return false;
			}
		}
	}
	return true;
}
