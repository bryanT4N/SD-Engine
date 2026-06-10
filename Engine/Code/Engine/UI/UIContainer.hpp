#pragma once
#include "Engine/UI/UIWidget.hpp"

//-----------------------------------------------------------------------------------------------
class UIContainer : public UIWidget
{
public:
	UIContainer() = default;
	~UIContainer() override = default;

	virtual int			GetChildCount() const = 0;
	virtual UIWidget*	GetChildWidget(int index) const = 0;

	void				Render(Renderer& renderer) const override;
	void				BuildPathToPoint(Vec2 const& screenPos, std::vector<UIWidget*>& outPath) override;
};
