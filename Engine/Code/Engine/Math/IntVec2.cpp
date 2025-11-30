#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>

//-----------------------------------------------------------------------------------------------

IntVec2::IntVec2(const IntVec2& copyFrom)
	: x(copyFrom.x)
	, y(copyFrom.y)
{
}

IntVec2::IntVec2(int initialX, int initialY)
	: x(initialX)
	, y(initialY)
{
}

float IntVec2::GetLength() const
{
	return sqrtf(static_cast<float>(x * x + y * y));
}

int IntVec2::GetTaxicabLength() const
{
	return abs(x) + abs(y);
}

int IntVec2::GetLengthSquared() const
{
	return (x * x + y * y);
}

float IntVec2::GetOrientationRadians() const
{
	return atan2f(static_cast<float>(y), static_cast<float>(x));
}

float IntVec2::GetOrientationDegrees() const
{
	return Atan2Degrees(static_cast<float>(y), static_cast<float>(x));
}

IntVec2 const IntVec2::GetRotatedBy90Degrees() const
{
	return IntVec2(-y, x);
}

IntVec2 const IntVec2::GetRotatedByMinus90Degrees() const
{
	return IntVec2(y, -x);
}

void IntVec2::Rotate90Degrees()
{
	IntVec2 previous = IntVec2(x, y);
	x = -previous.y;
	y = previous.x;
}

void IntVec2::RotateMinus90Degrees()
{
	IntVec2 previous = IntVec2(x, y);
	x = previous.y;
	y = -previous.x;
}

void IntVec2::operator=(IntVec2 const& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}

//-----------------------------------------------------------------------------------------------
void IntVec2::SetFromText(char const* text)
{
	if (text == nullptr)
	{
		x = 0;
		y = 0;
		return;
	}

	Strings parts = SplitStringOnDelimiter(std::string(text), ',');
	ASSERT_OR_DIE(parts.size() == 2, Stringf("IntVec2::SetFromText failed for \"%s\" (expected \"x,y\")", text));

	x = atoi(parts[0].c_str());
	y = atoi(parts[1].c_str());
}