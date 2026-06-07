#include "Game/ChessPlayer.hpp"

#include "Engine/Core/StringUtils.hpp"

ChessPlayer::ChessPlayer(int playerIdx, Rgba8 const& displayTint, std::string const& colorName)
	: m_playerIdx(playerIdx)
	, m_displayTint(displayTint)
	, m_colorName(colorName)
{
}

std::string ChessPlayer::GetDisplayName() const
{
	return Stringf("Player #%d (%s)", m_playerIdx, m_colorName.c_str());
}
