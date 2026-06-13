#pragma once
#include "Engine/UI/UISystem.hpp"
#include "Engine/Narrative/DialogueWalker.hpp"

//-----------------------------------------------------------------------------------------------
struct UITheme;
class Renderer;
class DialogueDefinition;
class NamedStrings;

//-----------------------------------------------------------------------------------------------
enum class EDialoguePresentState
{
	IDLE,
	SHOWING_LINE,
	SHOWING_CHOICES,
	FINISHED
};

//-----------------------------------------------------------------------------------------------
class DialogueScreen
{
public:
	void	Build( UITheme const* theme );
	void	Open( DialogueDefinition const* definition, NamedStrings* worldState );
	void	Update( float deltaSeconds );
	void	Render( Renderer& renderer ) const;
	bool	IsActive() const;

private:
	void	EnterCurrentNode();

	UISystem				m_uiSystem;
	DialogueWalker			m_walker;
	UITheme const*			m_theme = nullptr;
	EDialoguePresentState	m_state = EDialoguePresentState::IDLE;
	int						m_pendingChoiceIndex = -1;
	bool					m_justOpened = false;
};
