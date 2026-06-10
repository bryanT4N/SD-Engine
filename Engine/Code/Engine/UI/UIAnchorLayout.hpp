#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"

//-----------------------------------------------------------------------------------------------
struct UIAnchorLayout
{
	Vec2	anchorMin	= Vec2(0.f, 0.f);
	Vec2	anchorMax	= Vec2(0.f, 0.f);
	Vec2	pivot		= Vec2(0.5f, 0.5f);
	Vec2	sizeDelta	= Vec2(100.f, 30.f);
	Vec2	offsetMin	= Vec2(0.f, 0.f);
	Vec2	offsetMax	= Vec2(0.f, 0.f);
	bool	autoSize	= false;

	AABB2	ComputeAbsoluteBounds(AABB2 const& parentBounds, Vec2 const& desiredSize) const;

	static UIAnchorLayout	Centered(float widthPx = 100.f, float heightPx = 30.f);
	static UIAnchorLayout	AnchoredTopLeft(float xPx, float yPx, float widthPx, float heightPx);
	static UIAnchorLayout	FullStretch(float marginL = 0.f, float marginT = 0.f, float marginR = 0.f, float marginB = 0.f);
};
