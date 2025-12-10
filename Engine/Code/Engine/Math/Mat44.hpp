#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"


// 4x4 Homogeneous 3D/3D transformation matrix, stored basis-major in memory (Ix,Iy,Iz,Iw,Jx,Jy,Jz,Jw...).
//
// Note: we specifically do NOT provide an operator* overload, since doing so would require a
// decision to commit the Mat44 to only work consistently with EITHER column-major or row-major style
// notation.  They are actually two different ways of writing operator*, and in order to implement
// an operator*, we are forced to make a notational commitment.  This is certainly ambiguous to the
// student and at the very least, potentially confusing.  Instead, we prefer to use method names,
// such as Append(), which are more neutral (e.g. multiply a new matrix “on the right in column-
// notation” or “on the left in row-notation”.
struct Mat44
{
	enum {
		Ix, Iy, Iz, Iw,
		Jx, Jy, Jz, Jw,
		Kx, Ky, Kz, Kw,
		Tx, Ty, Tz, Tw
	};

	float m_values[16];		// stored in "basis major" order (Ix,Iy,Iz,Iw,Jx,Jy,Jz,Jw,...) - translation in [12,13,14]

	Mat44();				// Default constructor: Identity matrix!
	explicit Mat44( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D );
	explicit Mat44( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D );
	explicit Mat44( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D );
	explicit Mat44( float const* sixteenValuesBasisMajor );

//-----------------------------------------------------------------------------------------------
	static Mat44 MakeTranslation2D( Vec2 const& translationXY );
	static Mat44 MakeTranslation3D( Vec3 const translationXYZ );
	static Mat44 MakeUniformScale2D( float uniformScaleXY );
	static Mat44 MakeNonUniformScale2D( Vec2 const nonUniformScaleXY );
	static Mat44 MakeUniformScale3D( float uniformScaleXYZ );
	static Mat44 MakeNonUniformScale3D( Vec3 const nonUniformScaleXYZ );
	static Mat44 MakeZRotationDegrees( float rotationDegreesAboutZ );
	static Mat44 MakeYRotationDegrees( float rotationDegreesAboutY );
	static Mat44 MakeXRotationDegrees( float rotationDegreesAboutX );

//-----------------------------------------------------------------------------------------------
	Vec2 TransformVectorQuantity2D( Vec2 const vectorQuantityXY ) const;	// assumes z=0, w=0
	Vec3 TransformVectorQuantity3D( Vec3 const vectorQuantityXYZ ) const;	// assumes w=0
	Vec2 TransformPosition2D( Vec2 const positionXY ) const;				// assumes z=0, w=1
	Vec3 TransformPosition3D( Vec3 const position3D ) const;				// assumes w=1
	Vec4 TransformHomogeneous3D( Vec4 const homogeneousPoint3D ) const;		// w is provided

//-----------------------------------------------------------------------------------------------
	float*       GetAsFloatArray();
	float const* GetAsFloatArray() const;

	Vec2 const GetIBasis2D() const;
	Vec2 const GetJBasis2D() const;
	Vec2 const GetTranslation2D() const;

	Vec3 const GetIBasis3D() const;
	Vec3 const GetJBasis3D() const;
	Vec3 const GetKBasis3D() const;
	Vec3 const GetTranslation3D() const;

	Vec4 const GetIBasis4D() const;
	Vec4 const GetJBasis4D() const;
	Vec4 const GetKBasis4D() const;
	Vec4 const GetTranslation4D() const;

//-----------------------------------------------------------------------------------------------
	void SetTranslation2D( Vec2 const translationXY );										// Sets translationZ = 0, translationW = 1
	void SetTranslation3D( Vec3 const translationXYZ );										// Sets translationW = 1
	void SetIJ2D( Vec2 const iBasis2D, Vec2 const jBasis2D );								// Sets z=w=0 for I & J; does not modify k or t
	void SetIJT2D( Vec2 const iBasis2D, Vec2 const jBasis2D, Vec2 const translationXY );	// Sets z=w=0 for I & J; w=1 for t; does not modify k
	void SetIJK3D( Vec3 const iBasis3D, Vec3 const jBasis3D, Vec3 const kBasis3D );			// Sets w=0 for i/j/k; does not modify t
	void SetIJKT3D( Vec3 const iBasis3D, Vec3 const jBasis3D, Vec3 const kBasis3D, Vec3 const translationXYZ );	// Sets w=0 for i/j/k, w=1 for t
	void SetIJKT4D( Vec4 const iBasis4D, Vec4 const jBasis4D, Vec4 const kBasis4D, Vec4 const translation4D );	// All 16 values provided

//-----------------------------------------------------------------------------------------------
	void Append( Mat44 const appendThis );													// multiply on right in column notation / on left in row notation

	void AppendZRotation( float degreesRotationAboutZ );									// same as appending (= in column notation) a Z-rotation matrix
	void AppendYRotation( float degreesRotationAboutY );									// same as appending (= in column notation) a Y-rotation matrix
	void AppendXRotation( float degreesRotationAboutX );									// same as appending (= in column notation) a X-rotation matrix
	void AppendTranslation2D( Vec2 const translationXY );									// same as appending (= in column notation) a 2D-translation matrix
	void AppendTranslation3D( Vec3 const translationXYZ );									// same as appending (= in column notation) a 3D-translation matrix
	void AppendScaleUniform2D( float uniformScaleXY );										// k and t bases should remain unaffected
	void AppendScaleUniform3D( float uniformScaleXYZ );										// t basis should remain unaffected
	void AppendScaleNonUniform2D( Vec2 const nonUniformScaleXY );							// k and t bases should remain unaffected
	void AppendScaleNonUniform3D( Vec3 const nonUniformScaleXYZ );							// translation should remain unaffected
};


