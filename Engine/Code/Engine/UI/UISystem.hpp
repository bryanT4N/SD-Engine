#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/UI/UIEventReply.hpp"
#include "Engine/UI/UIInputContext.hpp"

#include <memory>

//-----------------------------------------------------------------------------------------------
class Renderer;
class UIWidget;

//-----------------------------------------------------------------------------------------------
class UISystem
{
public:
	UISystem();
	explicit UISystem(AABB2 const& screenBounds);
	~UISystem();

	void			SetRoot(std::unique_ptr<UIWidget> root);
	void			SetScreenBounds(AABB2 const& bounds) { m_screenBounds = bounds; }
	UIWidget*		GetRoot() const { return m_root.get(); }
	AABB2 const&	GetScreenBounds() const { return m_screenBounds; }

	void			Update();
	void			Render(Renderer& renderer) const;

	void			OnMouseMove(Vec2 const& screenPos);
	void			OnMouseDown(Vec2 const& screenPos);
	void			OnMouseUp(Vec2 const& screenPos);
	void			OnKeyDown(int keyCode);

	UIInputContext const&	GetInput() const { return m_input; }

	// Call before destroying a widget that may still be referenced by capture/focus/hovered.
	void					NotifyWidgetRemoved(UIWidget* widget);

private:
	void			RouteMouseMove(UIInputContext& ctx, Vec2 const& screenPos);
	void			RouteMouseDown(UIInputContext& ctx, Vec2 const& screenPos);
	void			RouteMouseUp(UIInputContext& ctx, Vec2 const& screenPos);
	void			RouteKeyDown(UIInputContext& ctx, int keyCode);
	void			ApplyReply(UIInputContext& ctx, UIEventReply const& reply);

	std::unique_ptr<UIWidget>	m_root;
	AABB2						m_screenBounds;
	UIInputContext				m_input;
};
