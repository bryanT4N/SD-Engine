#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"

//-----------------------------------------------------------------------------------------------
Rgba8 const DevConsole::LOG_COLOR_ERROR      = Rgba8( 255, 0, 0, 255 );
Rgba8 const DevConsole::LOG_COLOR_WARNING    = Rgba8( 255, 255, 0, 255 );
Rgba8 const DevConsole::LOG_COLOR_INFO_MAJOR = Rgba8( 200, 200, 255, 255 );
Rgba8 const DevConsole::LOG_COLOR_INFO_MINOR = Rgba8( 160, 160, 160, 255 );

//-----------------------------------------------------------------------------------------------
DevConsole::DevConsole( DevConsoleConfig const& config )
	: m_config( config )
{
}

//-----------------------------------------------------------------------------------------------
DevConsole::~DevConsole()
{
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Startup()
{
	m_lines.clear();
	m_frameNumber = 0;

	// Test devConsole & event
	SubscribeEventCallbackFunction( "Test", &DevConsole::Command_Test );

	AddLine( LOG_COLOR_INFO_MAJOR, "Help: Input currently not supported.\n       Press '/' to test events." );
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{
	m_lines.clear();
}

//-----------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{
	++m_frameNumber;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::EndFrame()
{
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Execute( std::string const& consoleCommandText )
{
	Strings lines = SplitStringOnDelimiter( consoleCommandText, '\n' );

	for( size_t lineIndex = 0; lineIndex < lines.size(); ++lineIndex ) {
		std::string const& line = lines[lineIndex];
		if( line.empty() ) {
			continue;
		}

		Strings tokens = SplitStringOnDelimiter( line, ' ' );
		if( tokens.empty() ) {
			continue;
		}

		std::string const& eventName = tokens[0];
		if( eventName.empty() ) {
			continue;
		}

		EventArgs args;

		for( size_t tokenIndex = 1; tokenIndex < tokens.size(); ++tokenIndex )
		{
			std::string const& token = tokens[tokenIndex];
			if( token.empty() ) {
				continue;
			}

			size_t equalIndex = token.find( '=' );
			if( equalIndex == std::string::npos ) {
				continue;
			}

			std::string key = token.substr( 0, equalIndex );
			std::string value = token.substr( equalIndex + 1 );

			if( key.empty() ) {
				continue;
			}

			args.SetValue( key, value );
		}

		FireEvent( eventName, args );
	}
}

//-----------------------------------------------------------------------------------------------
void DevConsole::AddLine( Rgba8 const& color, std::string const& text )
{
	Strings splitLines = SplitStringOnDelimiter( text, '\n' );
	if( splitLines.empty() ) {
		return;
	}

	float timeSeconds = static_cast<float>(GetCurrentTimeSeconds());

	for( size_t i = 0; i < splitLines.size(); ++i )
	{
		std::string const& piece = splitLines[i];
		if( piece.empty() ) {
			continue;
		}

		DevConsoleLine line;
		line.m_text        = piece;
		line.m_color       = color;
		line.m_timeSeconds = timeSeconds;
		line.m_frameIndex  = m_frameNumber;
		line.m_isContinuation = (i > 0);

		m_lines.push_back( line );
	}
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Render( AABB2 const& bounds, BitmapFont& font, float fontAspect ) const
{
	if (m_mode == DevConsoleMode::HIDDEN) {
		return;
	}

	Render_OpenFull( bounds, font, fontAspect );
}

//-----------------------------------------------------------------------------------------------
DevConsoleMode DevConsole::GetMode() const
{
	return m_mode;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::SetMode( DevConsoleMode mode ) {
	m_mode = mode;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::ToggleMode( DevConsoleMode mode ) {
	if( m_mode == mode ) {
		m_mode = DevConsoleMode::HIDDEN;
	}
	else {
		m_mode = mode;
	}
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Render_OpenFull( AABB2 const& bounds, BitmapFont& font, [[maybe_unused]]float fontAspect ) const
{
	if( g_engine == nullptr || g_engine->m_render == nullptr ) {
		return;
	}

	// Background quad
	std::vector<Vertex> backgroundVerts;
	AddVertsForAABB2D( backgroundVerts, bounds, Rgba8( 0, 0, 0, 180 ) );
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( backgroundVerts );

	std::vector<Vertex> textVerts;

	Vec2 dims = bounds.GetDimensions();

	float visibleLines = m_config.m_totalLines;
	if( visibleLines <= 1.0f ) {
		return;
	}

	float cellHeight = dims.y / visibleLines;

	const int MAX_FULL_LOG_LINES = 30;

	int totalLogLines = static_cast<int>( m_lines.size() );
	int linesToDraw = totalLogLines;
	if( linesToDraw > MAX_FULL_LOG_LINES ) {
		linesToDraw = MAX_FULL_LOG_LINES;
	}

	// Latest line at bottom
	int startIndexFull = 0;
	if( linesToDraw > 0 && totalLogLines > linesToDraw ) {
		startIndexFull = totalLogLines - linesToDraw;
	}

	// Draw log lines
	for( int drawIndex = 0; drawIndex < linesToDraw; ++drawIndex )
	{
		int lineIndex = startIndexFull + (linesToDraw - 1 - drawIndex);
		float yOffset = bounds.m_mins.y + cellHeight * static_cast<float>( drawIndex + 1 );
		Vec2 textMins( bounds.m_mins.x + 8.0f, yOffset + cellHeight * 0.25f );

		DevConsoleLine const& line = m_lines[static_cast<size_t>( lineIndex )];
		std::string framedText;
		if( line.m_isContinuation ) {
			framedText = Stringf( "          %s", line.m_text.c_str() );
		} else {
			framedText = Stringf( "[%08d] %s", line.m_frameIndex, line.m_text.c_str() );
		}
		font.AddVertsForText2D( textVerts, textMins, cellHeight, framedText, line.m_color, 0.64f );
	}

	// Input line
	{
		float yOffset = bounds.m_mins.y;
		Vec2 inputMins( bounds.m_mins.x + 8.0f, yOffset + cellHeight * 0.25f );
		std::string inputPrompt = ">";
		Rgba8 inputColor( 255, 255, 255, 255 );
		font.AddVertsForText2D( textVerts, inputMins, cellHeight, inputPrompt, inputColor, 0.72f );
	}

	g_engine->m_render->BindTexture( &font.GetTexture() );
	g_engine->m_render->DrawVertexArray( textVerts );
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Command_Test( [[maybe_unused]] EventArgs& args )
{
	if( g_engine != nullptr && g_engine->m_devConsole != nullptr ) {
		g_engine->m_devConsole->AddLine( DevConsole::LOG_COLOR_INFO_MAJOR, "Test event fired. `Command_Test()` called." );
	}

	return false;
}

