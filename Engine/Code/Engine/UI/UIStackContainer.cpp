#include "Engine/UI/UIStackContainer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <algorithm>

//-----------------------------------------------------------------------------------------------
void UIStackContainer::AddChild(std::unique_ptr<UIWidget> child)
{
	if (child == nullptr) {
		DebuggerPrintf("WARN UIStackContainer::AddChild: child is nullptr, ignoring\n");
		return;
	}
	child->SetParent(this);
	m_slots.push_back(Slot{ std::move(child) });
}

//-----------------------------------------------------------------------------------------------
UIWidget* UIStackContainer::GetChildWidget(int index) const
{
	if (index < 0 || index >= static_cast<int>(m_slots.size())) {
		return nullptr;
	}
	return m_slots[index].widget.get();
}

//-----------------------------------------------------------------------------------------------
Vec2 UIStackContainer::ComputeDesiredSize() const
{
	Vec2 total = Vec2::ZERO;
	int visibleCount = 0;
	for (Slot const& slot : m_slots) {
		if (slot.widget == nullptr) continue;
		if (slot.widget->GetVisibility() == EUIVisibility::COLLAPSED) continue;
		Vec2 childSize = slot.widget->ComputeDesiredSize();
		if (m_orientation == EUIOrientation::HORIZONTAL) {
			total.x += childSize.x;
			total.y = std::max(total.y, childSize.y);
		} else {
			total.y += childSize.y;
			total.x = std::max(total.x, childSize.x);
		}
		++visibleCount;
	}
	if (visibleCount > 1) {
		float gaps = m_spacing * static_cast<float>(visibleCount - 1);
		if (m_orientation == EUIOrientation::HORIZONTAL) {
			total.x += gaps;
		} else {
			total.y += gaps;
		}
	}
	return total;
}

//-----------------------------------------------------------------------------------------------
void UIStackContainer::UpdateLayout(AABB2 const& parentBounds)
{
	UIWidget::UpdateLayout(parentBounds);

	float mainAxisCursor = (m_orientation == EUIOrientation::HORIZONTAL)
		? parentBounds.m_mins.x
		: parentBounds.m_maxs.y;
	bool isFirstVisible = true;

	for (Slot& slot : m_slots) {
		if (slot.widget == nullptr) continue;
		if (slot.widget->GetVisibility() == EUIVisibility::COLLAPSED) continue;

		if (!isFirstVisible) {
			if (m_orientation == EUIOrientation::HORIZONTAL) {
				mainAxisCursor += m_spacing;
			} else {
				mainAxisCursor -= m_spacing;
			}
		}
		isFirstVisible = false;

		Vec2 childDesired = slot.widget->ComputeDesiredSize();
		AABB2 childBounds;
		if (m_orientation == EUIOrientation::HORIZONTAL) {
			childBounds.m_mins.x = mainAxisCursor;
			childBounds.m_maxs.x = mainAxisCursor + childDesired.x;
			childBounds.m_mins.y = parentBounds.m_mins.y;
			childBounds.m_maxs.y = parentBounds.m_maxs.y;
			mainAxisCursor += childDesired.x;
		} else {
			childBounds.m_maxs.y = mainAxisCursor;
			childBounds.m_mins.y = mainAxisCursor - childDesired.y;
			childBounds.m_mins.x = parentBounds.m_mins.x;
			childBounds.m_maxs.x = parentBounds.m_maxs.x;
			mainAxisCursor -= childDesired.y;
		}

		slot.widget->UpdateLayout(childBounds);
	}
}
