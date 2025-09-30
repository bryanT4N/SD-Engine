#pragma once
#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Input/KeyButtonState.hpp"

//-----------------------------------------------------------------------------------------------
enum XboxButtonID {
	A, B, X, Y, 
	LB, RB, 
	MENU, START, 
	LS, RS, 
	DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT, 
	NUM
};

//-----------------------------------------------------------------------------------------------
class XboxController {
public:
	friend class InputSystem;

private:
	XboxController(int controllerID);
	~XboxController();

public:
	bool					IsConnected() const; 
	int						GetControllerID() const; 
	AnalogJoystick const&	GetLeftStick() const;
	AnalogJoystick const&	GetRightStick() const;
	float					GetLeftTrigger() const;
	float					GetRightTrigger() const;
	KeyButtonState const&	GetButton(XboxButtonID buttonID) const;
	bool					IsButtonDown(XboxButtonID buttonID) const;
	bool					WasButtonJustPressed(XboxButtonID buttonID) const;
	bool					WasButtonJustReleased(XboxButtonID buttonID) const;
		      
private:
	void Update();
	void Reset();
	void UpdateJoystick(AnalogJoystick& out_joystick, short rawX, short rawY);
	void UpdateTrigger(float& out_triggerValue, unsigned char rawValue); 
	void UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag);

private:
	int					m_controllerID			= -1;
	bool				m_isConnected			= false;
	float				m_leftTrigger			= 0.f;
	float				m_rightTrigger			= 0.f;
	KeyButtonState		m_buttons[(int)XboxButtonID::NUM];
	AnalogJoystick		m_leftstick;
	AnalogJoystick		m_rightstick;
};
	