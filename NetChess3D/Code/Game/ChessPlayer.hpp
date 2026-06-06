#pragma once
#include "Engine/Core/Rgba8.hpp"

class ChessPlayer
{
public:
	ChessPlayer(int playerIdx, Rgba8 const& displayTint);

	int GetPlayerIdx() const;
	Rgba8 GetDisplayTint() const;

private:
	int m_playerIdx = 0;
	Rgba8 m_displayTint = Rgba8::WHITE;
};
