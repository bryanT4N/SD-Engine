#include "Game/ChessPlayer.hpp"

ChessPlayer::ChessPlayer(int playerIdx, Rgba8 const& displayTint)
	: m_playerIdx(playerIdx)
	, m_displayTint(displayTint)
{
}

int ChessPlayer::GetPlayerIdx() const
{
	return m_playerIdx;
}

Rgba8 ChessPlayer::GetDisplayTint() const
{
	return m_displayTint;
}
