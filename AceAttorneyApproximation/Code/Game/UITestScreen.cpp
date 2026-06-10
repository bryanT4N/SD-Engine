#include "Game/UITestScreen.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/UI/UIButton.hpp"
#include "Engine/UI/UILabel.hpp"
#include "Engine/UI/UIPanel.hpp"
#include "Engine/UI/UITheme.hpp"

#include <memory>

//-----------------------------------------------------------------------------------------------
void UITestScreen::Build(UITheme const* theme)
{
	auto root = std::make_unique<UIPanel>();
	root->m_bgColor = Rgba8(40, 40, 60, 80);
	root->m_borderColor = Rgba8(120, 140, 200, 200);
	root->m_borderThickness = 2.f;

	auto title = std::make_unique<UILabel>();
	title->m_text = "UI Framework Test";
	title->m_font = theme->defaultFont;
	title->m_textHeight = 32.f;
	title->m_color = theme->textColor;

	UIAnchorLayout titleLayout;
	titleLayout.anchorMin = Vec2(0.5f, 1.f);
	titleLayout.anchorMax = Vec2(0.5f, 1.f);
	titleLayout.pivot = Vec2(0.5f, 1.f);
	titleLayout.sizeDelta = Vec2(420.f, 40.f);
	titleLayout.offsetMin = Vec2(0.f, -40.f);
	titleLayout.offsetMax = Vec2::ZERO;
	root->AddChild(std::move(title), titleLayout);

	auto button = std::make_unique<UIButton>();
	button->m_colorByState[0] = theme->buttonNormal;
	button->m_colorByState[1] = theme->buttonHover;
	button->m_colorByState[2] = theme->buttonPressed;
	button->m_colorByState[3] = theme->buttonDisabled;
	button->m_onClick = []() {
		if (g_engine != nullptr && g_engine->m_devConsole != nullptr) {
			g_engine->m_devConsole->AddLine(DevConsole::LOG_COLOR_INFO_MAJOR, "Button clicked!");
		}
	};

	auto buttonLabel = std::make_unique<UILabel>();
	buttonLabel->m_text = "Click Me";
	buttonLabel->m_font = theme->defaultFont;
	buttonLabel->m_textHeight = 24.f;
	buttonLabel->m_color = theme->textColor;
	button->AddChild(std::move(buttonLabel), UIAnchorLayout::Centered(160.f, 32.f));

	root->AddChild(std::move(button), UIAnchorLayout::Centered(220.f, 60.f));

	m_uiSystem.SetScreenBounds(AABB2(Vec2::ZERO, Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y)));
	m_uiSystem.SetRoot(std::move(root));
}

//-----------------------------------------------------------------------------------------------
void UITestScreen::Update()
{
	if (g_engine == nullptr || g_engine->m_input == nullptr) return;
	InputSystem* input = g_engine->m_input;

	Vec2 normalizedPos = input->GetCursorNormalizedPosition();
	Vec2 screenPos(normalizedPos.x * SCREEN_SIZE_X, normalizedPos.y * SCREEN_SIZE_Y);

	m_uiSystem.Update();
	m_uiSystem.OnMouseMove(screenPos);

	if (input->WasKeyJustPressed(KEYCODE_LBUTTON)) {
		m_uiSystem.OnMouseDown(screenPos);
	}
	if (input->WasKeyJustReleased(KEYCODE_LBUTTON)) {
		m_uiSystem.OnMouseUp(screenPos);
	}
}

//-----------------------------------------------------------------------------------------------
void UITestScreen::Render(Renderer& renderer) const
{
	m_uiSystem.Render(renderer);
}
