#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EventSystem.hpp"

//-----------------------------------------------------------------------------------------------
class Game;
class App;

//-----------------------------------------------------------------------------------------------
extern App*		g_theApp;
extern double	timeCurrentFrame;
extern double	timePreviousFrame;

//-----------------------------------------------------------------------------------------------
class App
{
public:
	Game*		m_game;
	Camera		m_gameCamera;

	// App-level actions (such as the general �frame�, quitting, etc.)
	bool		m_isQuitting = false;

public:
	App();
	~App();

	void RunFrame();
	void Update(float deltaseconds);	
	void Render() const;				
										
	void SetIsQuitting();				
	void UpdateFromKeyboard();			
	void UpdateCursorMode();
	static bool Command_Quit(EventArgs& args);
										
	bool IsQuitting() const;			
};


