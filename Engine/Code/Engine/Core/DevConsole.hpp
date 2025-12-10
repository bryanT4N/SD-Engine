#pragma once

#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"

#include <string>
#include <vector>

class BitmapFont;

//-----------------------------------------------------------------------------------------------
struct DevConsoleConfig
{
	bool  m_isEnabled  = true;
	float m_totalLines = 30.8f;
};

//-----------------------------------------------------------------------------------------------
enum class DevConsoleMode
{
	HIDDEN,
	OPEN_FULL,
};

//-----------------------------------------------------------------------------------------------
struct DevConsoleLine
{
	std::string m_text;
	Rgba8       m_color;
	float       m_timeSeconds = 0.0f;
	int         m_frameIndex  = 0;
	bool        m_isContinuation = false;
};

//-----------------------------------------------------------------------------------------------
class DevConsole
{
public:
	DevConsole( DevConsoleConfig const& config );
	~DevConsole();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	// Parses and executes one or more console commands.
	void Execute( std::string const& consoleCommandText );

	void AddLine( Rgba8 const& color, std::string const& text );

	void Render( AABB2 const& bounds, BitmapFont& font, float fontAspect = 1.0f ) const;

	DevConsoleMode GetMode() const;
	void           SetMode( DevConsoleMode mode );
	void           ToggleMode( DevConsoleMode mode );

	// Common log colors
	static Rgba8 const LOG_COLOR_ERROR;
	static Rgba8 const LOG_COLOR_WARNING;
	static Rgba8 const LOG_COLOR_INFO_MAJOR;
	static Rgba8 const LOG_COLOR_INFO_MINOR;

	// Example command callback (used for testing)
	static bool Command_Test( EventArgs& args );

protected:
	void Render_OpenFull( AABB2 const& bounds, BitmapFont& font, float fontAspect ) const;

protected:
	DevConsoleConfig             m_config;
	DevConsoleMode               m_mode = DevConsoleMode::HIDDEN;
	std::vector<DevConsoleLine>  m_lines;
	int                          m_frameNumber = 0;
};


