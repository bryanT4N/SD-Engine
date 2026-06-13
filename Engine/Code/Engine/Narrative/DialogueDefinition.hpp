#pragma once
#include "Engine/Core/XmlUtils.hpp"

#include <map>
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
enum class EDialogueNodeKind
{
	LINE,
	CHOICE
};

//-----------------------------------------------------------------------------------------------
struct DialogueOption
{
	std::string text;
	std::string next;
};

//-----------------------------------------------------------------------------------------------
struct DialogueNode
{
	std::string					id;
	EDialogueNodeKind			kind = EDialogueNodeKind::LINE;
	std::string					speaker;
	std::string					body;
	std::string					prompt;
	std::string					next;
	std::vector<DialogueOption>	options;
};

//-----------------------------------------------------------------------------------------------
class DialogueDefinition
{
public:
	bool					LoadFromXmlElement( XmlElement const& dialogueElement );
	bool					IsValid() const { return m_isValid; }
	std::string const&		GetId() const { return m_id; }
	std::string const&		GetStartNodeId() const { return m_startNodeId; }
	DialogueNode const*		FindNode( std::string const& nodeId ) const;

private:
	bool					ValidateAndIndex();
	bool					IsLinkTargetReachable( std::string const& targetNodeId ) const;

	std::string					m_id;
	std::string					m_startNodeId;
	std::vector<DialogueNode>	m_nodes;
	std::map<std::string, int>	m_nodeIndexById;
	bool						m_isValid = false;
};
