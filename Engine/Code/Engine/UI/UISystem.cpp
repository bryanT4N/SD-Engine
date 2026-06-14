#include "Engine/UI/UISystem.hpp"
#include "Engine/UI/UIWidget.hpp"

#include <vector>

//-----------------------------------------------------------------------------------------------
UISystem::UISystem() = default;

//-----------------------------------------------------------------------------------------------
UISystem::UISystem(AABB2 const& screenBounds)
	: m_screenBounds(screenBounds)
{
}

//-----------------------------------------------------------------------------------------------
UISystem::~UISystem() = default;

//-----------------------------------------------------------------------------------------------
void UISystem::SetRoot(std::unique_ptr<UIWidget> root)
{
	m_input.capture = nullptr;
	m_input.focus = nullptr;
	m_input.hovered = nullptr;
	m_root = std::move(root);
}

//-----------------------------------------------------------------------------------------------
void UISystem::Update()
{
	if (m_root == nullptr) return;
	m_root->UpdateLayout(m_screenBounds);
}

//-----------------------------------------------------------------------------------------------
void UISystem::Render(Renderer& renderer) const
{
	if (m_root == nullptr) return;
	m_root->Render(renderer);
}

//-----------------------------------------------------------------------------------------------
void UISystem::NotifyWidgetRemoved(UIWidget* widget)
{
	if (m_input.capture == widget) m_input.capture = nullptr;
	if (m_input.focus == widget) m_input.focus = nullptr;
	if (m_input.hovered == widget) m_input.hovered = nullptr;
}

//-----------------------------------------------------------------------------------------------
void UISystem::OnMouseMove(Vec2 const& screenPos)
{
	RouteMouseMove(m_input, screenPos);
}

//-----------------------------------------------------------------------------------------------
void UISystem::OnMouseDown(Vec2 const& screenPos)
{
	RouteMouseDown(m_input, screenPos);
}

//-----------------------------------------------------------------------------------------------
void UISystem::OnMouseUp(Vec2 const& screenPos)
{
	RouteMouseUp(m_input, screenPos);
}

//-----------------------------------------------------------------------------------------------
void UISystem::OnKeyDown(int keyCode)
{
	RouteKeyDown(m_input, keyCode);
}

//-----------------------------------------------------------------------------------------------
void UISystem::RouteMouseMove(UIInputContext& ctx, Vec2 const& screenPos)
{
	ctx.lastMousePos = screenPos;

	if (ctx.capture != nullptr) {
		ApplyReply(ctx, ctx.capture->OnMouseMove(screenPos));
		return;
	}

	UIWidget* newHovered = nullptr;
	if (m_root != nullptr) {
		std::vector<UIWidget*> path;
		m_root->BuildPathToPoint(screenPos, path);
		if (!path.empty()) {
			newHovered = path.back();
		}
	}

	if (newHovered != ctx.hovered) {
		if (ctx.hovered != nullptr) {
			ApplyReply(ctx, ctx.hovered->OnMouseLeave());
		}
		ctx.hovered = newHovered;
		if (newHovered != nullptr) {
			ApplyReply(ctx, newHovered->OnMouseEnter());
		}
	}
}

//-----------------------------------------------------------------------------------------------
void UISystem::RouteMouseDown(UIInputContext& ctx, Vec2 const& screenPos)
{
	if (ctx.capture != nullptr) {
		ApplyReply(ctx, ctx.capture->OnMouseDown(screenPos));
		return;
	}

	if (m_root == nullptr) return;

	std::vector<UIWidget*> path;
	m_root->BuildPathToPoint(screenPos, path);

	for (auto it = path.rbegin(); it != path.rend(); ++it) {
		UIEventReply reply = (*it)->OnMouseDown(screenPos);
		ApplyReply(ctx, reply);
		if (reply.handled) return;
	}
}

//-----------------------------------------------------------------------------------------------
void UISystem::RouteMouseUp(UIInputContext& ctx, Vec2 const& screenPos)
{
	if (ctx.capture != nullptr) {
		ApplyReply(ctx, ctx.capture->OnMouseUp(screenPos));
		return;
	}

	if (m_root == nullptr) return;

	std::vector<UIWidget*> path;
	m_root->BuildPathToPoint(screenPos, path);

	for (auto it = path.rbegin(); it != path.rend(); ++it) {
		UIEventReply reply = (*it)->OnMouseUp(screenPos);
		ApplyReply(ctx, reply);
		if (reply.handled) return;
	}
}

//-----------------------------------------------------------------------------------------------
void UISystem::RouteKeyDown(UIInputContext& ctx, int keyCode)
{
	for (UIWidget* widget = ctx.focus; widget != nullptr; widget = widget->GetParent()) {
		UIEventReply reply = widget->OnKeyDown(keyCode);
		ApplyReply(ctx, reply);
		if (reply.handled) return;
	}
}

//-----------------------------------------------------------------------------------------------
void UISystem::ApplyReply(UIInputContext& ctx, UIEventReply const& reply)
{
	if (reply.requestCapture != nullptr) {
		ctx.capture = reply.requestCapture;
	}
	if (reply.releaseCapture) {
		ctx.capture = nullptr;
	}
	if (reply.requestFocus != nullptr && ctx.focus != reply.requestFocus) {
		if (ctx.focus != nullptr) {
			ctx.focus->OnFocusLost();
		}
		ctx.focus = reply.requestFocus;
		reply.requestFocus->OnFocusGained();
	}
	if (reply.releaseFocus) {
		if (ctx.focus != nullptr) {
			ctx.focus->OnFocusLost();
		}
		ctx.focus = nullptr;
	}
}
