#pragma once
#include "Engine/Core/Rgba8.hpp"

//-----------------------------------------------------------------------------------------------
class BitmapFont;

//-----------------------------------------------------------------------------------------------
struct UITheme
{
	BitmapFont*	defaultFont			= nullptr;
	float		defaultTextHeight	= 16.f;
	Rgba8		textColor			= Rgba8::WHITE;
	Rgba8		bgPanel				= Rgba8(50, 50, 50, 220);
	Rgba8		buttonNormal		= Rgba8(60, 60, 70, 255);
	Rgba8		buttonHover			= Rgba8(90, 90, 110, 255);
	Rgba8		buttonPressed		= Rgba8(110, 110, 130, 255);
	Rgba8		buttonDisabled		= Rgba8(40, 40, 40, 128);
};
