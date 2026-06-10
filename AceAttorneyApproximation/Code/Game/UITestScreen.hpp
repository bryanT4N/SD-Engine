#pragma once
#include "Engine/UI/UISystem.hpp"

//-----------------------------------------------------------------------------------------------
struct UITheme;
class Renderer;

//-----------------------------------------------------------------------------------------------
class UITestScreen
{
public:
	UITestScreen() = default;
	~UITestScreen() = default;

	void	Build(UITheme const* theme);
	void	Update();
	void	Render(Renderer& renderer) const;

private:
	UISystem	m_uiSystem;
};
