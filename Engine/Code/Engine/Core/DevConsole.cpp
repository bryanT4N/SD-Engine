#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/Window.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdlib>

DevConsole* g_theDevConsole = nullptr;

//-----------------------------------------------------------------------------------------------
Rgba8 const DevConsole::TIMESTAMP = Rgba8(130, 130, 130, 255);
Rgba8 const DevConsole::INPUT_TEXT = Rgba8(255, 0, 255, 255);
Rgba8 const DevConsole::INPUT_INSERTION_POINT = Rgba8(255, 255, 96, 255);
Rgba8 const DevConsole::LOG_COLOR_ERROR = Rgba8(255, 96, 96, 255);
Rgba8 const DevConsole::LOG_COLOR_WARNING = Rgba8(255, 220, 96, 255);
Rgba8 const DevConsole::LOG_COLOR_INFO_MAJOR = Rgba8(96, 220, 255, 255);
Rgba8 const DevConsole::LOG_COLOR_INFO_MINOR = Rgba8(180, 180, 180, 255);

//-----------------------------------------------------------------------------------------------
static constexpr int TIMESTAMP_MAX_FRAME_VALUE = 9999999;
static constexpr int TIMESTAMP_COLUMN_CHAR_COUNT = 10;
static char const* TIMESTAMP_COLUMN_SAMPLE = "[0000000] ";
static constexpr int HELP_COMMAND_NAME_WIDTH = 18;
static constexpr unsigned char KEYCODE_V = static_cast<unsigned char>('V');
static char const* SIMULATED_KEYPRESS_FROM_CONSOLE_ARG = "__simulatedKeypressFromConsoleCommand";

//-----------------------------------------------------------------------------------------------
static bool IsValidDevConsoleInputCharacter(unsigned char character)
{
	return character >= 32 && character <= 126 && character != '~' && character != '`';
}

//-----------------------------------------------------------------------------------------------
static std::string ToLowerCase(std::string const& text)
{
	std::string lowered = text;
	for (char& ch : lowered) {
		if (ch >= 'A' && ch <= 'Z') {
			ch = static_cast<char>(ch - 'A' + 'a');
		}
	}
	return lowered;
}

//-----------------------------------------------------------------------------------------------
static bool ParseArgumentToken(std::string const& token, std::string& out_key, std::string& out_value)
{
	size_t equalPos = token.find('=');
	if (equalPos == std::string::npos || equalPos == 0 || equalPos >= token.size() - 1) {
		return false;
	}

	out_key = token.substr(0, equalPos);
	out_value = token.substr(equalPos + 1);
	return true;
}

//-----------------------------------------------------------------------------------------------
static bool IsIntegerLiteral(std::string const& text)
{
	if (text.empty()) {
		return false;
	}

	char* endPtr = nullptr;
	std::strtol(text.c_str(), &endPtr, 10);
	return endPtr != text.c_str() && *endPtr == '\0';
}

//-----------------------------------------------------------------------------------------------
static bool IsFloatLiteral(std::string const& text)
{
	if (text.empty()) {
		return false;
	}

	char* endPtr = nullptr;
	std::strtod(text.c_str(), &endPtr);
	return endPtr != text.c_str() && *endPtr == '\0';
}

//-----------------------------------------------------------------------------------------------
enum class DevConsoleArgumentType
{
	ANY,
	INTEGER,
	FLOAT,
};

//-----------------------------------------------------------------------------------------------
struct DevConsoleArgumentSpec
{
	char const* m_canonicalKey = "";
	char const* m_lookupKeyLower = "";
	DevConsoleArgumentType m_valueType = DevConsoleArgumentType::ANY;
	char const* m_exampleValue = "";
};

//-----------------------------------------------------------------------------------------------
static constexpr int MAX_COMMAND_ARGUMENTS = 4;
struct DevConsoleCommandSpec
{
	char const* m_canonicalCommandName = "";
	char const* m_displayCommandName = "";
	bool m_showExampleInHelp = false;
	bool m_injectSimulatedKeypressFlag = false;
	int m_argumentCount = 0;
	DevConsoleArgumentSpec m_arguments[MAX_COMMAND_ARGUMENTS];
};

//-----------------------------------------------------------------------------------------------
static DevConsoleCommandSpec const g_devConsoleCommandSpecs[] = {
	{ "help",				"Help",					false, false, 0, {} },
	{ "clear",				"Clear",				false, false, 0, {} },
	{ "quit",				"Quit",					false, false, 0, {} },
	{ "debugrenderclear",	"DebugRenderClear",		false, false, 0, {} },
	{ "debugrendertoggle",	"DebugRenderToggle",	false, false, 0, {} },
	{ "setgameclockscale",	"SetGameClockScale",	true, false, 1, { { "scale", "scale", DevConsoleArgumentType::FLOAT, "0.25" } } },
	{ "keypressed",			"KeyPressed",			true, true, 1, { { "KeyCode", "keycode", DevConsoleArgumentType::INTEGER, "65" } } },
	{ "keyreleased",		"KeyReleased",			true, false, 1, { { "KeyCode", "keycode", DevConsoleArgumentType::INTEGER, "65" } } },
	{ "charinput",			"CharInput",			true, false, 1, { { "CharCode", "charcode", DevConsoleArgumentType::INTEGER, "97" } } },
};

//-----------------------------------------------------------------------------------------------
static DevConsoleCommandSpec const* FindCommandSpec(std::string const& commandName)
{
	std::string loweredName = ToLowerCase(commandName);
	for (DevConsoleCommandSpec const& spec : g_devConsoleCommandSpecs) {
		if (loweredName == spec.m_canonicalCommandName) {
			return &spec;
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
static bool IsValueValidForArgumentType(std::string const& value, DevConsoleArgumentType valueType)
{
	if (valueType == DevConsoleArgumentType::INTEGER) {
		return IsIntegerLiteral(value);
	}
	if (valueType == DevConsoleArgumentType::FLOAT) {
		return IsFloatLiteral(value);
	}

	return true;
}

//-----------------------------------------------------------------------------------------------
static std::string BuildCommandExample(DevConsoleCommandSpec const& spec)
{
	if (spec.m_argumentCount <= 0) {
		return "";
	}

	std::string example = spec.m_displayCommandName;
	for (int argumentIndex = 0; argumentIndex < spec.m_argumentCount; ++argumentIndex) {
		DevConsoleArgumentSpec const& argumentSpec = spec.m_arguments[argumentIndex];
		example += Stringf(" %s=%s", argumentSpec.m_canonicalKey, argumentSpec.m_exampleValue);
	}
	return example;
}

//-----------------------------------------------------------------------------------------------
static std::string GetCommandDisplayName(std::string const& commandName)
{
	DevConsoleCommandSpec const* spec = FindCommandSpec(commandName);
	if (spec != nullptr) {
		return spec->m_displayCommandName;
	}

	return commandName;
}

//-----------------------------------------------------------------------------------------------
static bool ShouldInjectSimulatedKeypressFlag(std::string const& commandName)
{
	DevConsoleCommandSpec const* spec = FindCommandSpec(commandName);
	return spec != nullptr && spec->m_injectSimulatedKeypressFlag;
}

//-----------------------------------------------------------------------------------------------
static bool BuildCommandArgsAndValidate(
	std::string const& commandName,
	Strings const& argumentTokens,
	EventArgs& out_args,
	std::string& out_exampleOnError)
{
	DevConsoleCommandSpec const* spec = FindCommandSpec(commandName);
	if (spec == nullptr || spec->m_argumentCount == 0) {
		for (std::string const& token : argumentTokens) {
			std::string key;
			std::string value;
			if (!ParseArgumentToken(token, key, value)) {
				continue;
			}
			if (!key.empty()) {
				out_args.SetValue(key, value);
			}
		}
		return true;
	}

	out_exampleOnError = BuildCommandExample(*spec);
	if (argumentTokens.size() != static_cast<size_t>(spec->m_argumentCount)) {
		return false;
	}

	bool matchedArguments[MAX_COMMAND_ARGUMENTS] = {};
	for (std::string const& token : argumentTokens) {
		std::string key;
		std::string value;
		if (!ParseArgumentToken(token, key, value)) {
			return false;
		}

		std::string loweredKey = ToLowerCase(key);
		bool matched = false;
		for (int argumentIndex = 0; argumentIndex < spec->m_argumentCount; ++argumentIndex) {
			DevConsoleArgumentSpec const& argumentSpec = spec->m_arguments[argumentIndex];
			if (loweredKey != argumentSpec.m_lookupKeyLower) {
				continue;
			}
			if (matchedArguments[argumentIndex]) {
				return false;
			}
			if (!IsValueValidForArgumentType(value, argumentSpec.m_valueType)) {
				return false;
			}

			out_args.SetValue(argumentSpec.m_canonicalKey, value);
			matchedArguments[argumentIndex] = true;
			matched = true;
			break;
		}

		if (!matched) {
			return false;
		}
	}

	for (int argumentIndex = 0; argumentIndex < spec->m_argumentCount; ++argumentIndex) {
		if (!matchedArguments[argumentIndex]) {
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------------------------
static std::string GetHelpExampleForCommand(std::string const& commandName)
{
	DevConsoleCommandSpec const* spec = FindCommandSpec(commandName);
	if (spec != nullptr) {
		if (!spec->m_showExampleInHelp) {
			return "";
		}

		return BuildCommandExample(*spec);
	}

	std::string displayCommandName = GetCommandDisplayName(commandName);
	return Stringf("%s [key=value ...]", displayCommandName.c_str());
}

//-----------------------------------------------------------------------------------------------
static void ResetInsertionPointBlinkState(bool& insertionPointVisible, Timer* blinkTimer)
{
	insertionPointVisible = true;
	if (blinkTimer != nullptr) {
		blinkTimer->Stop();
		blinkTimer->Start();
	}
}

//-----------------------------------------------------------------------------------------------
static void ClampInsertionPointPosition(int& insertionPointPosition, std::string const& inputText)
{
	if (insertionPointPosition < 0) {
		insertionPointPosition = 0;
	}

	int maxIndex = static_cast<int>(inputText.size());
	if (insertionPointPosition > maxIndex) {
		insertionPointPosition = maxIndex;
	}
}

//-----------------------------------------------------------------------------------------------
DevConsole::DevConsole(DevConsoleConfig const& config)
	: m_config(config)
{
}

//-----------------------------------------------------------------------------------------------
DevConsole::~DevConsole()
{
	delete m_insertionPointBlinkTimer;
	m_insertionPointBlinkTimer = nullptr;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Startup()
{
	g_theDevConsole = this;

	m_isOpen = m_config.m_startOpen;
	m_lines.clear();
	m_inputText.clear();
	m_insertionPointPosition = 0;
	m_insertionPointVisible = true;
	m_commandHistory.clear();
	m_historyIndex = -1;

	delete m_insertionPointBlinkTimer;
	m_insertionPointBlinkTimer = new Timer(0.5, &Clock::GetSystemClock());
	m_insertionPointBlinkTimer->Start();

	SubscribeEventCallbackFunction("KeyPressed", DevConsole::Event_KeyPressed);
	SubscribeEventCallbackFunction("CharInput", DevConsole::Event_CharInput);
	SubscribeEventCallbackFunction("Clear", DevConsole::Command_Clear);
	SubscribeEventCallbackFunction("Help", DevConsole::Command_Help);

	AddLine(LOG_COLOR_INFO_MAJOR, "Type help for a list of commands");
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{
	UnsubscribeEventCallbackFunction("KeyPressed", DevConsole::Event_KeyPressed);
	UnsubscribeEventCallbackFunction("CharInput", DevConsole::Event_CharInput);
	UnsubscribeEventCallbackFunction("Clear", DevConsole::Command_Clear);
	UnsubscribeEventCallbackFunction("Help", DevConsole::Command_Help);

	if (g_theDevConsole == this) {
		g_theDevConsole = nullptr;
	}

	m_lines.clear();
	m_inputText.clear();
	m_commandHistory.clear();
	m_historyIndex = -1;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{
	if (m_insertionPointBlinkTimer != nullptr) {
		while (m_insertionPointBlinkTimer->DecrementPeriodIfElapsed()) {
			m_insertionPointVisible = !m_insertionPointVisible;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void DevConsole::EndFrame()
{
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Execute(std::string const& consoleCommandText, bool echoCommand)
{
	Strings lines = SplitStringOnDelimiter(consoleCommandText, '\n');
	for (std::string const& line : lines) {
		if (line.empty()) {
			continue;
		}

		Strings tokens = SplitStringOnDelimiter(line, ' ');
		std::string eventName;
		for (std::string const& token : tokens) {
			if (!token.empty()) {
				eventName = token;
				break;
			}
		}

		if (eventName.empty()) {
			continue;
		}

		bool eventNameConsumed = false;
		Strings argumentTokens;
		for (std::string const& token : tokens) {
			if (token.empty()) {
				continue;
			}
			if (!eventNameConsumed) {
				eventNameConsumed = true;
				continue;
			}

			argumentTokens.push_back(token);
		}

		if (echoCommand) {
			AddLine(INPUT_TEXT, line);
		}

		EventArgs args;
		std::string parameterExample;
		if (!BuildCommandArgsAndValidate(eventName, argumentTokens, args, parameterExample)) {
			AddLine(LOG_COLOR_ERROR, Stringf("Invalid arguments. Example: %s", parameterExample.c_str()));

			if (m_config.m_maxCommandHistory > 0) {
				m_commandHistory.push_back(line);
				if (static_cast<int>(m_commandHistory.size()) > m_config.m_maxCommandHistory) {
					m_commandHistory.erase(m_commandHistory.begin());
				}
			}
			m_historyIndex = -1;
			continue;
		}

		if (ShouldInjectSimulatedKeypressFlag(eventName)) {
			args.SetValue(SIMULATED_KEYPRESS_FROM_CONSOLE_ARG, "true");
		}

		FireEvent(eventName, args);

		if (m_config.m_maxCommandHistory > 0) {
			m_commandHistory.push_back(line);
			if (static_cast<int>(m_commandHistory.size()) > m_config.m_maxCommandHistory) {
				m_commandHistory.erase(m_commandHistory.begin());
			}
		}
		m_historyIndex = -1;
	}
}

//-----------------------------------------------------------------------------------------------
void DevConsole::AddLine(Rgba8 const& color, std::string const& text, float cellAspectOverride)
{
	Strings splitLines = SplitStringOnDelimiter(text, '\n');
	int frameNumber = Clock::GetSystemClock().GetFrameCount();
	if (frameNumber > TIMESTAMP_MAX_FRAME_VALUE) {
		frameNumber = TIMESTAMP_MAX_FRAME_VALUE;
	}

	std::string timestampPrefix = Stringf("[%07i] ", frameNumber);
	std::string continuationPrefix(timestampPrefix.size(), ' ');
	bool isFirstLine = true;

	for (std::string const& piece : splitLines) {
		DevConsoleLine line;
		line.m_color = color;
		line.m_cellAspectOverride = cellAspectOverride;
		if (isFirstLine) {
			line.m_text = timestampPrefix + piece;
			isFirstLine = false;
		}
		else {
			line.m_text = continuationPrefix + piece;
		}
		m_lines.push_back(line);
	}
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Render(AABB2 const& bounds)
{
	if (g_engine == nullptr || g_engine->m_render == nullptr) {
		return;
	}

	BitmapFont* font = g_engine->m_render->CreateOrGetBitmapFont(m_config.m_fontName.c_str());
	if (font == nullptr) {
		return;
	}

	Render(bounds, *font, m_config.m_fontAspect);
}

//-----------------------------------------------------------------------------------------------
void DevConsole::Render(AABB2 const& bounds, BitmapFont& bitmapFont, float fontAspect) const
{
	if (!m_isOpen) {
		return;
	}
	if (g_engine == nullptr || g_engine->m_render == nullptr) {
		return;
	}

	std::vector<Vertex> bgVerts;
	std::vector<Vertex> textVerts;
	std::vector<Vertex> insertionVerts;

	// DevConsole overlay must always use alpha blending regardless of previous draw state.
	g_engine->m_render->SetBlendMode(BlendMode::ALPHA);
	g_engine->m_render->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_engine->m_render->SetDepthMode(DepthMode::DISABLED);
	AddVertsForAABB2D(bgVerts, bounds, Rgba8(0, 0, 0, 160));

	int lineCount = m_config.m_linesOnScreen;
	if (lineCount < 2) {
		lineCount = 2;
	}

	float lineHeight = bounds.GetDimensions().y / static_cast<float>(lineCount);
	float textCellHeight = lineHeight * 0.72f;

	AABB2 inputLineBounds(
		bounds.m_mins.x + 8.0f,
		bounds.m_mins.y,
		bounds.m_maxs.x - 8.0f,
		bounds.m_mins.y + lineHeight);

	std::string inputLine = Stringf("> %s", m_inputText.c_str());
	bitmapFont.AddVertsForTextInBox2D(
		textVerts,
		inputLine,
		inputLineBounds,
		textCellHeight,
		INPUT_TEXT,
		fontAspect,
		Vec2(0.0f, 0.5f),
		TextBoxMode::OVERRUN);

	if (m_insertionPointVisible) {
		std::string leftText = Stringf("> %s", m_inputText.substr(0, static_cast<size_t>(m_insertionPointPosition)).c_str());
		float insertionX = inputLineBounds.m_mins.x + bitmapFont.GetTextWidth(textCellHeight, leftText, fontAspect);
		Vec2 insertionBottom(insertionX, inputLineBounds.m_mins.y + lineHeight * 0.20f);
		Vec2 insertionTop(insertionX, inputLineBounds.m_mins.y + lineHeight * 0.85f);
		AddVertsForLineSegment2D(insertionVerts, insertionBottom, insertionTop, 2.0f, INPUT_INSERTION_POINT);
	}

	int visibleLogLines = lineCount - 1;
	int totalLogLines = static_cast<int>(m_lines.size());

	for (int visibleIndex = 0; visibleIndex < visibleLogLines; ++visibleIndex) {
		int lineIndex = totalLogLines - 1 - visibleIndex;
		if (lineIndex < 0) {
			break;
		}

		float lineBottom = bounds.m_mins.y + lineHeight * static_cast<float>(visibleIndex + 1);
		AABB2 lineBounds(
			bounds.m_mins.x + 8.0f,
			lineBottom,
			bounds.m_maxs.x - 8.0f,
			lineBottom + lineHeight);

		DevConsoleLine const& line = m_lines[static_cast<size_t>(lineIndex)];
		std::string timestampText = line.m_text;
		std::string messageText;
		if (line.m_text.size() > static_cast<size_t>(TIMESTAMP_COLUMN_CHAR_COUNT)) {
			timestampText = line.m_text.substr(0, static_cast<size_t>(TIMESTAMP_COLUMN_CHAR_COUNT));
			messageText = line.m_text.substr(static_cast<size_t>(TIMESTAMP_COLUMN_CHAR_COUNT));
		}
		float timestampColumnWidth = bitmapFont.GetTextWidth(textCellHeight, TIMESTAMP_COLUMN_SAMPLE, fontAspect);
		float messageMinsX = lineBounds.m_mins.x + timestampColumnWidth;
		if (messageMinsX > lineBounds.m_maxs.x) {
			messageMinsX = lineBounds.m_maxs.x;
		}

		AABB2 timestampBounds(
			lineBounds.m_mins.x,
			lineBounds.m_mins.y,
			lineBounds.m_mins.x + timestampColumnWidth,
			lineBounds.m_maxs.y);
		AABB2 messageBounds(
			messageMinsX,
			lineBounds.m_mins.y,
			lineBounds.m_maxs.x,
			lineBounds.m_maxs.y);

		float messageAspect = (line.m_cellAspectOverride > 0.0f) ? line.m_cellAspectOverride : fontAspect;
		bitmapFont.AddVertsForTextInBox2D(
			textVerts,
			timestampText,
			timestampBounds,
			textCellHeight,
			TIMESTAMP,
			fontAspect,
			Vec2(0.0f, 0.5f),
			TextBoxMode::OVERRUN);
		bitmapFont.AddVertsForTextInBox2D(
			textVerts,
			messageText,
			messageBounds,
			textCellHeight,
			line.m_color,
			messageAspect,
			Vec2(0.0f, 0.5f),
			TextBoxMode::OVERRUN);
	}

	g_engine->m_render->BindTexture(nullptr);
	g_engine->m_render->DrawVertexArray(bgVerts);

	g_engine->m_render->BindTexture(&bitmapFont.GetTexture());
	g_engine->m_render->DrawVertexArray(textVerts);

	g_engine->m_render->BindTexture(nullptr);
	g_engine->m_render->DrawVertexArray(insertionVerts);
}

//-----------------------------------------------------------------------------------------------
void DevConsole::ToggleOpen()
{
	m_isOpen = !m_isOpen;
	ResetInsertionPointBlinkState(m_insertionPointVisible, m_insertionPointBlinkTimer);
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::IsOpen() const
{
	return m_isOpen;
}

//-----------------------------------------------------------------------------------------------
DevConsoleMode DevConsole::GetMode() const
{
	return m_isOpen ? DevConsoleMode::OPEN_FULL : DevConsoleMode::HIDDEN;
}

//-----------------------------------------------------------------------------------------------
void DevConsole::SetMode(DevConsoleMode mode)
{
	m_isOpen = (mode != DevConsoleMode::HIDDEN);
	ResetInsertionPointBlinkState(m_insertionPointVisible, m_insertionPointBlinkTimer);
}

//-----------------------------------------------------------------------------------------------
void DevConsole::ToggleMode(DevConsoleMode mode)
{
	if (GetMode() == mode) {
		SetMode(DevConsoleMode::HIDDEN);
	}
	else {
		SetMode(mode);
	}
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Event_KeyPressed(EventArgs& args)
{
	if (g_theDevConsole == nullptr) {
		return false;
	}

	int keyCodeInt = args.GetValue("KeyCode", -1);
	if (keyCodeInt < 0 || keyCodeInt >= NUM_KEYCODES) {
		return false;
	}

	unsigned char keyCode = static_cast<unsigned char>(keyCodeInt);
	bool isSimulatedKeypressFromCommand = args.GetValue(SIMULATED_KEYPRESS_FROM_CONSOLE_ARG, false);
	if (isSimulatedKeypressFromCommand) {
		return false;
	}

	if (keyCode == KEYCODE_TILDE) {
		g_theDevConsole->ToggleOpen();
		return true;
	}

	if (!g_theDevConsole->m_isOpen) {
		return false;
	}

	if (keyCode == KEYCODE_V && (GetKeyState(VK_CONTROL) & 0x8000) != 0) {
		HWND windowHandle = nullptr;
		if (g_engine != nullptr && g_engine->m_window != nullptr) {
			windowHandle = static_cast<HWND>(g_engine->m_window->GetHwnd());
		}

		if (windowHandle != nullptr && IsClipboardFormatAvailable(CF_TEXT) && OpenClipboard(windowHandle)) {
			HGLOBAL clipboardData = GetClipboardData(CF_TEXT);
			if (clipboardData != nullptr) {
				char const* clipboardText = static_cast<char const*>(GlobalLock(clipboardData));
				if (clipboardText != nullptr) {
					ClampInsertionPointPosition(g_theDevConsole->m_insertionPointPosition, g_theDevConsole->m_inputText);
					for (char const* textPtr = clipboardText; *textPtr != '\0'; ++textPtr) {
						unsigned char pastedCharacter = static_cast<unsigned char>(*textPtr);
						if (!IsValidDevConsoleInputCharacter(pastedCharacter)) {
							continue;
						}

						size_t insertIndex = static_cast<size_t>(g_theDevConsole->m_insertionPointPosition);
						g_theDevConsole->m_inputText.insert(insertIndex, 1, static_cast<char>(pastedCharacter));
						++g_theDevConsole->m_insertionPointPosition;
					}
					GlobalUnlock(clipboardData);
				}
			}
			CloseClipboard();
		}
	}
	else if (keyCode == KEYCODE_ESC) {
		if (g_theDevConsole->m_inputText.empty()) {
			g_theDevConsole->m_isOpen = false;
		}
		else {
			g_theDevConsole->m_inputText.clear();
			g_theDevConsole->m_insertionPointPosition = 0;
		}
	}
	else if (keyCode == KEYCODE_ENTER) {
		if (g_theDevConsole->m_inputText.empty()) {
			g_theDevConsole->m_isOpen = false;
		}
		else {
			g_theDevConsole->Execute(g_theDevConsole->m_inputText);
			g_theDevConsole->m_inputText.clear();
			g_theDevConsole->m_insertionPointPosition = 0;
			g_theDevConsole->m_historyIndex = -1;
		}
	}
	else if (keyCode == KEYCODE_LEFTARROW) {
		--g_theDevConsole->m_insertionPointPosition;
	}
	else if (keyCode == KEYCODE_RIGHTARROW) {
		++g_theDevConsole->m_insertionPointPosition;
	}
	else if (keyCode == KEYCODE_HOME) {
		g_theDevConsole->m_insertionPointPosition = 0;
	}
	else if (keyCode == KEYCODE_END) {
		g_theDevConsole->m_insertionPointPosition = static_cast<int>(g_theDevConsole->m_inputText.size());
	}
	else if (keyCode == KEYCODE_BACKSPACE) {
		if (g_theDevConsole->m_insertionPointPosition > 0 &&
			!g_theDevConsole->m_inputText.empty())
		{
			int eraseIndex = g_theDevConsole->m_insertionPointPosition - 1;
			g_theDevConsole->m_inputText.erase(static_cast<size_t>(eraseIndex), 1);
			--g_theDevConsole->m_insertionPointPosition;
		}
	}
	else if (keyCode == KEYCODE_DELETE) {
		if (g_theDevConsole->m_insertionPointPosition < static_cast<int>(g_theDevConsole->m_inputText.size())) {
			g_theDevConsole->m_inputText.erase(static_cast<size_t>(g_theDevConsole->m_insertionPointPosition), 1);
		}
	}
	else if (keyCode == KEYCODE_UPARROW) {
		if (!g_theDevConsole->m_commandHistory.empty()) {
			if (g_theDevConsole->m_historyIndex < 0) {
				g_theDevConsole->m_historyIndex = static_cast<int>(g_theDevConsole->m_commandHistory.size()) - 1;
			}
			else if (g_theDevConsole->m_historyIndex > 0) {
				--g_theDevConsole->m_historyIndex;
			}
			g_theDevConsole->m_inputText = g_theDevConsole->m_commandHistory[static_cast<size_t>(g_theDevConsole->m_historyIndex)];
			g_theDevConsole->m_insertionPointPosition = static_cast<int>(g_theDevConsole->m_inputText.size());
		}
	}
	else if (keyCode == KEYCODE_DOWNARROW) {
		if (!g_theDevConsole->m_commandHistory.empty()) {
			if (g_theDevConsole->m_historyIndex >= 0 &&
				g_theDevConsole->m_historyIndex < static_cast<int>(g_theDevConsole->m_commandHistory.size()) - 1)
			{
				++g_theDevConsole->m_historyIndex;
				g_theDevConsole->m_inputText = g_theDevConsole->m_commandHistory[static_cast<size_t>(g_theDevConsole->m_historyIndex)];
			}
			else {
				g_theDevConsole->m_historyIndex = -1;
				g_theDevConsole->m_inputText.clear();
			}
			g_theDevConsole->m_insertionPointPosition = static_cast<int>(g_theDevConsole->m_inputText.size());
		}
	}

	ClampInsertionPointPosition(g_theDevConsole->m_insertionPointPosition, g_theDevConsole->m_inputText);
	ResetInsertionPointBlinkState(g_theDevConsole->m_insertionPointVisible, g_theDevConsole->m_insertionPointBlinkTimer);
	return true;
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Event_CharInput(EventArgs& args)
{
	if (g_theDevConsole == nullptr || !g_theDevConsole->m_isOpen) {
		return false;
	}

	int charCodeInt = args.GetValue("CharCode", -1);
	if (charCodeInt < 0 || charCodeInt > 255) {
		return true;
	}

	unsigned char character = static_cast<unsigned char>(charCodeInt);
	if (!IsValidDevConsoleInputCharacter(character)) {
		return true;
	}

	ClampInsertionPointPosition(g_theDevConsole->m_insertionPointPosition, g_theDevConsole->m_inputText);
	size_t insertIndex = static_cast<size_t>(g_theDevConsole->m_insertionPointPosition);
	g_theDevConsole->m_inputText.insert(insertIndex, 1, static_cast<char>(character));
	++g_theDevConsole->m_insertionPointPosition;
	ResetInsertionPointBlinkState(g_theDevConsole->m_insertionPointVisible, g_theDevConsole->m_insertionPointBlinkTimer);
	return true;
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Command_Clear([[maybe_unused]]EventArgs& args)
{
	if (g_theDevConsole == nullptr) {
		return false;
	}

	g_theDevConsole->m_lines.clear();
	return true;
}

//-----------------------------------------------------------------------------------------------
bool DevConsole::Command_Help([[maybe_unused]]EventArgs& args)
{
	if (g_theDevConsole == nullptr) {
		return false;
	}

	g_theDevConsole->AddLine(LOG_COLOR_INFO_MAJOR, "Registered Commands: ");
	Strings commands = GetRegisteredCommandNames();
	std::string commandBlockText;
	for (std::string const& command : commands) {
		if (!commandBlockText.empty()) {
			commandBlockText += "\n";
		}

		std::string displayCommandName = GetCommandDisplayName(command);
		std::string example = GetHelpExampleForCommand(command);
		if (example.empty()) {
			commandBlockText += displayCommandName;
		}
		else {
			commandBlockText += Stringf(
				"%-*s example: %s",
				HELP_COMMAND_NAME_WIDTH,
				displayCommandName.c_str(),
				example.c_str());
		}
	}

	if (!commandBlockText.empty()) {
		g_theDevConsole->AddLine(LOG_COLOR_INFO_MINOR, commandBlockText);
	}
	return true;
}
