#pragma once
#include <string>

//-----------------------------------------------------------------------------------------------
class DialogueDefinition;
struct DialogueNode;
class NamedStrings;

//-----------------------------------------------------------------------------------------------
enum class EDialogueWalkerState
{
	INACTIVE,
	AT_LINE,
	AT_CHOICE,
	FINISHED
};

//-----------------------------------------------------------------------------------------------
class DialogueWalker
{
public:
	void					Start( DialogueDefinition const* definition, NamedStrings* worldState );
	DialogueNode const*		GetCurrentNode() const { return m_currentNode; }
	std::string const&		GetCurrentNodeId() const;
	bool					IsAtChoice() const { return m_state == EDialogueWalkerState::AT_CHOICE; }
	bool					IsFinished() const { return m_state == EDialogueWalkerState::FINISHED; }
	void					Advance();
	void					Choose( int optionIndex );

private:
	void					EnterNode( std::string const& nodeId );

	DialogueDefinition const*	m_definition = nullptr;
	NamedStrings*				m_worldState = nullptr;
	DialogueNode const*			m_currentNode = nullptr;
	EDialogueWalkerState		m_state = EDialogueWalkerState::INACTIVE;
};
