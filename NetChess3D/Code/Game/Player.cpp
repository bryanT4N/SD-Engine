#include "Game/Player.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Window/Window.hpp"

static float constexpr PLAYER_MOVE_SPEED = 5.f;
static float constexpr PLAYER_SPRINT_MULTIPLIER = 10.f;
static float constexpr PLAYER_ROLL_SPEED_DEGREES = 90.f;
static float constexpr PLAYER_GAMEPAD_LOOK_SPEED_DEGREES = 90.f;
static float constexpr PLAYER_MOUSE_LOOK_DEGREES_PER_PIXEL = 0.125f;
static float constexpr PLAYER_MAX_PITCH_DEGREES = 85.f;
static float constexpr PLAYER_MAX_ROLL_DEGREES = 45.f;
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
	if (!m_isInputEnabled) {
		UpdateCameraFromTransform();
		return;
	}

	if (g_engine == nullptr || g_engine->m_input == nullptr) {
		UpdateCameraFromTransform();
		return;
	}

	float deltaSecondsFromSystemClock =
		static_cast<float>(Clock::GetSystemClock().GetDeltaSeconds());
	if (deltaSecondsFromSystemClock <= 0.f) {
		UpdateCameraFromTransform();
		return;
	}

	InputSystem* inputSystem = g_engine->m_input;
	XboxController const& controller = inputSystem->GetController(0);
	float const playerMoveSpeed = GetPlayerConfigFloat("playerMoveSpeed", PLAYER_MOVE_SPEED);
	float const playerSprintMultiplier =
		GetPlayerConfigFloat("playerSprintMultiplier", PLAYER_SPRINT_MULTIPLIER);
	float const playerRollSpeedDegrees =
		GetPlayerConfigFloat("playerRollSpeedDegrees", PLAYER_ROLL_SPEED_DEGREES);
	float const playerGamepadLookSpeedDegrees =
		GetPlayerConfigFloat("playerGamepadLookSpeedDegrees", PLAYER_GAMEPAD_LOOK_SPEED_DEGREES);
	float const playerMouseLookDegreesPerPixel =
		GetPlayerConfigFloat("playerMouseLookDegreesPerPixel", PLAYER_MOUSE_LOOK_DEGREES_PER_PIXEL);
	float const playerMaxPitchDegrees =
		GetPlayerConfigFloat("playerMaxPitchDegrees", PLAYER_MAX_PITCH_DEGREES);
	float const playerMaxRollDegrees =
		GetPlayerConfigFloat("playerMaxRollDegrees", PLAYER_MAX_ROLL_DEGREES);

	if (inputSystem->WasKeyJustPressed('H') ||
		controller.WasButtonJustPressed(XboxButtonID::START)) {
		ResetPose();
		return;
	}

	float forwardInput = 0.f;
	if (inputSystem->IsKeyDown('W')) {
		forwardInput += 1.f;
	}
	if (inputSystem->IsKeyDown('S')) {
		forwardInput -= 1.f;
	}

	float leftInput = 0.f;
	if (inputSystem->IsKeyDown('A')) {
		leftInput += 1.f;
	}
	if (inputSystem->IsKeyDown('D')) {
		leftInput -= 1.f;
	}

	float verticalInput = 0.f;
	if (inputSystem->IsKeyDown('C')) {
		verticalInput += 1.f;
	}
	if (inputSystem->IsKeyDown('Z')) {
		verticalInput -= 1.f;
	}

	Vec2 leftStickPosition = controller.GetLeftStick().GetPosition();
	forwardInput += leftStickPosition.y;
	leftInput -= leftStickPosition.x;

	if (controller.IsButtonDown(XboxButtonID::RB)) {
		verticalInput += 1.f;
	}
	if (controller.IsButtonDown(XboxButtonID::LB)) {
		verticalInput -= 1.f;
	}

	Vec3 forwardDirection;
	Vec3 leftDirection;
	Vec3 upDirection;
	m_orientation.GetAsVectors_IFwd_JLeft_KUp(
		forwardDirection,
		leftDirection,
		upDirection);

	Vec3 movementDirection =
		(forwardDirection * forwardInput) +
		(leftDirection * leftInput) +
		(Vec3(0.f, 0.f, 1.f) * verticalInput);
	if (movementDirection.GetLengthSquared() > 0.f) {
		movementDirection = movementDirection.GetNormalized();
	}

	float movementSpeed = playerMoveSpeed;
	bool isSprintKeyHeld = inputSystem->IsKeyDown(KEYCODE_SHIFT);
	bool isSprintButtonHeld = controller.IsButtonDown(XboxButtonID::A);
	if (isSprintKeyHeld || isSprintButtonHeld) {
		movementSpeed *= playerSprintMultiplier;
	}
	m_position += movementDirection * (movementSpeed * deltaSecondsFromSystemClock);

	float yawDeltaDegrees = 0.f;
	float pitchDeltaDegrees = 0.f;

	Vec2 cursorDelta = inputSystem->GetCursorClientDelta();
	yawDeltaDegrees += -cursorDelta.x * playerMouseLookDegreesPerPixel;
	pitchDeltaDegrees += cursorDelta.y * playerMouseLookDegreesPerPixel;

	Vec2 rightStickPosition = controller.GetRightStick().GetPosition();
	yawDeltaDegrees +=
		-rightStickPosition.x * playerGamepadLookSpeedDegrees * deltaSecondsFromSystemClock;
	pitchDeltaDegrees +=
		-rightStickPosition.y * playerGamepadLookSpeedDegrees * deltaSecondsFromSystemClock;

	float rollInput = 0.f;
	if (inputSystem->IsKeyDown('Q')) {
		rollInput += 1.f;
	}
	if (inputSystem->IsKeyDown('E')) {
		rollInput -= 1.f;
	}
	rollInput += controller.GetLeftTrigger();
	rollInput -= controller.GetRightTrigger();
	float rollDeltaDegrees =
		rollInput * playerRollSpeedDegrees * deltaSecondsFromSystemClock;

	m_orientation.m_yawDegrees += yawDeltaDegrees;
	m_orientation.m_pitchDegrees += pitchDeltaDegrees;
	m_orientation.m_rollDegrees += rollDeltaDegrees;
	m_orientation.m_pitchDegrees = GetClamped(
		m_orientation.m_pitchDegrees,
		-playerMaxPitchDegrees,
		playerMaxPitchDegrees);
	m_orientation.m_rollDegrees = GetClamped(
		m_orientation.m_rollDegrees,
		-playerMaxRollDegrees,
		playerMaxRollDegrees);

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

void Player::SnapToPose(Vec3 const& position, EulerAngles const& orientation)
{
	m_position = position;
	m_orientation = orientation;
	UpdateCameraFromTransform();
}

void Player::UpdateCameraFromTransform()
{
	m_camera.SetPositionAndOrientation(m_position, m_orientation);
}
