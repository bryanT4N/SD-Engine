#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Window/Window.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//-----------------------------------------------------------------------------------------------
unsigned char const KEYCODE_LBUTTON = 0x01;
unsigned char const KEYCODE_RBUTTON = 0x02;
unsigned char const KEYCODE_ESC = 0x1B;
unsigned char const KEYCODE_SPACE = 0x20;
unsigned char const KEYCODE_ENTER = 0x0D;
unsigned char const KEYCODE_TILDE = 0xC0;		// VK_OEM_3 (`/~ key)
unsigned char const KEYCODE_SLASH = 0xBF;		// VK_OEM_2 (/ ? key)
unsigned char const KEYCODE_UPARROW = 0x26;
unsigned char const KEYCODE_DOWNARROW = 0x28;
unsigned char const KEYCODE_LEFTARROW = 0x25;
unsigned char const KEYCODE_RIGHTARROW = 0x27;
unsigned char const KEYCODE_BACKSPACE = 0x08;
unsigned char const KEYCODE_INSERT = 0x2D;
unsigned char const KEYCODE_DELETE = 0x2E;
unsigned char const KEYCODE_HOME = 0x24;
unsigned char const KEYCODE_END = 0x23;
unsigned char const UPARROW = KEYCODE_UPARROW;
unsigned char const DOWNARROW = KEYCODE_DOWNARROW;
unsigned char const LEFTARROW = KEYCODE_LEFTARROW;
unsigned char const RIGHTARROW = KEYCODE_RIGHTARROW;
unsigned char const KEYCODE_F1 = 0x70;
unsigned char const KEYCODE_F2 = 0x71;
unsigned char const KEYCODE_F3 = 0x72;
unsigned char const KEYCODE_F4 = 0x73;
unsigned char const KEYCODE_F5 = 0x74;
unsigned char const KEYCODE_F6 = 0x75;
unsigned char const KEYCODE_F7 = 0x76;
unsigned char const KEYCODE_F8 = 0x77;
unsigned char const KEYCODE_F9 = 0x78;
unsigned char const KEYCODE_F10 = 0x79;
unsigned char const KEYCODE_F11 = 0x7A;
unsigned char const KEYCODE_F12 = 0x7B;
unsigned char const KEYCODE_SHIFT = 0x10;

//-----------------------------------------------------------------------------------------------
static HWND GetEngineWindowHandle()
{
	if (g_engine == nullptr || g_engine->m_window == nullptr) {
		return nullptr;
	}

	return static_cast<HWND>(g_engine->m_window->GetHwnd());
}

//-----------------------------------------------------------------------------------------------
static IntVec2 GetClientCenterForWindow(HWND windowHandle)
{
	RECT clientRect;
	::GetClientRect(windowHandle, &clientRect);
	int centerX = (clientRect.left + clientRect.right) / 2;
	int centerY = (clientRect.top + clientRect.bottom) / 2;
	return IntVec2(centerX, centerY);
}

//-----------------------------------------------------------------------------------------------
static IntVec2 GetCursorClientPositionForWindow(HWND windowHandle)
{
	POINT cursorPosition;
	::GetCursorPos(&cursorPosition);
	::ScreenToClient(windowHandle, &cursorPosition);
	return IntVec2(cursorPosition.x, cursorPosition.y);
}

//-----------------------------------------------------------------------------------------------
static void SetCursorScreenPositionFromClient(HWND windowHandle, IntVec2 const& clientPosition)
{
	POINT screenPosition;
	screenPosition.x = clientPosition.x;
	screenPosition.y = clientPosition.y;
	::ClientToScreen(windowHandle, &screenPosition);
	::SetCursorPos(screenPosition.x, screenPosition.y);
}

//-----------------------------------------------------------------------------------------------
InputSystem::InputSystem(InputConfig const& config)
	:m_config(config)
{
	for (int keyCode = 0; keyCode < NUM_KEYCODES; ++keyCode) {
		m_keyStates[keyCode].isPressed = false;
		m_keyStates[keyCode].wasPressedLastFrame = false;
	}

	m_cursorState.m_cursorMode = CursorMode::POINTER;
	m_cursorState.m_cursorClientDelta = IntVec2(0, 0);
	m_cursorState.m_cursorClientPosition = IntVec2(0, 0);
	m_previousCursorClientPosition = IntVec2(0, 0);
	m_isCursorVisible = true;
}

void InputSystem::Startup()
{
	SubscribeEventCallbackFunction("KeyPressed", InputSystem::Event_KeyPressed);
	SubscribeEventCallbackFunction("KeyReleased", InputSystem::Event_KeyReleased);

	HWND windowHandle = GetEngineWindowHandle();
	if (windowHandle != nullptr) {
		IntVec2 cursorCenter = GetClientCenterForWindow(windowHandle);
		m_cursorState.m_cursorClientPosition = cursorCenter;
		m_previousCursorClientPosition = cursorCenter;
	}

	SetCursorMode(CursorMode::POINTER);
}

void InputSystem::Shutdown()
{
	SetCursorMode(CursorMode::POINTER);

	UnsubscribeEventCallbackFunction("KeyPressed", InputSystem::Event_KeyPressed);
	UnsubscribeEventCallbackFunction("KeyReleased", InputSystem::Event_KeyReleased);
}

void InputSystem::SetCursorMode(CursorMode cursorMode)
{
	if (m_cursorState.m_cursorMode == cursorMode) {
		return;
	}

	m_cursorState.m_cursorMode = cursorMode;

	bool shouldCursorBeVisible = (m_cursorState.m_cursorMode == CursorMode::POINTER);
	if (shouldCursorBeVisible != m_isCursorVisible) {
		if (shouldCursorBeVisible) {
			while (::ShowCursor(TRUE) < 0) {
			}
		}
		else {
			while (::ShowCursor(FALSE) >= 0) {
			}
		}
		m_isCursorVisible = shouldCursorBeVisible;
	}

	HWND windowHandle = GetEngineWindowHandle();
	if (windowHandle == nullptr) {
		m_cursorState.m_cursorClientDelta = IntVec2(0, 0);
		return;
	}

	if (m_cursorState.m_cursorMode == CursorMode::FPS) {
		IntVec2 cursorCenter = GetClientCenterForWindow(windowHandle);
		SetCursorScreenPositionFromClient(windowHandle, cursorCenter);
		m_cursorState.m_cursorClientPosition = cursorCenter;
		m_previousCursorClientPosition = cursorCenter;
	}
	else {
		IntVec2 currentCursorPosition = GetCursorClientPositionForWindow(windowHandle);
		m_cursorState.m_cursorClientPosition = currentCursorPosition;
		m_previousCursorClientPosition = currentCursorPosition;
	}
	m_cursorState.m_cursorClientDelta = IntVec2(0, 0);
}

Vec2 InputSystem::GetCursorClientDelta() const
{
	return Vec2(
		static_cast<float>(m_cursorState.m_cursorClientDelta.x),
		static_cast<float>(m_cursorState.m_cursorClientDelta.y));
}

Vec2 InputSystem::GetCursorClientPosition() const
{
	return Vec2(
		static_cast<float>(m_cursorState.m_cursorClientPosition.x),
		static_cast<float>(m_cursorState.m_cursorClientPosition.y));
}

Vec2 InputSystem::GetCursorNormalizedPosition() const
{
	if (g_engine == nullptr || g_engine->m_window == nullptr) {
		return Vec2(0.f, 0.f);
	}

	IntVec2 clientDimensions = g_engine->m_window->GetClientDimensions();
	if (clientDimensions.x <= 0 || clientDimensions.y <= 0) {
		return Vec2(0.f, 0.f);
	}

	float normalizedX =
		static_cast<float>(m_cursorState.m_cursorClientPosition.x) /
		static_cast<float>(clientDimensions.x);
	float normalizedY =
		static_cast<float>(m_cursorState.m_cursorClientPosition.y) /
		static_cast<float>(clientDimensions.y);
	return Vec2(normalizedX, 1.f - normalizedY);
}

void InputSystem::BeginFrame()
{
	bool shouldCursorBeVisible = (m_cursorState.m_cursorMode == CursorMode::POINTER);
	if (shouldCursorBeVisible != m_isCursorVisible) {
		if (shouldCursorBeVisible) {
			while (::ShowCursor(TRUE) < 0) {
			}
		}
		else {
			while (::ShowCursor(FALSE) >= 0) {
			}
		}
		m_isCursorVisible = shouldCursorBeVisible;
	}

	m_previousCursorClientPosition = m_cursorState.m_cursorClientPosition;
	HWND windowHandle = GetEngineWindowHandle();
	if (windowHandle != nullptr) {
		if (m_cursorState.m_cursorMode == CursorMode::FPS) {
			IntVec2 currentPosition = GetCursorClientPositionForWindow(windowHandle);
			int deltaX = currentPosition.x - m_previousCursorClientPosition.x;
			int deltaY = currentPosition.y - m_previousCursorClientPosition.y;
			m_cursorState.m_cursorClientDelta = IntVec2(deltaX, deltaY);

			IntVec2 cursorCenter = GetClientCenterForWindow(windowHandle);
			SetCursorScreenPositionFromClient(windowHandle, cursorCenter);
			m_cursorState.m_cursorClientPosition = cursorCenter;
			m_previousCursorClientPosition = cursorCenter;
		}
		else {
			m_cursorState.m_cursorClientPosition = GetCursorClientPositionForWindow(windowHandle);
			m_cursorState.m_cursorClientDelta = IntVec2(0, 0);
		}
	}
	else {
		m_cursorState.m_cursorClientDelta = IntVec2(0, 0);
	}

	// Update controller status in new frame.
	for (int controllerID = 0; controllerID < NUM_XBOX_CONTROLLERS; ++controllerID) {
		m_controllers[controllerID].Update();
	}
}

void InputSystem::EndFrame()
{
	// Move keyStatus
	for (int keyCode = 0; keyCode < NUM_KEYCODES; ++keyCode) {
		m_keyStates[keyCode].wasPressedLastFrame = m_keyStates[keyCode].isPressed;
	}

	// Move controller button status
	for (int controllerID = 0; controllerID < NUM_XBOX_CONTROLLERS; ++controllerID) {
		for (int buttonId = 0; buttonId < (int)XboxButtonID::NUM; ++buttonId) {
			m_controllers[controllerID].m_buttons[buttonId].wasPressedLastFrame =
				m_controllers[controllerID].m_buttons[buttonId].isPressed;
		}
	}
}

bool InputSystem::WasKeyJustPressed(unsigned char keyCode)
{
	return m_keyStates[keyCode].isPressed && !m_keyStates[keyCode].wasPressedLastFrame;
}

bool InputSystem::WasKeyJustReleased(unsigned char keyCode)
{
	return !m_keyStates[keyCode].isPressed && m_keyStates[keyCode].wasPressedLastFrame;
}

bool InputSystem::IsKeyDown(unsigned char keyCode)
{
	return m_keyStates[keyCode].isPressed;
}

void InputSystem::HandleKeyPressed(unsigned char keyCode)
{
	m_keyStates[keyCode].isPressed = true;
}

void InputSystem::HandleKeyReleased(unsigned char keyCode)
{
	m_keyStates[keyCode].isPressed = false;
}

XboxController& InputSystem::GetController(int controllerID)
{
	return m_controllers[controllerID];
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::Event_KeyPressed(EventArgs& args)
{
	if (g_engine == nullptr || g_engine->m_input == nullptr) {
		return false;
	}

	int keyCodeInt = args.GetValue("KeyCode", -1);
	if (keyCodeInt < 0 || keyCodeInt >= NUM_KEYCODES) {
		return false;
	}

	unsigned char keyCode = static_cast<unsigned char>(keyCodeInt);
	g_engine->m_input->HandleKeyPressed(keyCode);
	return true;
}

//-----------------------------------------------------------------------------------------------
bool InputSystem::Event_KeyReleased(EventArgs& args)
{
	if (g_engine == nullptr || g_engine->m_input == nullptr) {
		return false;
	}

	int keyCodeInt = args.GetValue("KeyCode", -1);
	if (keyCodeInt < 0 || keyCodeInt >= NUM_KEYCODES) {
		return false;
	}

	unsigned char keyCode = static_cast<unsigned char>(keyCodeInt);
	g_engine->m_input->HandleKeyReleased(keyCode);
	return true;
}
