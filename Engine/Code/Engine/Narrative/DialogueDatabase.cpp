#include "Engine/Narrative/DialogueDatabase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void DialogueDatabase::LoadFile( std::string const& xmlFilePath )
{
	XmlDocument document;
	XmlResult loadResult = document.LoadFile( xmlFilePath.c_str() );
	if( loadResult != tinyxml2::XML_SUCCESS )
	{
		DebuggerPrintf( "ERROR DialogueDatabase: failed to load '%s'\n", xmlFilePath.c_str() );
		return;
	}

	XmlElement const* rootElement = document.RootElement();
	if( rootElement == nullptr )
	{
		DebuggerPrintf( "ERROR DialogueDatabase: '%s' has no root element\n", xmlFilePath.c_str() );
		return;
	}

	auto definition = std::make_unique<DialogueDefinition>();
	if( !definition->LoadFromXmlElement( *rootElement ) )
	{
		DebuggerPrintf( "ERROR DialogueDatabase: '%s' invalid, keeping previous if any\n", xmlFilePath.c_str() );
		return;
	}

	m_definitionsById[definition->GetId()] = std::move( definition );
}

//-----------------------------------------------------------------------------------------------
DialogueDefinition const* DialogueDatabase::Find( std::string const& dialogueId ) const
{
	auto found = m_definitionsById.find( dialogueId );
	if( found == m_definitionsById.end() )
	{
		return nullptr;
	}
	return found->second.get();
}
