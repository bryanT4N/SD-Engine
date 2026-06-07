#pragma once
#include "Engine/Math/IntVec2.hpp"

#include <string>

class ChessBoard;
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
	ChessMatch();
	~ChessMatch();

	void PrintBoardToDevConsole() const;
	void PrintGameStateToDevConsole() const;
	void PrintTurnHeaderToDevConsole() const;
	void PrintVictoryHeaderToDevConsole(std::string const& victoryAnnouncement) const;

	bool TryExecuteMove(
		IntVec2 const& fromSquare,
		IntVec2 const& toSquare,
		std::string& out_errorMessage,
		std::string* out_moveAnnouncement = nullptr,
		std::string* out_captureAnnouncement = nullptr,
		std::string* out_victoryAnnouncement = nullptr);
	static IntVec2 ParseSquareNotation(std::string const& notation);

	ChessBoard* GetBoard() const;
	ChessGameState GetCurrentState() const;
	int GetCurrentPlayerIdx() const;
	ChessPlayer* GetPlayer(int playerIdx) const;

private:
	ChessPlayer* m_players[2] = { nullptr, nullptr };
	ChessBoard* m_board = nullptr;
	ChessGameState m_currentState = ChessGameState::FIRST_PLAYER_TURN;
};
