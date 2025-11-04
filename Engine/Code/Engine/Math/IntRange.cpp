#include "Engine/Math/IntRange.hpp"

//-----------------------------------------------------------------------------------------------
const IntRange IntRange::ZERO(0, 0);
const IntRange IntRange::ONE(1, 1);
const IntRange IntRange::ZERO_TO_ONE(0, 1);

//-----------------------------------------------------------------------------------------------
IntRange::IntRange()
	:m_min(0), m_max(0)
{

}

//-----------------------------------------------------------------------------------------------
IntRange::IntRange(const int& min, const int& max)
	:m_min(min), m_max(max)
{

}

//-----------------------------------------------------------------------------------------------
bool IntRange::IsOnRange(int value) {
	return !((value > m_max) || (value < m_min));
}

//-----------------------------------------------------------------------------------------------
bool IntRange::IsOverlappingWith(IntRange other) {
	return !(other.m_max < m_min || other.m_min > m_max);
}

//-----------------------------------------------------------------------------------------------
void IntRange::operator=(IntRange const& copyFrom)
{
	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;
}

//-----------------------------------------------------------------------------------------------
bool IntRange::operator==(IntRange const& compare) const {
	return m_min == compare.m_min && m_max == compare.m_max;
}

//-----------------------------------------------------------------------------------------------
bool IntRange::operator!=(IntRange const& compare) const {
	return !(m_min == compare.m_min && m_max == compare.m_max);
}