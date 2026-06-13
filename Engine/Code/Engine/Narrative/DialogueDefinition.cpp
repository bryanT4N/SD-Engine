#include "Engine/Narrative/DialogueDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

//-----------------------------------------------------------------------------------------------
static std::string const RESERVED_END_TARGET = "end";

//-----------------------------------------------------------------------------------------------
bool DialogueDefinition::LoadFromXmlElement( XmlElement const& dialogueElement )
{
	m_id          = ParseXmlAttribute( dialogueElement, "id", std::string() );
	m_startNodeId = ParseXmlAttribute( dialogueElement, "start", std::string() );

	for( XmlElement const* nodeElement = dialogueElement.FirstChildElement();
		nodeElement != nullptr;
		nodeElement = nodeElement->NextSiblingElement() )
	{
		std::string elementName = nodeElement->Name();
		DialogueNode node;
		node.id      = ParseXmlAttribute( *nodeElement, "id", std::string() );
		node.speaker = ParseXmlAttribute( *nodeElement, "speaker", std::string() );

		if( elementName == "Line" )
		{
			node.kind = EDialogueNodeKind::LINE;
			node.next = ParseXmlAttribute( *nodeElement, "next", std::string() );
			char const* bodyText = nodeElement->GetText();
			node.body = ( bodyText != nullptr ) ? bodyText : "";
		}
		else if( elementName == "Choice" )
		{
			node.kind   = EDialogueNodeKind::CHOICE;
			node.prompt = ParseXmlAttribute( *nodeElement, "prompt", std::string() );
			for( XmlElement const* optionElement = nodeElement->FirstChildElement( "Option" );
				optionElement != nullptr;
				optionElement = optionElement->NextSiblingElement( "Option" ) )
			{
				DialogueOption option;
				option.text = ParseXmlAttribute( *optionElement, "text", std::string() );
				option.next = ParseXmlAttribute( *optionElement, "next", std::string() );
				node.options.push_back( option );
			}
		}
		else
		{
			DebuggerPrintf( "ERROR Dialogue '%s': unknown node element '%s'\n", m_id.c_str(), elementName.c_str() );
			continue;
		}

		m_nodes.push_back( node );
	}

	m_isValid = ValidateAndIndex();
	return m_isValid;
}

//-----------------------------------------------------------------------------------------------
DialogueNode const* DialogueDefinition::FindNode( std::string const& nodeId ) const
{
	auto found = m_nodeIndexById.find( nodeId );
	if( found == m_nodeIndexById.end() )
	{
		return nullptr;
	}
	return &m_nodes[static_cast<size_t>( found->second )];
}

//-----------------------------------------------------------------------------------------------
bool DialogueDefinition::IsLinkTargetReachable( std::string const& targetNodeId ) const
{
	if( targetNodeId.empty() || targetNodeId == RESERVED_END_TARGET )
	{
		return true;
	}
	return m_nodeIndexById.find( targetNodeId ) != m_nodeIndexById.end();
}

//-----------------------------------------------------------------------------------------------
bool DialogueDefinition::ValidateAndIndex()
{
	bool isValid = true;

	for( int nodeIndex = 0; nodeIndex < static_cast<int>( m_nodes.size() ); ++nodeIndex )
	{
		std::string const& nodeId = m_nodes[static_cast<size_t>( nodeIndex )].id;
		if( nodeId.empty() )
		{
			DebuggerPrintf( "ERROR Dialogue '%s': node has empty id\n", m_id.c_str() );
			isValid = false;
			continue;
		}
		if( m_nodeIndexById.find( nodeId ) != m_nodeIndexById.end() )
		{
			DebuggerPrintf( "ERROR Dialogue '%s': duplicate node id '%s'\n", m_id.c_str(), nodeId.c_str() );
			isValid = false;
			continue;
		}
		m_nodeIndexById[nodeId] = nodeIndex;
	}

	if( !IsLinkTargetReachable( m_startNodeId ) || m_startNodeId.empty() )
	{
		DebuggerPrintf( "ERROR Dialogue '%s': start node '%s' not found\n", m_id.c_str(), m_startNodeId.c_str() );
		isValid = false;
	}

	for( DialogueNode const& node : m_nodes )
	{
		if( node.kind == EDialogueNodeKind::LINE )
		{
			if( !IsLinkTargetReachable( node.next ) )
			{
				DebuggerPrintf( "ERROR Dialogue '%s': node '%s' next='%s' not found\n", m_id.c_str(), node.id.c_str(), node.next.c_str() );
				isValid = false;
			}
		}
		else
		{
			for( DialogueOption const& option : node.options )
			{
				if( !IsLinkTargetReachable( option.next ) )
				{
					DebuggerPrintf( "ERROR Dialogue '%s': node '%s' option next='%s' not found\n", m_id.c_str(), node.id.c_str(), option.next.c_str() );
					isValid = false;
				}
			}
		}
	}

	return isValid;
}
