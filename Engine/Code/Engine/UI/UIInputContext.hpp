#pragma once
#include "Engine/Math/Vec2.hpp"

//-----------------------------------------------------------------------------------------------
class UIWidget;

//-----------------------------------------------------------------------------------------------
struct UIInputContext
{
	UIWidget*	capture			= nullptr;
	UIWidget*	focus			= nullptr;
	UIWidget*	hovered			= nullptr;
	Vec2		lastMousePos;
};
