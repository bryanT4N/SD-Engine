#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/UI/UIAnchorContainer.hpp"

//-----------------------------------------------------------------------------------------------
class UIPanel : public UIAnchorContainer
{
public:
	UIPanel() = default;
	~UIPanel() override = default;

	void	Render(Renderer& renderer) const override;

public:
	Rgba8	m_bgColor = Rgba8(50, 50, 50, 220);
	Rgba8	m_borderColor = Rgba8::WHITE;
	float	m_borderThickness = 0.f;
};
