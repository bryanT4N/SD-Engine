#pragma once

class FloatRange {
public:
	float		m_min;
	float		m_max;

public:
	FloatRange();
	FloatRange(const float& min, const float& max);
	~FloatRange() = default;

	// Operators
	void		operator=(FloatRange const& copyFrom);
	bool		operator==(FloatRange const& compare) const;
	bool		operator!=(FloatRange const& compare) const;

	bool		IsOnRange(float value);
	bool		IsOverlappingWith(FloatRange other);

	// Static const
	static const FloatRange ZERO;        // [0, 0]
	static const FloatRange ONE;         // [1, 1]
	static const FloatRange ZERO_TO_ONE; // [0, 1]
};