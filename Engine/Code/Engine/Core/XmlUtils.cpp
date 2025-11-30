#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <cstdlib>
#include <string>

//-----------------------------------------------------------------------------------------------
static char const* GetXmlAttributeText(XmlElement const& element, char const* attributeName) {
	return element.Attribute(attributeName);
}

//-----------------------------------------------------------------------------------------------
int ParseXmlAttribute(XmlElement const& element, char const* attributeName, int defaultValue)
{
	char const* text = GetXmlAttributeText(element, attributeName);
	if (text == nullptr) {
		return defaultValue;
	}
	return std::atoi(text);
}

char ParseXmlAttribute(XmlElement const& element, char const* attributeName, char defaultValue)
{
	char const* text = GetXmlAttributeText(element, attributeName);
	if (text == nullptr || text[0] == '\0') {
		return defaultValue;
	}
	return text[0];
}

bool ParseXmlAttribute(XmlElement const& element, char const* attributeName, bool defaultValue)
{
	char const* text = GetXmlAttributeText(element, attributeName);
	if (text == nullptr)
	{
		return defaultValue;
	}

	std::string valueText(text);
	for (std::size_t i = 0; i < valueText.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(valueText[i]);
		valueText[i] = static_cast<char>(std::tolower(c));
	}

	if (valueText == "true")  return true;
	if (valueText == "false") return false;
	if (valueText == "1")     return true;
	if (valueText == "0")     return false;

	return defaultValue;
}

float ParseXmlAttribute(XmlElement const& element, char const* attributeName, float defaultValue)
{
	char const* text = GetXmlAttributeText(element, attributeName);
	if (text == nullptr) {
		return defaultValue;
	}
	return static_cast<float>(std::atof(text));
}

Rgba8 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue)
{
	char const* text = GetXmlAttributeText(element, attributeName);
	if (text == nullptr) {
		return defaultValue;
	}

	Rgba8 value = defaultValue;
	value.SetFromText(text);
	return value;
}

Vec2 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec2 const& defaultValue)
{
	char const* text = GetXmlAttributeText(element, attributeName);
	if (text == nullptr) {
		return defaultValue;
	}

	Vec2 value = defaultValue;
	value.SetFromText(text);
	return value;
}

IntVec2 ParseXmlAttribute(XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue)
{
	char const* text = GetXmlAttributeText(element, attributeName);
	if (text == nullptr) {
		return defaultValue;
	}

	IntVec2 value = defaultValue;
	value.SetFromText(text);
	return value;
}

std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, std::string const& defaultValue)
{
	char const* text = GetXmlAttributeText(element, attributeName);
	if (text == nullptr) {
		return defaultValue;
	}
	return std::string(text);
}

Strings ParseXmlAttribute(XmlElement const& element, char const* attributeName, Strings const& defaultValues, char delimiter)
{
	char const* text = GetXmlAttributeText(element, attributeName);
	if (text == nullptr) {
		return defaultValues;
	}

	return SplitStringOnDelimiter(std::string(text), delimiter);
}

// a custom special-case function for getting an attribute as a std::string
std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, char const* defaultValue)
{
	return ParseXmlAttribute(element, attributeName, std::string(defaultValue));
}