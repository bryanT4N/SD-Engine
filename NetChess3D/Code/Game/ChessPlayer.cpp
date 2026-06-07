#include "Game/ChessPlayer.hpp"

#include "Engine/Core/StringUtils.hpp"

ChessPlayer::ChessPlayer(int playerIdx, Rgba8 const& displayTint, std::string const& colorName)
	: m_playerIdx(playerIdx)
	, m_displayTint(displayTint)
	, m_colorName(colorName)
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

std::string ChessPlayer::GetColorName() const
{
	return m_colorName;
}

std::string ChessPlayer::GetDisplayName() const
{
	return Stringf("Player #%d (%s)", m_playerIdx, m_colorName.c_str());
}
