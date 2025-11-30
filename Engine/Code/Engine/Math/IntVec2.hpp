#pragma once

//-----------------------------------------------------------------------------------------------
struct IntVec2
{
public: // NOTE: this is one of the few cases where we break both the "m " naming rule AND the avoid-public-members rule
	int x = 0;
	int y = 0;

public:
	// Construction/Destruction
	IntVec2() {}
	IntVec2(const IntVec2& copyFrom);
	~IntVec2() {} 
	explicit IntVec2(int initialX, int initialY);

	// Accessors (const methods) 
	float			GetLength() const;
	int				GetTaxicabLength() const;
	int				GetLengthSquared() const;
	float			GetOrientationRadians() const;
	float			GetOrientationDegrees() const;
	IntVec2 const	GetRotatedBy90Degrees() const;
	IntVec2 const	GetRotatedByMinus90Degrees() const;
		
	// Mutators(non-const methods)
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	
	// Operators (const)
//	bool		operator==(IntVec2 const& compare) const;		// intVec2 == intVec2
//	bool		operator!=(IntVec2 const& compare) const;		// intVec2 != intVec2
//	IntVec2 const	operator+(IntVec2 const& vecToAdd) const;		// intVec2 + intVec2
//	IntVec2 const	operator-(IntVec2 const& vecToSubtract) const;	// intVec2 - intVec2
//	IntVec2 const	operator-() const;								// -intVec2, i.e. "unary negation"
//	IntVec2 const	operator*(float uniformScale) const;			// intVec2 * float
//	IntVec2 const	operator*(IntVec2 const& vecToMultiply) const;	// intVec2 * intVec2
//	IntVec2 const	operator/(float inverseScale) const;			// intVec2 / float

	// Operators (self-mutating / non-const)
//	void		operator+=(IntVec2 const& vecToAdd);				// intVec2 += intVec2
//	void		operator-=(IntVec2 const& vecToSubtract);		// intVec2 -= intVec2
//	void		operator*=(const float uniformScale);			// intVec2 *= float
//	void		operator/=(const float uniformDivisor);		// intVec2 /= float
	void		operator=(IntVec2 const& copyFrom);				// intVec2 = intVec2

	void		SetFromText( char const* text );

	// Standalone "friend" functions that are conceptually, but not actually, part of intVec2::
//	friend IntVec2 const operator*(float uniformScale, IntVec2 const& vecToScale);	// float * intVec2

};