#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/UI/UIWidget.hpp"

#include <string>

//-----------------------------------------------------------------------------------------------
class BitmapFont;

//-----------------------------------------------------------------------------------------------
class UILabel : public UIWidget
{
public:
	UILabel() = default;
	~UILabel() override = default;

	Vec2	ComputeDesiredSize() const override;
	void	Render(Renderer& renderer) const override;

public:
	std::string	m_text;
	float		m_textHeight = 16.f;
	Rgba8		m_color = Rgba8::WHITE;
	BitmapFont*	m_font = nullptr;
};
