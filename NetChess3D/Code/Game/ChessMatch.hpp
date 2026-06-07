#pragma once

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

	ChessBoard* GetBoard() const;
	ChessGameState GetCurrentState() const;
	int GetCurrentPlayerIdx() const;
	ChessPlayer* GetPlayer(int playerIdx) const;

private:
	ChessPlayer* m_players[2] = { nullptr, nullptr };
	ChessBoard* m_board = nullptr;
	ChessGameState m_currentState = ChessGameState::FIRST_PLAYER_TURN;
};
