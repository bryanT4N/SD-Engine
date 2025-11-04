#include "Engine/Math/FloatRange.hpp"

//-----------------------------------------------------------------------------------------------
const FloatRange FloatRange::ZERO(0.f, 0.f);
const FloatRange FloatRange::ONE(1.f, 1.f);
const FloatRange FloatRange::ZERO_TO_ONE(0.f, 1.f);

//-----------------------------------------------------------------------------------------------
FloatRange::FloatRange()
	:m_min(0.f), m_max(0.f)
{

}

//-----------------------------------------------------------------------------------------------
FloatRange::FloatRange(const float& min, const float& max)
	:m_min(min), m_max(max)
{

}

//-----------------------------------------------------------------------------------------------
bool FloatRange::IsOnRange(float value) {
	return !((value > m_max) || (value < m_min));
}

//-----------------------------------------------------------------------------------------------
bool FloatRange::IsOverlappingWith(FloatRange other) {
	return !(other.m_max < m_min || other.m_min > m_max);
}

//-----------------------------------------------------------------------------------------------
void FloatRange::operator=(FloatRange const& copyFrom)
{
	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;
}

//-----------------------------------------------------------------------------------------------
bool FloatRange::operator==(FloatRange const& compare) const {
	return m_min == compare.m_min && m_max == compare.m_max;
}

//-----------------------------------------------------------------------------------------------
bool FloatRange::operator!=(FloatRange const& compare) const {
	return !(m_min == compare.m_min && m_max == compare.m_max);
}