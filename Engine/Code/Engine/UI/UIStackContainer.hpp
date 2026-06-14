#pragma once
#include "Engine/UI/UIContainer.hpp"

#include <memory>
#include <vector>

//-----------------------------------------------------------------------------------------------
enum class EUIOrientation : unsigned char
{
	HORIZONTAL,
	VERTICAL
};

//-----------------------------------------------------------------------------------------------
class UIStackContainer : public UIContainer
{
public:
	UIStackContainer() = default;
	~UIStackContainer() override = default;

	void			AddChild(std::unique_ptr<UIWidget> child);
	int				GetSlotCount() const { return static_cast<int>(m_slots.size()); }

	int				GetChildCount() const override { return static_cast<int>(m_slots.size()); }
	UIWidget*		GetChildWidget(int index) const override;
	Vec2			ComputeDesiredSize() const override;
	void			UpdateLayout(AABB2 const& parentBounds) override;

public:
	EUIOrientation	m_orientation	= EUIOrientation::HORIZONTAL;
	float			m_spacing		= 0.f;

private:
	struct Slot
	{
		std::unique_ptr<UIWidget>	widget;
	};

	std::vector<Slot>	m_slots;
};
