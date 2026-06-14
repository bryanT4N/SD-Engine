#include "Engine/UI/UIAnchorContainer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void UIAnchorContainer::AddChild(std::unique_ptr<UIWidget> child, UIAnchorLayout const& layout)
{
	if (child == nullptr) {
		DebuggerPrintf("WARN UIAnchorContainer::AddChild: child is nullptr, ignoring\n");
		return;
	}
	child->SetParent(this);
	m_slots.push_back(Slot{ std::move(child), layout });
}

//-----------------------------------------------------------------------------------------------
UIWidget* UIAnchorContainer::GetChildWidget(int index) const
{
	if (index < 0 || index >= static_cast<int>(m_slots.size())) {
		return nullptr;
	}
	return m_slots[index].widget.get();
}

//-----------------------------------------------------------------------------------------------
UIAnchorLayout const& UIAnchorContainer::GetSlotLayout(int slotIndex) const
{
	return m_slots[slotIndex].layout;
}

//-----------------------------------------------------------------------------------------------
void UIAnchorContainer::UpdateLayout(AABB2 const& parentBounds)
{
	UIWidget::UpdateLayout(parentBounds);

	for (Slot& slot : m_slots) {
		if (slot.widget == nullptr) continue;
		if (slot.widget->GetVisibility() == EUIVisibility::COLLAPSED) continue;

		Vec2 desiredSize = slot.layout.autoSize ? slot.widget->ComputeDesiredSize() : Vec2::ZERO;
		AABB2 childBounds = slot.layout.ComputeAbsoluteBounds(m_screenBounds, desiredSize);
		slot.widget->UpdateLayout(childBounds);
	}
}
