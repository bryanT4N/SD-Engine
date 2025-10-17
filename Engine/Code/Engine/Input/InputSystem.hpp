#pragma once
#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/XboxController.hpp"

//-----------------------------------------------------------------------------------------------
struct InputConfig {
	bool m_isEnabled = true;
};

//-----------------------------------------------------------------------------------------------
extern unsigned char const KEYCODE_LBUTTON;
extern unsigned char const KEYCODE_RBUTTON;
extern unsigned char const KEYCODE_ESC;
extern unsigned char const KEYCODE_SPACE;
extern unsigned char const KEYCODE_ENTER;
extern unsigned char const UPARROW;
extern unsigned char const DONNARROW;
extern unsigned char const LEFTARROW;
extern unsigned char const RIGHTARROW;
extern unsigned char const KEYCODE_F1;
extern unsigned char const KEYCODE_F2;
extern unsigned char const KEYCODE_F3;
extern unsigned char const KEYCODE_F4;
extern unsigned char const KEYCODE_F5;
extern unsigned char const KEYCODE_F6;
extern unsigned char const KEYCODE_F7;
extern unsigned char const KEYCODE_F8;
extern unsigned char const KEYCODE_F9;
extern unsigned char const KEYCODE_F10;
extern unsigned char const KEYCODE_F11;
extern unsigned char const KEYCODE_F12;

//-----------------------------------------------------------------------------------------------
constexpr int NUM_KEYCODES			= 256;
constexpr int NUM_XBOX_CONTROLLERS	= 4;

class InputSystem{
public:
	InputSystem(InputConfig const& config); 
	~InputSystem() = default;
	void Startup(); 
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	bool WasKeyJustPressed(unsigned char keyCode); 
	bool WasKeyJustReleased(unsigned char keyCode);

	bool IsKeyDown(unsigned char keyCode);
	void HandleKeyPressed(unsigned char keyCode);
	void HandleKeyReleased(unsigned char keyCode);

	XboxController& GetController(int controllerID); 

	InputConfig		m_config;

protected:
	KeyButtonState m_keyStates[NUM_KEYCODES];	// Indexed by key code, e.g. 65 == 'A'
	XboxController m_controllers[NUM_XBOX_CONTROLLERS] = { XboxController(0), XboxController(1), XboxController(2), XboxController(3)};
};