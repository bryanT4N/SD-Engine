#define STB_IMAGE_IMPLEMENTATION // Exactly one .CPP (this Image.cpp) should #define this before #including stb_image.h
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"


Image::Image()
{
}


Image::~Image()
{
}


Image::Image(const char* imageFilePath)
	: m_imageFilePath(imageFilePath)
{
	int bytesPerTexel = 0;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* texelData = stbi_load(imageFilePath, &m_dimensions.x, &m_dimensions.y, &bytesPerTexel, 4);

	GUARANTEE_OR_DIE(texelData, Stringf("Failed to load image \"%s\"", imageFilePath));

	m_texelRgba8Data.resize(m_dimensions.x * m_dimensions.y);
	memcpy(m_texelRgba8Data.data(), texelData, m_dimensions.x * m_dimensions.y * 4);

	stbi_image_free(texelData);
}


Image::Image(IntVec2 size, Rgba8 color)
	: m_dimensions(size)
{
	int numTexels = size.x * size.y;
	m_texelRgba8Data.resize(numTexels);
	for (int i = 0; i < numTexels; ++i)
	{
		m_texelRgba8Data[i] = color;
	}
}


IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}


const std::string& Image::GetImageFilePath() const
{
	return m_imageFilePath;
}


const void* Image::GetRawData() const
{
	return m_texelRgba8Data.data();
}
