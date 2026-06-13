#include "Game/DialogueScreen.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Narrative/DialogueDefinition.hpp"
#include "Engine/UI/UIAnchorLayout.hpp"
#include "Engine/UI/UIButton.hpp"
#include "Engine/UI/UILabel.hpp"
#include "Engine/UI/UIPanel.hpp"
#include "Engine/UI/UITheme.hpp"

#include <memory>

//-----------------------------------------------------------------------------------------------
constexpr float DIALOGUE_BAND_MARGIN = 40.f;
constexpr float DIALOGUE_BAND_TOP = 320.f;
constexpr float DIALOGUE_CHOICE_BUTTON_HEIGHT = 32.f;
constexpr float DIALOGUE_CHOICE_BUTTON_SPACING = 8.f;
constexpr float DIALOGUE_CHOICE_FIRST_OFFSET = 100.f;

//-----------------------------------------------------------------------------------------------
void DialogueScreen::Build( UITheme const* theme )
{
	m_theme = theme;
	m_uiSystem.SetScreenBounds(
		AABB2( Vec2( DIALOGUE_BAND_MARGIN, DIALOGUE_BAND_MARGIN ),
			   Vec2( SCREEN_SIZE_X - DIALOGUE_BAND_MARGIN, DIALOGUE_BAND_TOP ) ) );
}

//-----------------------------------------------------------------------------------------------
void DialogueScreen::Open( DialogueDefinition const* definition, NamedStrings* worldState )
{
	m_pendingChoiceIndex = -1;
	m_justOpened = true;
	m_walker.Start( definition, worldState );
	EnterCurrentNode();
}

//-----------------------------------------------------------------------------------------------
bool DialogueScreen::IsActive() const
{
	return m_state == EDialoguePresentState::SHOWING_LINE || m_state == EDialoguePresentState::SHOWING_CHOICES;
}

//-----------------------------------------------------------------------------------------------
void DialogueScreen::Update( float deltaSeconds )
{
	(void)deltaSeconds;
	if( !IsActive() )
	{
		return;
	}

	if( m_pendingChoiceIndex >= 0 )
	{
		int chosenOptionIndex = m_pendingChoiceIndex;
		m_pendingChoiceIndex = -1;
		m_walker.Choose( chosenOptionIndex );
		EnterCurrentNode();
		if( !IsActive() )
		{
			return;
		}
	}

	if( g_engine == nullptr || g_engine->m_input == nullptr )
	{
		return;
	}
	InputSystem* input = g_engine->m_input;
	Vec2 normalizedCursor = input->GetCursorNormalizedPosition();
	Vec2 cursorScreenPos( normalizedCursor.x * SCREEN_SIZE_X, normalizedCursor.y * SCREEN_SIZE_Y );

	m_uiSystem.Update();
	m_uiSystem.OnMouseMove( cursorScreenPos );

	if( m_justOpened )
	{
		m_justOpened = false;
		return;
	}

	if( m_state == EDialoguePresentState::SHOWING_LINE )
	{
		if( input->WasKeyJustPressed( KEYCODE_LBUTTON ) || input->WasKeyJustPressed( KEYCODE_SPACE ) )
		{
			m_walker.Advance();
			EnterCurrentNode();
		}
	}
	else
	{
		if( input->WasKeyJustPressed( KEYCODE_LBUTTON ) )
		{
			m_uiSystem.OnMouseDown( cursorScreenPos );
		}
		if( input->WasKeyJustReleased( KEYCODE_LBUTTON ) )
		{
			m_uiSystem.OnMouseUp( cursorScreenPos );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void DialogueScreen::Render( Renderer& renderer ) const
{
	m_uiSystem.Render( renderer );
}

//-----------------------------------------------------------------------------------------------
void DialogueScreen::EnterCurrentNode()
{
	if( m_walker.IsFinished() )
	{
		m_state = EDialoguePresentState::FINISHED;
		m_uiSystem.SetRoot( nullptr );
		return;
	}

	DialogueNode const* node = m_walker.GetCurrentNode();
	bool isChoiceNode = m_walker.IsAtChoice();
	m_state = isChoiceNode ? EDialoguePresentState::SHOWING_CHOICES : EDialoguePresentState::SHOWING_LINE;

	auto root = std::make_unique<UIPanel>();
	root->m_bgColor = Rgba8( 18, 20, 38, 235 );
	root->m_borderColor = Rgba8( 120, 140, 200, 255 );
	root->m_borderThickness = 3.f;

	auto speakerLabel = std::make_unique<UILabel>();
	speakerLabel->m_font = m_theme->defaultFont;
	speakerLabel->m_textHeight = 24.f;
	speakerLabel->m_color = Rgba8( 200, 220, 255, 255 );
	speakerLabel->m_text = node->speaker;
	UIAnchorLayout speakerLayout;
	speakerLayout.anchorMin = Vec2( 0.f, 1.f );
	speakerLayout.anchorMax = Vec2( 0.f, 1.f );
	speakerLayout.pivot = Vec2( 0.f, 1.f );
	speakerLayout.sizeDelta = Vec2( 500.f, 28.f );
	speakerLayout.offsetMin = Vec2( 20.f, -14.f );
	speakerLayout.offsetMax = Vec2::ZERO;
	root->AddChild( std::move( speakerLabel ), speakerLayout );

	auto bodyLabel = std::make_unique<UILabel>();
	bodyLabel->m_font = m_theme->defaultFont;
	bodyLabel->m_textHeight = 20.f;
	bodyLabel->m_color = m_theme->textColor;
	bodyLabel->m_wrapText = true;
	bodyLabel->m_text = isChoiceNode ? node->prompt : node->body;
	UIAnchorLayout bodyLayout;
	bodyLayout.anchorMin = Vec2( 0.f, isChoiceNode ? 1.f : 0.f );
	bodyLayout.anchorMax = Vec2( 1.f, 1.f );
	if( isChoiceNode )
	{
		bodyLayout.pivot = Vec2( 0.f, 1.f );
		bodyLayout.sizeDelta = Vec2( 0.f, 44.f );
		bodyLayout.offsetMin = Vec2( 20.f, -50.f );
		bodyLayout.offsetMax = Vec2( 20.f, 0.f );
	}
	else
	{
		bodyLayout.offsetMin = Vec2( 20.f, 20.f );
		bodyLayout.offsetMax = Vec2( 20.f, 56.f );
	}
	root->AddChild( std::move( bodyLabel ), bodyLayout );

	if( isChoiceNode )
	{
		for( int optionIndex = 0; optionIndex < static_cast<int>( node->options.size() ); ++optionIndex )
		{
			auto optionButton = std::make_unique<UIButton>();
			optionButton->m_colorByState[0] = m_theme->buttonNormal;
			optionButton->m_colorByState[1] = m_theme->buttonHover;
			optionButton->m_colorByState[2] = m_theme->buttonPressed;
			optionButton->m_colorByState[3] = m_theme->buttonDisabled;
			int capturedOptionIndex = optionIndex;
			optionButton->m_onClick = [this, capturedOptionIndex]() { m_pendingChoiceIndex = capturedOptionIndex; };

			auto optionLabel = std::make_unique<UILabel>();
			optionLabel->m_font = m_theme->defaultFont;
			optionLabel->m_textHeight = 18.f;
			optionLabel->m_color = m_theme->textColor;
			optionLabel->m_text = node->options[static_cast<size_t>( optionIndex )].text;
			optionButton->AddChild( std::move( optionLabel ), UIAnchorLayout::Centered( 600.f, 22.f ) );

			UIAnchorLayout buttonLayout;
			buttonLayout.anchorMin = Vec2( 0.f, 1.f );
			buttonLayout.anchorMax = Vec2( 1.f, 1.f );
			buttonLayout.pivot = Vec2( 0.f, 1.f );
			buttonLayout.sizeDelta = Vec2( 0.f, DIALOGUE_CHOICE_BUTTON_HEIGHT );
			float stackOffset = DIALOGUE_CHOICE_FIRST_OFFSET +
				static_cast<float>( optionIndex ) * ( DIALOGUE_CHOICE_BUTTON_HEIGHT + DIALOGUE_CHOICE_BUTTON_SPACING );
			buttonLayout.offsetMin = Vec2( 20.f, -stackOffset );
			buttonLayout.offsetMax = Vec2( 20.f, 0.f );
			root->AddChild( std::move( optionButton ), buttonLayout );
		}
	}

	m_uiSystem.SetRoot( std::move( root ) );
}
