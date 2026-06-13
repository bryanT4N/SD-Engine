#include "Engine/Narrative/DialogueWalker.hpp"
#include "Engine/Narrative/DialogueDefinition.hpp"

//-----------------------------------------------------------------------------------------------
static std::string const s_noCurrentNodeId;

//-----------------------------------------------------------------------------------------------
void DialogueWalker::Start( DialogueDefinition const* definition, NamedStrings* worldState )
{
	m_definition  = definition;
	m_worldState  = worldState;
	m_currentNode = nullptr;
	m_state       = EDialogueWalkerState::INACTIVE;

	if( m_definition == nullptr || !m_definition->IsValid() )
	{
		m_state = EDialogueWalkerState::FINISHED;
		return;
	}

	EnterNode( m_definition->GetStartNodeId() );
}

//-----------------------------------------------------------------------------------------------
std::string const& DialogueWalker::GetCurrentNodeId() const
{
	return ( m_currentNode != nullptr ) ? m_currentNode->id : s_noCurrentNodeId;
}

//-----------------------------------------------------------------------------------------------
void DialogueWalker::Advance()
{
	if( m_state != EDialogueWalkerState::AT_LINE || m_currentNode == nullptr )
	{
		return;
	}
	EnterNode( m_currentNode->next );
}

//-----------------------------------------------------------------------------------------------
void DialogueWalker::Choose( int optionIndex )
{
	if( m_state != EDialogueWalkerState::AT_CHOICE || m_currentNode == nullptr )
	{
		return;
	}
	if( optionIndex < 0 || optionIndex >= static_cast<int>( m_currentNode->options.size() ) )
	{
		return;
	}
	EnterNode( m_currentNode->options[static_cast<size_t>( optionIndex )].next );
}

//-----------------------------------------------------------------------------------------------
void DialogueWalker::EnterNode( std::string const& nodeId )
{
	if( m_definition == nullptr || nodeId.empty() || nodeId == "end" )
	{
		m_currentNode = nullptr;
		m_state = EDialogueWalkerState::FINISHED;
		return;
	}

	m_currentNode = m_definition->FindNode( nodeId );
	if( m_currentNode == nullptr )
	{
		m_state = EDialogueWalkerState::FINISHED;
		return;
	}

	m_state = ( m_currentNode->kind == EDialogueNodeKind::CHOICE )
		? EDialogueWalkerState::AT_CHOICE
		: EDialogueWalkerState::AT_LINE;
}
