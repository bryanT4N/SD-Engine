#include "Engine/Renderer/Camera.hpp"

void Camera::SetOrthographicView(
	Vec2 const& bottomLeft,
	Vec2 const& topRight,
	float near,
	float far)
{
	m_mode = eMode_Orthographic;
	m_orthographicBottomLeft = bottomLeft;
	m_orthographicTopRight = topRight;
	m_orthographicNear = near;
	m_orthographicFar = far;
}

void Camera::SetPerspectiveView(float aspect, float fov, float near, float far)
{
	m_mode = eMode_Perspective;
	m_perspectiveAspect = aspect;
	m_perspectiveFOV = fov;
	m_perspectiveNear = near;
	m_perspectiveFar = far;
}

void Camera::SetPositionAndOrientation(Vec3 const& position, EulerAngles const& orientation)
{
	m_position = position;
	m_orientation = orientation;
}

void Camera::SetPosition(Vec3 const& position)
{
	m_position = position;
}

Vec3 Camera::GetPosition() const
{
	return m_position;
}

void Camera::SetOrientation(EulerAngles const& orientation)
{
	m_orientation = orientation;
}

EulerAngles Camera::GetOrientation() const
{
	return m_orientation;
}

Mat44 Camera::GetCameraToWorldTransform() const
{
	Mat44 cameraToWorld = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	cameraToWorld.SetTranslation3D(m_position);
	return cameraToWorld;
}

Mat44 Camera::GetWorldToCameraTransform() const
{
	return GetCameraToWorldTransform().GetOrthonormalInverse();
}

void Camera::SetCameraToRenderTransform(Mat44 const& m)
{
	m_cameraToRenderTransform = m;
}

Mat44 Camera::GetCameraToRenderTransform() const
{
	return m_cameraToRenderTransform;
}

Mat44 Camera::GetRenderToClipTransform() const
{
	return GetProjectionMatrix();
}

Vec2 Camera::GetOrthographicBottomLeft() const
{
	return m_orthographicBottomLeft;
}

Vec2 Camera::GetOrthographicTopRight() const
{
	return m_orthographicTopRight;
}

void Camera::Translate2D(Vec2 const& translation)
{
	m_orthographicBottomLeft += translation;
	m_orthographicTopRight += translation;
}

Mat44 Camera::GetOrthographicMatrix() const
{
	return Mat44::MakeOrthoProjection(
		m_orthographicBottomLeft.x,
		m_orthographicTopRight.x,
		m_orthographicBottomLeft.y,
		m_orthographicTopRight.y,
		m_orthographicNear,
		m_orthographicFar);
}

Mat44 Camera::GetPerspectiveMatrix() const
{
	return Mat44::MakePerspectiveProjection(
		m_perspectiveFOV,
		m_perspectiveAspect,
		m_perspectiveNear,
		m_perspectiveFar);
}

Mat44 Camera::GetProjectionMatrix() const
{
	if (m_mode == eMode_Perspective) {
		return GetPerspectiveMatrix();
	}

	return GetOrthographicMatrix();
}

void Camera::SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight)
{
	SetOrthographicView(bottomLeft, topRight);
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return GetOrthographicBottomLeft();
}

Vec2 Camera::GetOrthoTopRight() const
{
	return GetOrthographicTopRight();
}

void Camera::SetNormalizedViewport(AABB2 const& normalizedViewport)
{
	m_normalizedViewport = normalizedViewport;
}

AABB2 Camera::GetNormalizedViewport() const
{
	return m_normalizedViewport;
}
