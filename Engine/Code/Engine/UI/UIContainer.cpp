#include "Engine/UI/UIContainer.hpp"

//-----------------------------------------------------------------------------------------------
void UIContainer::Render(Renderer& renderer) const
{
	if (m_visibility != EUIVisibility::VISIBLE) return;

	int childCount = GetChildCount();
	for (int i = 0; i < childCount; ++i) {
		UIWidget* child = GetChildWidget(i);
		if (child == nullptr) continue;
		if (child->GetVisibility() != EUIVisibility::VISIBLE) continue;
		child->Render(renderer);
	}
}

//-----------------------------------------------------------------------------------------------
void UIContainer::BuildPathToPoint(Vec2 const& screenPos, std::vector<UIWidget*>& outPath)
{
	if (m_visibility != EUIVisibility::VISIBLE) return;
	if (!m_screenBounds.IsPointInside(screenPos)) return;

	if (!m_isHitTestInvisible) {
		outPath.push_back(this);
	}

	int childCount = GetChildCount();
	for (int i = childCount - 1; i >= 0; --i) {
		UIWidget* child = GetChildWidget(i);
		if (child == nullptr) continue;
		if (child->GetVisibility() == EUIVisibility::COLLAPSED) continue;
		size_t before = outPath.size();
		child->BuildPathToPoint(screenPos, outPath);
		if (outPath.size() != before) return;
	}
}
