#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <cstdlib>

//-----------------------------------------------------------------------------------------------
const Rgba8 Rgba8::WHITE(255, 255, 255, 255);
const Rgba8 Rgba8::GRAY(128, 128, 128, 255);
const Rgba8 Rgba8::RED(255, 0, 0, 255);
const Rgba8 Rgba8::GREEN(0, 255, 0, 255);
const Rgba8 Rgba8::BLUE(0, 0, 255, 255);
const Rgba8 Rgba8::NAVI(0, 0, 128, 255);
const Rgba8 Rgba8::YELLOW(255, 255, 0, 255);
const Rgba8 Rgba8::CYAN(0, 255, 255, 255);
const Rgba8 Rgba8::PURPLE(255, 0, 255, 255);
const Rgba8 Rgba8::ORANGE(255, 128, 0, 255);
const Rgba8 Rgba8::BLACK(0, 0, 0, 255);
const Rgba8 Rgba8::TEAL(0, 128, 128, 255);


//-----------------------------------------------------------------------------------------------
void Rgba8::ScaleAlpha(float alphaMultiplier)
{
	a = static_cast<unsigned char>(alphaMultiplier * static_cast<float>(a));
}

//-----------------------------------------------------------------------------------------------
Rgba8::Rgba8(Rgba8 const& copyFrom)
{
	r = copyFrom.r;
	g = copyFrom.g;
	b = copyFrom.b;
	a = copyFrom.a;
}

Rgba8::Rgba8(unsigned char initialR, unsigned char initialG, unsigned char initialB, unsigned char initialA)
	: r(initialR)
	, g(initialG)
	, b(initialB)
	, a(initialA)
{
}

//-----------------------------------------------------------------------------------------------
void Rgba8::SetFromText(char const* text)
{
	if (text == nullptr)
	{
		r = 255;
		g = 255;
		b = 255;
		a = 255;
		return;
	}

	Strings parts = SplitStringOnDelimiter(std::string(text), ',');
	ASSERT_OR_DIE(parts.size() == 3 || parts.size() == 4,
		Stringf("Rgba8::SetFromText failed for \"%s\" (expected \"r,g,b\" or \"r,g,b,a\")", text));

	int rInt = atoi(parts[0].c_str());
	int gInt = atoi(parts[1].c_str());
	int bInt = atoi(parts[2].c_str());
	int aInt = (parts.size() == 4) ? atoi(parts[3].c_str()) : 255;

	r = static_cast<unsigned char>(rInt);
	g = static_cast<unsigned char>(gInt);
	b = static_cast<unsigned char>(bInt);
	a = static_cast<unsigned char>(aInt);
}

void Rgba8::GetAsFloats(float* colorAsFloats) const
{
	constexpr float inv255 = 1.0f / 255.0f;

	colorAsFloats[0] = r * inv255;
	colorAsFloats[1] = g * inv255;
	colorAsFloats[2] = b * inv255;
	colorAsFloats[3] = a * inv255;
}


bool Rgba8::operator==(Rgba8 const& compare) const
{
	return r == compare.r && g == compare.g && b == compare.b && a == compare.a;
}

bool Rgba8::operator!=(Rgba8 const& compare) const
{
	return !(r == compare.r && g == compare.g && b == compare.b && a == compare.a);
}

//-----------------------------------------------------------------------------------------------
Rgba8 Interpolate( Rgba8 start, Rgba8 end, float fractionOfEnd )
{
	float r = Interpolate( NormalizeByte( start.r ), NormalizeByte( end.r ), fractionOfEnd );
	float g = Interpolate( NormalizeByte( start.g ), NormalizeByte( end.g ), fractionOfEnd );
	float b = Interpolate( NormalizeByte( start.b ), NormalizeByte( end.b ), fractionOfEnd );
	float a = Interpolate( NormalizeByte( start.a ), NormalizeByte( end.a ), fractionOfEnd );

	return Rgba8(
		DenormalizeByte( r ),
		DenormalizeByte( g ),
		DenormalizeByte( b ),
		DenormalizeByte( a ) );
}
