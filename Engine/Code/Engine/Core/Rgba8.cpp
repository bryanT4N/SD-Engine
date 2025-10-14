#include "Engine/Core/Rgba8.hpp"
//#include "Engine/Math/MathUtils.hpp"
//#include "Engine/Core/EngineCommon.hpp"


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

bool Rgba8::operator==(Rgba8 const& compare) const
{
	return r == compare.r && g == compare.g && b == compare.b && a == compare.a;
}

bool Rgba8::operator!=(Rgba8 const& compare) const
{
	return !(r == compare.r && g == compare.g && b == compare.b && a == compare.a);
}
