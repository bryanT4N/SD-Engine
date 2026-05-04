#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"

class Camera
{
public:
	enum Mode
	{
		eMode_Orthographic,
		eMode_Perspective,

		eMode_Count
	};

public:
	void SetOrthographicView(
		Vec2 const& bottomLeft,
		Vec2 const& topRight,
		float near = 0.0f,
		float far = 1.0f);
	void SetPerspectiveView(float aspect, float fov, float near, float far);

	void SetPositionAndOrientation(Vec3 const& position, EulerAngles const& orientation);
	void SetPosition(Vec3 const& position);
	Vec3 GetPosition() const;
	void SetOrientation(EulerAngles const& orientation);
	EulerAngles GetOrientation() const;

	Mat44 GetCameraToWorldTransform() const;
	Mat44 GetWorldToCameraTransform() const;

	void SetCameraToRenderTransform(Mat44 const& m);
	Mat44 GetCameraToRenderTransform() const;

	Mat44 GetRenderToClipTransform() const;

	Vec2 GetOrthographicBottomLeft() const;
	Vec2 GetOrthographicTopRight() const;
	void Translate2D(Vec2 const& translation);

	Mat44 GetOrthographicMatrix() const;
	Mat44 GetPerspectiveMatrix() const;
	Mat44 GetProjectionMatrix() const;

	void SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight);
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;

	void SetNormalizedViewport(AABB2 const& normalizedViewport);
	AABB2 GetNormalizedViewport() const;

protected:
	Mode m_mode = eMode_Orthographic;

	Vec3 m_position;
	EulerAngles m_orientation;

	Vec2 m_orthographicBottomLeft = Vec2(0.f, 0.f);
	Vec2 m_orthographicTopRight = Vec2(1.f, 1.f);
	float m_orthographicNear = 0.f;
	float m_orthographicFar = 1.f;

	float m_perspectiveAspect = 1.f;
	float m_perspectiveFOV = 60.f;
	float m_perspectiveNear = 0.1f;
	float m_perspectiveFar = 100.f;

	Mat44 m_cameraToRenderTransform;

	AABB2 m_normalizedViewport = AABB2(0.f, 0.f, 1.f, 1.f);
};