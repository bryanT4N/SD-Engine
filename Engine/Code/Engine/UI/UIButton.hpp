#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/UI/UIAnchorContainer.hpp"

#include <functional>

//-----------------------------------------------------------------------------------------------
enum class EUIWidgetState : unsigned char
{
	NORMAL,
	HOVER,
	PRESSED,
	DISABLED
};

//-----------------------------------------------------------------------------------------------
class UIButton : public UIAnchorContainer
{
public:
	UIButton() = default;
	~UIButton() override = default;

	void			Render(Renderer& renderer) const override;
	UIEventReply	OnMouseEnter() override;
	UIEventReply	OnMouseLeave() override;
	UIEventReply	OnMouseDown(Vec2 const& screenPos) override;
	UIEventReply	OnMouseUp(Vec2 const& screenPos) override;
	void			AddChild(std::unique_ptr<UIWidget> child, UIAnchorLayout const& layout) override;

public:
	std::function<void()>	m_onClick;
	EUIWidgetState			m_state = EUIWidgetState::NORMAL;
	Rgba8					m_colorByState[4] = {
		Rgba8(60, 60, 70, 255),
		Rgba8(90, 90, 110, 255),
		Rgba8(110, 110, 130, 255),
		Rgba8(40, 40, 40, 128)
	};
};
