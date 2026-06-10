#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/UI/UIEventReply.hpp"

#include <vector>

//-----------------------------------------------------------------------------------------------
class Renderer;

//-----------------------------------------------------------------------------------------------
enum class EUIVisibility : unsigned char
{
	VISIBLE,
	HIDDEN,
	COLLAPSED
};

//-----------------------------------------------------------------------------------------------
class UIWidget
{
public:
	UIWidget() = default;
	virtual ~UIWidget() = default;
	UIWidget(UIWidget const&) = delete;
	UIWidget& operator=(UIWidget const&) = delete;

	virtual Vec2			ComputeDesiredSize() const;
	virtual void			UpdateLayout(AABB2 const& parentBounds);

	virtual void			Render(Renderer& renderer) const = 0;

	virtual void			BuildPathToPoint(Vec2 const& screenPos, std::vector<UIWidget*>& outPath);

	virtual UIEventReply	OnMouseEnter();
	virtual UIEventReply	OnMouseLeave();
	virtual UIEventReply	OnMouseMove(Vec2 const& screenPos);
	virtual UIEventReply	OnMouseDown(Vec2 const& screenPos);
	virtual UIEventReply	OnMouseUp(Vec2 const& screenPos);

	virtual UIEventReply	OnKeyDown(int keyCode);
	virtual void			OnFocusGained() {}
	virtual void			OnFocusLost() {}

	EUIVisibility			GetVisibility() const { return m_visibility; }
	void					SetVisibility(EUIVisibility visibility) { m_visibility = visibility; }
	bool					IsVisible() const { return m_visibility == EUIVisibility::VISIBLE; }

	AABB2 const&			GetScreenBounds() const { return m_screenBounds; }
	UIWidget*				GetParent() const { return m_parent; }
	void					SetParent(UIWidget* parent) { m_parent = parent; }

public:
	AABB2			m_screenBounds;
	EUIVisibility	m_visibility			= EUIVisibility::VISIBLE;
	UIWidget*		m_parent				= nullptr;
	bool			m_isHitTestInvisible	= false;
};
