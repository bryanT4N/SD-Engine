#pragma once
#include "Engine/Math/IntVec2.hpp"

#include <string>

class ChessBoard;
class ChessPiece;
class ChessPlayer;

enum class ChessGameState
{
	FIRST_PLAYER_TURN,
	SECOND_PLAYER_TURN,
	GAME_OVER_PLAYER_0_WINS,
	GAME_OVER_PLAYER_1_WINS
};

class ChessMatch
{
public:
	ChessPlayer* m_players[2] = { nullptr, nullptr };
	ChessBoard* m_board = nullptr;
	ChessGameState m_currentState = ChessGameState::FIRST_PLAYER_TURN;
	IntVec2 m_enPassantSquare = IntVec2(-1, -1);
	IntVec2 m_enPassantVictimSquare = IntVec2(-1, -1);

	ChessMatch();
	~ChessMatch();

	void Render() const;
	void Update(float deltaSeconds);

	void PrintBoardToDevConsole() const;
	void PrintGameStateToDevConsole() const;
	void PrintTurnHeaderToDevConsole() const;
	void PrintVictoryHeaderToDevConsole(std::string const& victoryAnnouncement) const;

	bool TryExecuteMove(
		IntVec2 const& fromSquare,
		IntVec2 const& toSquare,
		bool isTeleport,
		std::string const& promoteTo,
		std::string& out_errorMessage,
		std::string* out_moveAnnouncement = nullptr,
		std::string* out_captureAnnouncement = nullptr,
		std::string* out_victoryAnnouncement = nullptr);
	static IntVec2 ParseSquareNotation(std::string const& notation);

	bool ApplyOverride(std::string const& board64, std::string& out_error);

	int GetCurrentPlayerIdx() const;

private:
	bool IsMoveGeometryLegalForPiece(
		ChessPiece const* movingPiece,
		IntVec2 const& fromSquare,
		IntVec2 const& toSquare,
		std::string& out_errorMessage) const;
	bool IsSlidingPathClear(
		IntVec2 const& fromSquare,
		IntVec2 const& toSquare,
		std::string& out_errorMessage) const;
	bool IsPawnMoveLegal(
		ChessPiece const* movingPiece,
		IntVec2 const& fromSquare,
		IntVec2 const& toSquare,
		std::string& out_errorMessage) const;
	bool IsKingDistanceLegal(
		ChessPiece const* movingKing,
		IntVec2 const& toSquare,
		std::string& out_errorMessage) const;
	bool IsCastlingLegal(
		ChessPiece const* movingKing,
		IntVec2 const& fromSquare,
		IntVec2 const& toSquare,
		IntVec2& out_rookFromSquare,
		IntVec2& out_rookToSquare,
		std::string& out_errorMessage) const;
};
