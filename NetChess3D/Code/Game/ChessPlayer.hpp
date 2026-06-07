#pragma once
#include "Engine/Core/Rgba8.hpp"

#include <string>

class ChessPlayer
{
public:
	int m_playerIdx = 0;
	Rgba8 m_displayTint = Rgba8::WHITE;
	std::string m_colorName;

	ChessPlayer(int playerIdx, Rgba8 const& displayTint, std::string const& colorName);

	std::string GetDisplayName() const;
};
