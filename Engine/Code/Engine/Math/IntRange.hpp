#pragma once

class IntRange {
public:
	int		m_min;
	int		m_max;

public:
	IntRange();
	IntRange(const int& min, const int& max);
	~IntRange() = default;

	// Operators
	void		operator=(IntRange const& copyFrom);
	bool		operator==(IntRange const& compare) const;
	bool		operator!=(IntRange const& compare) const;

	bool		IsOnRange(int value);
	bool		IsOverlappingWith(IntRange other);

	// Static const
	static const IntRange ZERO;        // [0, 0]
	static const IntRange ONE;         // [1, 1]
	static const IntRange ZERO_TO_ONE; // [0, 1]
};