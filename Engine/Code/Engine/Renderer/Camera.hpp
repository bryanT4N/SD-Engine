#pragma once
#include "Engine/Math/Vec2.hpp"

class Camera {
private:
	Vec2 m_orthoBottomLeft;
	Vec2 m_orthoTopRight;

public:
	void SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight);
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	void Translate2D(Vec2 disp);
};