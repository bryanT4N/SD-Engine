#include "Engine/UI/UIButton.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

//-----------------------------------------------------------------------------------------------
void UIButton::Render(Renderer& renderer) const
{
	if (m_visibility != EUIVisibility::VISIBLE) return;

	std::vector<Vertex> bgVerts;
	AddVertsForAABB2D(bgVerts, m_screenBounds, m_colorByState[static_cast<int>(m_state)]);

	renderer.SetBlendMode(BlendMode::ALPHA);
	renderer.SetSamplerMode(SamplerMode::POINT_CLAMP);
	renderer.SetDepthMode(DepthMode::DISABLED);
	renderer.SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	renderer.BindTexture(nullptr);
	renderer.DrawVertexArray(bgVerts);

	UIAnchorContainer::Render(renderer);
}

//-----------------------------------------------------------------------------------------------
void UIButton::AddChild(std::unique_ptr<UIWidget> child, UIAnchorLayout const& layout)
{
	if (child != nullptr) {
		child->m_isHitTestInvisible = true;
	}
	UIAnchorContainer::AddChild(std::move(child), layout);
}

//-----------------------------------------------------------------------------------------------
UIEventReply UIButton::OnMouseEnter()
{
	if (m_state == EUIWidgetState::DISABLED) return UIEventReply::Unhandled();
	if (m_state == EUIWidgetState::NORMAL) {
		m_state = EUIWidgetState::HOVER;
	}
	return UIEventReply::Handled();
}

//-----------------------------------------------------------------------------------------------
UIEventReply UIButton::OnMouseLeave()
{
	if (m_state == EUIWidgetState::DISABLED) return UIEventReply::Unhandled();
	if (m_state == EUIWidgetState::HOVER) {
		m_state = EUIWidgetState::NORMAL;
	}
	return UIEventReply::Handled();
}

//-----------------------------------------------------------------------------------------------
UIEventReply UIButton::OnMouseDown(Vec2 const& screenPos)
{
	(void)screenPos;
	if (m_state == EUIWidgetState::DISABLED) return UIEventReply::Unhandled();
	m_state = EUIWidgetState::PRESSED;
	return UIEventReply::Handled().CaptureMouse(this);
}

//-----------------------------------------------------------------------------------------------
UIEventReply UIButton::OnMouseUp(Vec2 const& screenPos)
{
	if (m_state == EUIWidgetState::DISABLED) return UIEventReply::Unhandled();
	if (m_state != EUIWidgetState::PRESSED) return UIEventReply::Unhandled();

	bool isInside = m_screenBounds.IsPointInside(screenPos);
	m_state = isInside ? EUIWidgetState::HOVER : EUIWidgetState::NORMAL;

	UIEventReply reply = UIEventReply::Handled().ReleaseMouseCapture();
	if (isInside && m_onClick) {
		m_onClick();
	}
	return reply;
}
