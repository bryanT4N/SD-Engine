#include "Engine/UI/UIWidget.hpp"

//-----------------------------------------------------------------------------------------------
Vec2 UIWidget::ComputeDesiredSize() const
{
	return Vec2::ZERO;
}

//-----------------------------------------------------------------------------------------------
void UIWidget::UpdateLayout(AABB2 const& parentBounds)
{
	m_screenBounds = parentBounds;
}

//-----------------------------------------------------------------------------------------------
void UIWidget::BuildPathToPoint(Vec2 const& screenPos, std::vector<UIWidget*>& outPath)
{
	if (m_visibility != EUIVisibility::VISIBLE) return;
	if (!m_screenBounds.IsPointInside(screenPos)) return;
	if (m_isHitTestInvisible) return;
	outPath.push_back(this);
}

//-----------------------------------------------------------------------------------------------
UIEventReply UIWidget::OnMouseEnter()
{
	return UIEventReply::Unhandled();
}

//-----------------------------------------------------------------------------------------------
UIEventReply UIWidget::OnMouseLeave()
{
	return UIEventReply::Unhandled();
}

//-----------------------------------------------------------------------------------------------
UIEventReply UIWidget::OnMouseMove(Vec2 const& screenPos)
{
	(void)screenPos;
	return UIEventReply::Unhandled();
}

//-----------------------------------------------------------------------------------------------
UIEventReply UIWidget::OnMouseDown(Vec2 const& screenPos)
{
	(void)screenPos;
	return UIEventReply::Unhandled();
}

//-----------------------------------------------------------------------------------------------
UIEventReply UIWidget::OnMouseUp(Vec2 const& screenPos)
{
	(void)screenPos;
	return UIEventReply::Unhandled();
}

//-----------------------------------------------------------------------------------------------
UIEventReply UIWidget::OnKeyDown(int keyCode)
{
	(void)keyCode;
	return UIEventReply::Unhandled();
}
