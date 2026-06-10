#include "Engine/UI/UIAnchorLayout.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <algorithm>

//-----------------------------------------------------------------------------------------------
static float ClampUnit(float value)
{
	if (value < 0.f) return 0.f;
	if (value > 1.f) return 1.f;
	return value;
}

//-----------------------------------------------------------------------------------------------
AABB2 UIAnchorLayout::ComputeAbsoluteBounds(AABB2 const& parentBounds, Vec2 const& desiredSize) const
{
	Vec2 sanitizedAnchorMin = anchorMin;
	Vec2 sanitizedAnchorMax = anchorMax;
	if (anchorMin.x > anchorMax.x) {
		DebuggerPrintf("WARN UIAnchorLayout: anchorMin.x > anchorMax.x, auto-swap\n");
		std::swap(sanitizedAnchorMin.x, sanitizedAnchorMax.x);
	}
	if (anchorMin.y > anchorMax.y) {
		DebuggerPrintf("WARN UIAnchorLayout: anchorMin.y > anchorMax.y, auto-swap\n");
		std::swap(sanitizedAnchorMin.y, sanitizedAnchorMax.y);
	}

	Vec2 sanitizedPivot = pivot;
	if (pivot.x < 0.f || pivot.x > 1.f || pivot.y < 0.f || pivot.y > 1.f) {
		DebuggerPrintf("WARN UIAnchorLayout: pivot out of [0,1], clamp\n");
		sanitizedPivot.x = ClampUnit(pivot.x);
		sanitizedPivot.y = ClampUnit(pivot.y);
	}

	bool isHorizontalStretch = (sanitizedAnchorMin.x != sanitizedAnchorMax.x);
	bool isVerticalStretch = (sanitizedAnchorMin.y != sanitizedAnchorMax.y);

	if (autoSize && (isHorizontalStretch || isVerticalStretch)) {
		DebuggerPrintf("WARN UIAnchorLayout: autoSize+stretch conflict, stretch wins\n");
	}

	Vec2 parentSize = parentBounds.GetDimensions();

	Vec2 anchorPxMin = Vec2(
		sanitizedAnchorMin.x * parentSize.x + parentBounds.m_mins.x,
		sanitizedAnchorMin.y * parentSize.y + parentBounds.m_mins.y);
	Vec2 anchorPxMax = Vec2(
		sanitizedAnchorMax.x * parentSize.x + parentBounds.m_mins.x,
		sanitizedAnchorMax.y * parentSize.y + parentBounds.m_mins.y);

	Vec2 widgetSize = autoSize ? desiredSize : sizeDelta;
	Vec2 pivotOffset = Vec2(widgetSize.x * sanitizedPivot.x, widgetSize.y * sanitizedPivot.y);

	AABB2 result;
	if (isHorizontalStretch) {
		result.m_mins.x = anchorPxMin.x + offsetMin.x;
		result.m_maxs.x = anchorPxMax.x - offsetMax.x;
	} else {
		result.m_mins.x = anchorPxMin.x + offsetMin.x - pivotOffset.x;
		result.m_maxs.x = result.m_mins.x + widgetSize.x;
	}

	if (isVerticalStretch) {
		result.m_mins.y = anchorPxMin.y + offsetMin.y;
		result.m_maxs.y = anchorPxMax.y - offsetMax.y;
	} else {
		result.m_mins.y = anchorPxMin.y + offsetMin.y - pivotOffset.y;
		result.m_maxs.y = result.m_mins.y + widgetSize.y;
	}

	return result;
}

//-----------------------------------------------------------------------------------------------
UIAnchorLayout UIAnchorLayout::Centered(float widthPx, float heightPx)
{
	UIAnchorLayout layout;
	layout.anchorMin = Vec2(0.5f, 0.5f);
	layout.anchorMax = Vec2(0.5f, 0.5f);
	layout.pivot = Vec2(0.5f, 0.5f);
	layout.sizeDelta = Vec2(widthPx, heightPx);
	layout.offsetMin = Vec2::ZERO;
	layout.offsetMax = Vec2::ZERO;
	layout.autoSize = false;
	return layout;
}

//-----------------------------------------------------------------------------------------------
UIAnchorLayout UIAnchorLayout::AnchoredTopLeft(float xPx, float yPx, float widthPx, float heightPx)
{
	UIAnchorLayout layout;
	layout.anchorMin = Vec2(0.f, 0.f);
	layout.anchorMax = Vec2(0.f, 0.f);
	layout.pivot = Vec2(0.f, 0.f);
	layout.sizeDelta = Vec2(widthPx, heightPx);
	layout.offsetMin = Vec2(xPx, yPx);
	layout.offsetMax = Vec2::ZERO;
	layout.autoSize = false;
	return layout;
}

//-----------------------------------------------------------------------------------------------
UIAnchorLayout UIAnchorLayout::FullStretch(float marginL, float marginT, float marginR, float marginB)
{
	UIAnchorLayout layout;
	layout.anchorMin = Vec2(0.f, 0.f);
	layout.anchorMax = Vec2(1.f, 1.f);
	layout.pivot = Vec2(0.f, 0.f);
	layout.sizeDelta = Vec2::ZERO;
	layout.offsetMin = Vec2(marginL, marginT);
	layout.offsetMax = Vec2(marginR, marginB);
	layout.autoSize = false;
	return layout;
}
