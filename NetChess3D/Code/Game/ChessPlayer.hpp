#pragma once
#include "Engine/Core/Rgba8.hpp"

#include <string>

class ChessPlayer
{
public:
	ChessPlayer(int playerIdx, Rgba8 const& displayTint, std::string const& colorName);

	int GetPlayerIdx() const;
	Rgba8 GetDisplayTint() const;
	std::string GetColorName() const;
	std::string GetDisplayName() const;

private:
	int m_playerIdx = 0;
	Rgba8 m_displayTint = Rgba8::WHITE;
	std::string m_colorName;
};
