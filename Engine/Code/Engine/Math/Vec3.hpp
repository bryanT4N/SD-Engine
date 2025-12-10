#pragma once


//-----------------------------------------------------------------------------------------------
struct Vec2;


//-----------------------------------------------------------------------------------------------
struct Vec3
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

public:
	// Construction/Destruction
	~Vec3() {}														// destructor (do nothing)
	Vec3() {}														// default constructor (do nothing)
	Vec3(Vec3 const& copyFrom);										// copy constructor (from another vec3)
	explicit Vec3(float initialX, float intialY, float intialZ);	// explicit constructor (from x, y)
	Vec3(Vec2 const& vecXY);										// constructor from another vec2

	// Accessors (const methods)
	float		GetLength() const; 
	float		GetLengthXY() const; 
	float		GetLengthSquared() const;
	float		GetLengthXYSquared() const;
	float		GetOrientationAboutZDegrees() const; 
	float		GetOrientationAboutZRadians() const; 
	Vec3 const	GetRotatedAboutZDegrees(float rotationDegreesAboutZ) const; 
	Vec3 const	GetRotatedAboutZRadians(float rotationRadiansAboutZ) const; 
	Vec3 const	GetClampedToMaxLength(float maxLength) const; 
	Vec3 const	GetNormalized() const;

	// Operators (const)
	bool		operator==(Vec3 const& compare) const;		// vec3 == vec3
	bool		operator!=(Vec3 const& compare) const;		// vec3 != vec3
	Vec3 const	operator+(Vec3 const& vecToAdd) const;		// vec3 + vec3
	Vec3 const	operator-(Vec3 const& vecToSubtract) const;	// vec3 - vec3
	Vec3 const	operator-() const;							// -vec3, i.e. "unary negation"
	Vec3 const	operator*(float uniformScale) const;		// vec3 * float
	Vec3 const	operator*(Vec3 const& vecToMultiply) const;	// vec3 * vec3
	Vec3 const	operator/(float inverseScale) const;		// vec3 / float

	// Operators (self-mutating / non-const)
	void		operator+=(Vec3 const& vecToAdd);			// vec3 += vec3
	void		operator-=(Vec3 const& vecToSubtract);		// vec3 -= vec3
	void		operator*=(const float uniformScale);		// vec3 *= float
	void		operator/=(const float uniformDivisor);		// vec3 /= float
	void		operator=(Vec3 const& copyFrom);			// vec3 = vec3

	// Standalone "friend" functions that are conceptually, but not actually, part of vec3::
	friend Vec3 const operator*(float uniformScale, Vec3 const& vecToScale);	// float * vec3
};


