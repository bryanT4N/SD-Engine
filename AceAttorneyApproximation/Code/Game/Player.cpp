#include "Game/Player.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Window/Window.hpp"

static float constexpr PLAYER_DEFAULT_FOV_DEGREES = 60.f;
static float constexpr PLAYER_DEFAULT_NEAR_CLIP = 0.1f;
static float constexpr PLAYER_DEFAULT_FAR_CLIP = 100.f;

static float GetPlayerConfigFloat(char const* key, float defaultValue)
{
	return g_gameConfigBlackboard.GetValue(key, defaultValue);
}

Player::Player(Game* owner)
	: Entity(owner)
{
	float cameraAspect = 2.f;
	if (g_engine != nullptr && g_engine->m_window != nullptr) {
		IntVec2 clientDimensions = g_engine->m_window->GetClientDimensions();
		if (clientDimensions.y > 0) {
			cameraAspect =
				static_cast<float>(clientDimensions.x) /
				static_cast<float>(clientDimensions.y);
		}
	}

	m_camera.SetPerspectiveView(
		cameraAspect,
		GetPlayerConfigFloat("playerDefaultFovDegrees", PLAYER_DEFAULT_FOV_DEGREES),
		GetPlayerConfigFloat("playerDefaultNearClip", PLAYER_DEFAULT_NEAR_CLIP),
		GetPlayerConfigFloat("playerDefaultFarClip", PLAYER_DEFAULT_FAR_CLIP));
	Mat44 cameraToRenderTransform(
		Vec3(0.f, 0.f, 1.f),
		Vec3(-1.f, 0.f, 0.f),
		Vec3(0.f, 1.f, 0.f),
		Vec3(0.f, 0.f, 0.f));
	m_camera.SetCameraToRenderTransform(cameraToRenderTransform);
	ResetPose();
}

Player::~Player()
{
}

void Player::Update([[maybe_unused]] float deltaSeconds)
{
	UpdateCameraFromTransform();
}

void Player::Render() const
{
}

Camera const& Player::GetCamera() const
{
	return m_camera;
}

void Player::ResetPose()
{
	m_position = Vec3(0.f, 0.f, 0.f);
	m_velocity = Vec3(0.f, 0.f, 0.f);
	m_orientation = EulerAngles(0.f, 0.f, 0.f);
	m_angularVelocity = EulerAngles(0.f, 0.f, 0.f);
	UpdateCameraFromTransform();
}

void Player::UpdateCameraFromTransform()
{
	m_camera.SetPositionAndOrientation(m_position, m_orientation);
}
