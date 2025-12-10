#pragma once


//-----------------------------------------------------------------------------------------------
struct Vec3;


//-----------------------------------------------------------------------------------------------
struct Vec4
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

public:
	// Construction/Destruction
	~Vec4() {}															// destructor (do nothing)
	Vec4() {}															// default constructor (do nothing)
	Vec4( Vec4 const& copyFrom );										// copy constructor (from another vec4)
	explicit Vec4( float initialX, float initialY, float initialZ, float initialW );	// explicit constructor (from x, y, z, w)
	Vec4( Vec3 const& vecXYZ, float initialW = 0.f );					// constructor from Vec3 plus w

	// Accessors (const methods)
	float		GetLength() const;
	float		GetLengthXYZ() const;
	float		GetLengthSquared() const;
	float		GetLengthXYZSquared() const;

	// Operators (const)
	bool		operator==( Vec4 const& compare ) const;				// vec4 == vec4
	bool		operator!=( Vec4 const& compare ) const;				// vec4 != vec4
	Vec4 const	operator+( Vec4 const& vecToAdd ) const;				// vec4 + vec4
	Vec4 const	operator-( Vec4 const& vecToSubtract ) const;			// vec4 - vec4
	Vec4 const	operator-() const;										// -vec4, i.e. "unary negation"
	Vec4 const	operator*( float uniformScale ) const;					// vec4 * float
	Vec4 const	operator*( Vec4 const& vecToMultiply ) const;			// vec4 * vec4
	Vec4 const	operator/( float inverseScale ) const;					// vec4 / float

	// Operators (self-mutating / non-const)
	void		operator+=( Vec4 const& vecToAdd );						// vec4 += vec4
	void		operator-=( Vec4 const& vecToSubtract );				// vec4 -= vec4
	void		operator*=( float uniformScale );						// vec4 *= float
	void		operator/=( float uniformDivisor );						// vec4 /= float
	void		operator=( Vec4 const& copyFrom );						// vec4 = vec4

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec4::
	friend Vec4 const operator*( float uniformScale, Vec4 const& vecToScale );	// float * vec4
};



