#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/UI/UIWidget.hpp"

//-----------------------------------------------------------------------------------------------
class Texture;

//-----------------------------------------------------------------------------------------------
class UIImage : public UIWidget
{
public:
	UIImage() = default;
	~UIImage() override = default;

	Vec2	ComputeDesiredSize() const override;
	void	Render(Renderer& renderer) const override;

public:
	Texture*	m_texture = nullptr;
	AABB2		m_uvRect = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f));
	Rgba8		m_tint = Rgba8::WHITE;
};
