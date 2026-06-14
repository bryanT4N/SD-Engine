#pragma once
#include "Engine/UI/UIAnchorLayout.hpp"
#include "Engine/UI/UIContainer.hpp"

#include <memory>
#include <vector>

//-----------------------------------------------------------------------------------------------
class UIAnchorContainer : public UIContainer
{
public:
	UIAnchorContainer() = default;
	~UIAnchorContainer() override = default;

	virtual void			AddChild(std::unique_ptr<UIWidget> child, UIAnchorLayout const& layout);
	int						GetSlotCount() const { return static_cast<int>(m_slots.size()); }
	UIAnchorLayout const&	GetSlotLayout(int slotIndex) const;

	int						GetChildCount() const override { return static_cast<int>(m_slots.size()); }
	UIWidget*				GetChildWidget(int index) const override;
	void					UpdateLayout(AABB2 const& parentBounds) override;

private:
	struct Slot
	{
		std::unique_ptr<UIWidget>	widget;
		UIAnchorLayout				layout;
	};

	std::vector<Slot>	m_slots;
};
