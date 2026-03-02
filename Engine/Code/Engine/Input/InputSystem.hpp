#pragma once
#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"

//-----------------------------------------------------------------------------------------------
struct InputConfig {
	bool m_isEnabled = true;
};

//-----------------------------------------------------------------------------------------------
enum class CursorMode
{
	POINTER,
	FPS,
};

//-----------------------------------------------------------------------------------------------
struct CursorState
{
	IntVec2 m_cursorClientDelta;
	IntVec2 m_cursorClientPosition;
	CursorMode m_cursorMode = CursorMode::POINTER;
};

//-----------------------------------------------------------------------------------------------
extern unsigned char const KEYCODE_LBUTTON;
extern unsigned char const KEYCODE_RBUTTON;
extern unsigned char const KEYCODE_ESC;
extern unsigned char const KEYCODE_SPACE;
extern unsigned char const KEYCODE_ENTER;
extern unsigned char const KEYCODE_TILDE;
extern unsigned char const KEYCODE_SLASH;
extern unsigned char const KEYCODE_UPARROW;
extern unsigned char const KEYCODE_DOWNARROW;
extern unsigned char const KEYCODE_LEFTARROW;
extern unsigned char const KEYCODE_RIGHTARROW;
extern unsigned char const KEYCODE_BACKSPACE;
extern unsigned char const KEYCODE_INSERT;
extern unsigned char const KEYCODE_DELETE;
extern unsigned char const KEYCODE_HOME;
extern unsigned char const KEYCODE_END;
extern unsigned char const UPARROW;
extern unsigned char const DOWNARROW;
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
extern unsigned char const KEYCODE_SHIFT;

//-----------------------------------------------------------------------------------------------
constexpr int NUM_KEYCODES			= 256;
constexpr int NUM_XBOX_CONTROLLERS	= 4;

class InputSystem;

class InputSystem{
public:
	InputSystem(InputConfig const& config); 
	~InputSystem() = default;
	void Startup(); 
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void SetCursorMode(CursorMode cursorMode);
	Vec2 GetCursorClientDelta() const;
	Vec2 GetCursorClientPosition() const;
	Vec2 GetCursorNormalizedPosition() const;

	bool WasKeyJustPressed(unsigned char keyCode); 
	bool WasKeyJustReleased(unsigned char keyCode);

	bool IsKeyDown(unsigned char keyCode);
	void HandleKeyPressed(unsigned char keyCode);
	void HandleKeyReleased(unsigned char keyCode);

	XboxController& GetController(int controllerID); 

	InputConfig		m_config;

	static bool Event_KeyPressed(EventArgs& args);
	static bool Event_KeyReleased(EventArgs& args);

protected:
	KeyButtonState m_keyStates[NUM_KEYCODES];	// Indexed by key code, e.g. 65 == 'A'
	XboxController m_controllers[NUM_XBOX_CONTROLLERS] = {
		XboxController(0),
		XboxController(1),
		XboxController(2),
		XboxController(3)
	};
	CursorState m_cursorState;
	IntVec2 m_previousCursorClientPosition;
	bool m_isCursorVisible = true;
};