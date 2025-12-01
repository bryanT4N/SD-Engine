#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <climits>
#include <cmath>

//-----------------------------------------------------------------------------------------------
Vec2 AnalogJoystick::GetPosition() const
{
	return m_correctedPosition;
}

float AnalogJoystick::GetMagnitude() const
{
	return m_correctedPosition.GetLength();
}

float AnalogJoystick::GetOrientationDegrees() const
{
	return m_correctedPosition.GetOrientationDegrees();
}

Vec2 AnalogJoystick::GetRawUncorrectedPosition() const
{
	return m_rawPosition;
}

float AnalogJoystick::GetInnerDeadZoneFraction() const
{
	return m_innerDeadZoneFraction;
}

float AnalogJoystick::GetOuterDeadZoneFraction() const
{
	return m_outerDeadZoneFraction;
}

void AnalogJoystick::Reset()
{
	UpdatePosition(0.f, 0.f);
}

void AnalogJoystick::SetDeadzoneThresholds(float normalizedInnerDeadzoneThreshold, float normalizedouterDeadzoneThreshold)
{
	m_innerDeadZoneFraction = normalizedInnerDeadzoneThreshold;
	m_outerDeadZoneFraction = normalizedouterDeadzoneThreshold;
}

void AnalogJoystick::UpdatePosition(float rawNormalizedX, float rawNormalizedY)
{
	m_rawPosition = Vec2(rawNormalizedX, rawNormalizedY);

	float degrees	= m_rawPosition.GetOrientationDegrees();
	float length	= m_rawPosition.GetLength();
	length = RangeMapClamped(length, m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.f, 1.f);

	m_correctedPosition = Vec2::MakeFromPolarDegrees(degrees, length);
}

