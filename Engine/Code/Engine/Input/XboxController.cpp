#include "Engine/Input/XboxController.hpp"

#include "Engine/Math/MathUtils.hpp"

#include <stdio.h>
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)
#include <Xinput.h>
#pragma comment( lib, "xinput" )

//-----------------------------------------------------------------------------------------------

short XboxButtonCodes[XboxButtonID::NUM] = { static_cast<short>(XINPUT_GAMEPAD_A), static_cast<short>(XINPUT_GAMEPAD_B), static_cast<short>(XINPUT_GAMEPAD_X), static_cast<short>(XINPUT_GAMEPAD_Y),
static_cast<short>(XINPUT_GAMEPAD_LEFT_SHOULDER), static_cast<short>(XINPUT_GAMEPAD_RIGHT_SHOULDER),
static_cast<short>(XINPUT_GAMEPAD_BACK), static_cast<short>(XINPUT_GAMEPAD_START),
static_cast<short>(XINPUT_GAMEPAD_LEFT_THUMB), static_cast<short>(XINPUT_GAMEPAD_RIGHT_THUMB),
static_cast<short>(XINPUT_GAMEPAD_DPAD_UP), static_cast<short>(XINPUT_GAMEPAD_DPAD_DOWN), static_cast<short>(XINPUT_GAMEPAD_DPAD_LEFT), static_cast<short>(XINPUT_GAMEPAD_DPAD_RIGHT)
};

//-----------------------------------------------------------------------------------------------
XboxController::XboxController(int controllerID):
	m_controllerID(controllerID) {
	for (int buttonIndex = 0; buttonIndex < (int)XboxButtonID::NUM; ++buttonIndex) {
		m_buttons[buttonIndex].isPressed = false;
		m_buttons[buttonIndex].wasPressedLastFrame = false;
	}
}

XboxController::~XboxController()
{
}

bool XboxController::IsConnected() const
{
	return m_isConnected;
}

int XboxController::GetControllerID() const
{
	return m_controllerID;
}

AnalogJoystick const& XboxController::GetLeftStick() const
{
	return m_leftstick;
}

AnalogJoystick const& XboxController::GetRightStick() const
{
	return m_rightstick;
}

float XboxController::GetLeftTrigger() const
{
	return m_leftTrigger;
}

float XboxController::GetRightTrigger() const
{
	return m_rightTrigger;
}

KeyButtonState const& XboxController::GetButton(XboxButtonID buttonID) const
{
	return m_buttons[int(buttonID)];
}

bool XboxController::IsButtonDown(XboxButtonID buttonID) const
{
	return m_buttons[int(buttonID)].isPressed;
}

bool XboxController::WasButtonJustPressed(XboxButtonID buttonID) const
{
	return m_buttons[int(buttonID)].isPressed && !m_buttons[int(buttonID)].wasPressedLastFrame;
}

bool XboxController::WasButtonJustReleased(XboxButtonID buttonID) const
{
	return !m_buttons[int(buttonID)].isPressed && m_buttons[int(buttonID)].wasPressedLastFrame;
}

void XboxController::Update()
{
	XINPUT_STATE xboxControllerState = {}; // Clear (zero-out) the controller state structure
	DWORD result = XInputGetState(m_controllerID, &xboxControllerState); // Get fresh state info

	m_isConnected = (result == ERROR_SUCCESS);

	if (IsConnected()) {
		UpdateJoystick(m_leftstick, xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY);
		UpdateJoystick(m_rightstick, xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY);

		UpdateTrigger(m_leftTrigger, xboxControllerState.Gamepad.bLeftTrigger);
		UpdateTrigger(m_rightTrigger, xboxControllerState.Gamepad.bRightTrigger);

		for (int buttonIndex = 0; buttonIndex < (int)XboxButtonID::NUM; ++buttonIndex) {
			UpdateButton(XboxButtonID(buttonIndex), xboxControllerState.Gamepad.wButtons, XboxButtonCodes[buttonIndex]);
		}
	}
	else {
		Reset();
	}

}

void XboxController::Reset()
{
	m_leftstick.Reset();
	m_rightstick.Reset();

	UpdateTrigger(m_leftTrigger, unsigned char(0));
	UpdateTrigger(m_rightTrigger, unsigned char(0));

	for (int buttonIndex = 0; buttonIndex < (int)XboxButtonID::NUM; ++buttonIndex) {
		m_buttons[buttonIndex].isPressed = false;
	}
}

void XboxController::UpdateJoystick(AnalogJoystick& out_joystick, short rawX, short rawY)
{
	float normalizedX = RangeMap(rawX, static_cast<float>(SHRT_MIN), static_cast<float>(SHRT_MAX), -1.f, 1.f);
	float normalizedY = RangeMap(rawY, static_cast<float>(SHRT_MIN), static_cast<float>(SHRT_MAX), -1.f, 1.f);
	out_joystick.UpdatePosition(normalizedX, normalizedY);
}

void XboxController::UpdateTrigger(float& out_triggerValue, unsigned char rawValue)
{
	out_triggerValue = RangeMap(rawValue, 0.f, 255.f, 0.f, 1.f);
}

void XboxController::UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag)
{
	m_buttons[int(buttonID)].isPressed = ((buttonFlags & buttonFlag) == buttonFlag);
}
