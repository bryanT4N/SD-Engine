#pragma once
#include "Engine/Narrative/DialogueDefinition.hpp"

#include <map>
#include <memory>
#include <string>

//-----------------------------------------------------------------------------------------------
class DialogueDatabase
{
public:
	void						LoadFile( std::string const& xmlFilePath );
	DialogueDefinition const*	Find( std::string const& dialogueId ) const;

private:
	std::map<std::string, std::unique_ptr<DialogueDefinition>>	m_definitionsById;
};
