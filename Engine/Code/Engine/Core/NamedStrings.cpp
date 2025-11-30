#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <cstdlib>
#include <string>

void NamedStrings::PopulateFromXmlElementAttributes(XmlElement const& element)
{
	XmlAttribute const* attr = element.FirstAttribute();
	while (attr != nullptr)
	{
		const char* name = attr->Name();
		const char* value = attr->Value();
		if (name && value)
		{
			SetValue(std::string(name), std::string(value));
		}
		attr = attr->Next();
	}
}

void NamedStrings::SetValue(std::string const& keyName, std::string const& newValue)
{
	m_keyValuePairs[keyName] = newValue;
}

std::string NamedStrings::GetValue(std::string const& keyName, std::string const& defaultValue) const
{
	std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	return iter->second;
}

bool NamedStrings::GetValue(std::string const& keyName, bool defaultValue) const
{
	std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	std::string text = iter->second;
	for (std::size_t i = 0; i < text.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(text[i]);
		text[i] = static_cast<char>(std::tolower(c));
	}

	if (text == "true")  return true;
	if (text == "false") return false;
	if (text == "1")     return true;
	if (text == "0")     return false;

	return defaultValue;
}

int NamedStrings::GetValue(std::string const& keyName, int defaultValue) const
{
	std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	return std::atoi(iter->second.c_str());
}

float NamedStrings::GetValue(std::string const& keyName, float defaultValue) const
{
	std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end())
	{
		return defaultValue;
	}
	return static_cast<float>(std::atof(iter->second.c_str()));
}

std::string NamedStrings::GetValue(std::string const& keyName, char const* defaultValue) const
{
	return GetValue(keyName, std::string(defaultValue));
}

Rgba8 NamedStrings::GetValue(std::string const& keyName, Rgba8 const& defaultValue) const
{
	std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	Rgba8 value = defaultValue;
	value.SetFromText(iter->second.c_str());
	return value;
}

Vec2 NamedStrings::GetValue(std::string const& keyName, Vec2 const& defaultValue) const
{
	std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	Vec2 value = defaultValue;
	value.SetFromText(iter->second.c_str());
	return value;
}

IntVec2 NamedStrings::GetValue(std::string const& keyName, IntVec2 const& defaultValue) const
{
	std::map< std::string, std::string >::const_iterator iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	IntVec2 value = defaultValue;
	value.SetFromText(iter->second.c_str());
	return value;
}