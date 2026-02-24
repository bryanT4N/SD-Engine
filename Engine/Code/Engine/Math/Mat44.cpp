#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"


Mat44::Mat44()
	: m_values{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f }
{
}


Mat44::Mat44( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D )
	: m_values{
		iBasis2D.x,      iBasis2D.y,      0.f, 0.f,
		jBasis2D.x,      jBasis2D.y,      0.f, 0.f,
		0.f,             0.f,             1.f, 0.f,
		translation2D.x, translation2D.y, 0.f, 1.f }
{
}


Mat44::Mat44( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D )
	: m_values{
		iBasis3D.x,      iBasis3D.y,      iBasis3D.z,      0.f,
		jBasis3D.x,      jBasis3D.y,      jBasis3D.z,      0.f,
		kBasis3D.x,      kBasis3D.y,      kBasis3D.z,      0.f,
		translation3D.x, translation3D.y, translation3D.z, 1.f }
{
}


Mat44::Mat44( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D )
	: m_values{
		iBasis4D.x,      iBasis4D.y,      iBasis4D.z,      iBasis4D.w,
		jBasis4D.x,      jBasis4D.y,      jBasis4D.z,      jBasis4D.w,
		kBasis4D.x,      kBasis4D.y,      kBasis4D.z,      kBasis4D.w,
		translation4D.x, translation4D.y, translation4D.z, translation4D.w }
{
}


Mat44::Mat44( float const* sixteenValuesBasisMajor )
{
	if( sixteenValuesBasisMajor != nullptr ) {
		for( int i = 0; i < 16; ++i ) { m_values[i] = sixteenValuesBasisMajor[i]; }
	}
	else { *this = Mat44(); }
}


Mat44 Mat44::MakeTranslation2D( Vec2 const& translationXY )
{
	float const values[16] = {
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		translationXY.x, translationXY.y, 0.f, 1.f };
	return Mat44( values );
}


Mat44 Mat44::MakeTranslation3D( Vec3 const translationXYZ )
{
	float const values[16] = {
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		translationXYZ.x, translationXYZ.y, translationXYZ.z, 1.f };
	return Mat44( values );
}


Mat44 Mat44::MakeUniformScale2D( float uniformScaleXY )
{
	float const values[16] = {
		uniformScaleXY, 0.f,            0.f, 0.f,
		0.f,            uniformScaleXY, 0.f, 0.f,
		0.f,            0.f,            1.f, 0.f,
		0.f,            0.f,            0.f, 1.f };
	return Mat44( values );
}


Mat44 Mat44::MakeNonUniformScale2D( Vec2 const nonUniformScaleXY )
{
	float const values[16] = {
		nonUniformScaleXY.x, 0.f,                0.f, 0.f,
		0.f,                 nonUniformScaleXY.y,0.f, 0.f,
		0.f,                 0.f,                1.f, 0.f,
		0.f,                 0.f,                0.f, 1.f };
	return Mat44( values );
}


Mat44 Mat44::MakeUniformScale3D( float uniformScaleXYZ )
{
	float const values[16] = {
		uniformScaleXYZ, 0.f,            0.f,            0.f,
		0.f,             uniformScaleXYZ,0.f,            0.f,
		0.f,             0.f,            uniformScaleXYZ,0.f,
		0.f,             0.f,            0.f,            1.f };
	return Mat44( values );
}


Mat44 Mat44::MakeNonUniformScale3D( Vec3 const nonUniformScaleXYZ )
{
	float const values[16] = {
		nonUniformScaleXYZ.x, 0.f,                 0.f,                 0.f,
		0.f,                  nonUniformScaleXYZ.y,0.f,                 0.f,
		0.f,                  0.f,                 nonUniformScaleXYZ.z,0.f,
		0.f,                  0.f,                 0.f,                 1.f };
	return Mat44( values );
}


Mat44 Mat44::MakeZRotationDegrees( float rotationDegreesAboutZ )
{
	float const cosRotationAboutZ = CosDegrees( rotationDegreesAboutZ );
	float const sinRotationAboutZ = SinDegrees( rotationDegreesAboutZ );

	float const values[16] = {
		cosRotationAboutZ,  sinRotationAboutZ, 0.f, 0.f,
		-sinRotationAboutZ, cosRotationAboutZ, 0.f, 0.f,
		0.f,                0.f,               1.f, 0.f,
		0.f,                0.f,               0.f, 1.f };
	return Mat44( values );
}


Mat44 Mat44::MakeYRotationDegrees( float rotationDegreesAboutY )
{
	float const cosRotationAboutY = CosDegrees( rotationDegreesAboutY );
	float const sinRotationAboutY = SinDegrees( rotationDegreesAboutY );

	float const values[16] = {
		cosRotationAboutY, 0.f, -sinRotationAboutY, 0.f,
		0.f,               1.f, 0.f,               0.f,
		sinRotationAboutY, 0.f, cosRotationAboutY, 0.f,
		0.f,               0.f, 0.f,               1.f };
	return Mat44( values );
}


Mat44 Mat44::MakeXRotationDegrees( float rotationDegreesAboutX )
{
	float const cosRotationAboutX = CosDegrees( rotationDegreesAboutX );
	float const sinRotationAboutX = SinDegrees( rotationDegreesAboutX );

	float const values[16] = {
		1.f,               0.f,               0.f,              0.f,
		0.f,               cosRotationAboutX, sinRotationAboutX,0.f,
		0.f,              -sinRotationAboutX, cosRotationAboutX,0.f,
		0.f,               0.f,               0.f,              1.f };
	return Mat44( values );
}

Mat44 Mat44::MakeOrthoProjection( float left, float right, float bottom, float top, float zNear, float zFar )
{
	float const scaleX = 2.f / ( right - left );
	float const scaleY = 2.f / ( top - bottom );
	float const scaleZ = 1.f / ( zFar - zNear );
	float const translateX = -( right + left ) / ( right - left );
	float const translateY = -( top + bottom ) / ( top - bottom );
	float const translateZ = -zNear / ( zFar - zNear );

	float const values[16] = {
		scaleX, 0.f,    0.f,    0.f,
		0.f,    scaleY, 0.f,    0.f,
		0.f,    0.f,    scaleZ, 0.f,
		translateX, translateY, translateZ, 1.f
	};
	return Mat44( values );
}

Mat44 Mat44::MakePerspectiveProjection( float fovYDegrees, float aspect, float zNear, float zFar )
{
	float const halfFovY = 0.5f * fovYDegrees;
	float const sinHalfFovY = SinDegrees( halfFovY );
	float const cosHalfFovY = CosDegrees( halfFovY );
	float const scaleY = cosHalfFovY / sinHalfFovY;
	float const scaleX = scaleY / aspect;
	float const scaleZ = zFar / ( zFar - zNear );
	float const translateZ = -( zNear * zFar ) / ( zFar - zNear );

	float const values[16] = {
		scaleX, 0.f,    0.f,      0.f,
		0.f,    scaleY, 0.f,      0.f,
		0.f,    0.f,    scaleZ,   1.f,
		0.f,    0.f,    translateZ, 0.f
	};
	return Mat44( values );
}


Vec2 Mat44::TransformVectorQuantity2D( Vec2 const vectorQuantityXY ) const
{
	float x = vectorQuantityXY.x * m_values[ Ix ] + vectorQuantityXY.y * m_values[ Jx ];
	float y = vectorQuantityXY.x * m_values[ Iy ] + vectorQuantityXY.y * m_values[ Jy ];
	return Vec2( x, y );
}


Vec3 Mat44::TransformVectorQuantity3D( Vec3 const vectorQuantityXYZ ) const
{
	float x = vectorQuantityXYZ.x * m_values[ Ix ] + vectorQuantityXYZ.y * m_values[ Jx ] + vectorQuantityXYZ.z * m_values[ Kx ];
	float y = vectorQuantityXYZ.x * m_values[ Iy ] + vectorQuantityXYZ.y * m_values[ Jy ] + vectorQuantityXYZ.z * m_values[ Ky ];
	float z = vectorQuantityXYZ.x * m_values[ Iz ] + vectorQuantityXYZ.y * m_values[ Jz ] + vectorQuantityXYZ.z * m_values[ Kz ];
	return Vec3( x, y, z );
}


Vec2 Mat44::TransformPosition2D( Vec2 const positionXY ) const
{
	float x = positionXY.x * m_values[ Ix ] + positionXY.y * m_values[ Jx ] + m_values[ Tx ];
	float y = positionXY.x * m_values[ Iy ] + positionXY.y * m_values[ Jy ] + m_values[ Ty ];
	return Vec2( x, y );
}


Vec3 Mat44::TransformPosition3D( Vec3 const position3D ) const
{
	float x = position3D.x * m_values[ Ix ] + position3D.y * m_values[ Jx ] + position3D.z * m_values[ Kx ] + m_values[ Tx ];
	float y = position3D.x * m_values[ Iy ] + position3D.y * m_values[ Jy ] + position3D.z * m_values[ Ky ] + m_values[ Ty ];
	float z = position3D.x * m_values[ Iz ] + position3D.y * m_values[ Jz ] + position3D.z * m_values[ Kz ] + m_values[ Tz ];
	return Vec3( x, y, z );
}


Vec4 Mat44::TransformHomogeneous3D( Vec4 const homogeneousPoint3D ) const
{
	float x = homogeneousPoint3D.x * m_values[ Ix ] + homogeneousPoint3D.y * m_values[ Jx ] + homogeneousPoint3D.z * m_values[ Kx ] + homogeneousPoint3D.w * m_values[ Tx ];
	float y = homogeneousPoint3D.x * m_values[ Iy ] + homogeneousPoint3D.y * m_values[ Jy ] + homogeneousPoint3D.z * m_values[ Ky ] + homogeneousPoint3D.w * m_values[ Ty ];
	float z = homogeneousPoint3D.x * m_values[ Iz ] + homogeneousPoint3D.y * m_values[ Jz ] + homogeneousPoint3D.z * m_values[ Kz ] + homogeneousPoint3D.w * m_values[ Tz ];
	float w = homogeneousPoint3D.x * m_values[ Iw ] + homogeneousPoint3D.y * m_values[ Jw ] + homogeneousPoint3D.z * m_values[ Kw ] + homogeneousPoint3D.w * m_values[ Tw ];
	return Vec4( x, y, z, w );
}


float* Mat44::GetAsFloatArray() {
	return m_values;
}


float const* Mat44::GetAsFloatArray() const {
	return m_values;
}


Vec2 const Mat44::GetIBasis2D() const {
	return Vec2( m_values[ Ix ], m_values[ Iy ] );
}


Vec2 const Mat44::GetJBasis2D() const {
	return Vec2( m_values[ Jx ], m_values[ Jy ] );
}


Vec2 const Mat44::GetTranslation2D() const {
	return Vec2( m_values[ Tx ], m_values[ Ty ] );
}


Vec3 const Mat44::GetIBasis3D() const {
	return Vec3( m_values[ Ix ], m_values[ Iy ], m_values[ Iz ] );
}


Vec3 const Mat44::GetJBasis3D() const {
	return Vec3( m_values[ Jx ], m_values[ Jy ], m_values[ Jz ] );
}


Vec3 const Mat44::GetKBasis3D() const {
	return Vec3( m_values[ Kx ], m_values[ Ky ], m_values[ Kz ] );
}


Vec3 const Mat44::GetTranslation3D() const {
	return Vec3( m_values[ Tx ], m_values[ Ty ], m_values[ Tz ] );
}


Vec4 const Mat44::GetIBasis4D() const {
	return Vec4( m_values[ Ix ], m_values[ Iy ], m_values[ Iz ], m_values[ Iw ] );
}


Vec4 const Mat44::GetJBasis4D() const {
	return Vec4( m_values[ Jx ], m_values[ Jy ], m_values[ Jz ], m_values[ Jw ] );
}


Vec4 const Mat44::GetKBasis4D() const {
	return Vec4( m_values[ Kx ], m_values[ Ky ], m_values[ Kz ], m_values[ Kw ] );
}


Vec4 const Mat44::GetTranslation4D() const {
	return Vec4( m_values[ Tx ], m_values[ Ty ], m_values[ Tz ], m_values[ Tw ] );
}

Mat44 const Mat44::GetOrthonormalInverse() const
{
	Mat44 inverse;

	inverse.m_values[ Ix ] = m_values[ Ix ];
	inverse.m_values[ Iy ] = m_values[ Jx ];
	inverse.m_values[ Iz ] = m_values[ Kx ];
	inverse.m_values[ Iw ] = 0.f;

	inverse.m_values[ Jx ] = m_values[ Iy ];
	inverse.m_values[ Jy ] = m_values[ Jy ];
	inverse.m_values[ Jz ] = m_values[ Ky ];
	inverse.m_values[ Jw ] = 0.f;

	inverse.m_values[ Kx ] = m_values[ Iz ];
	inverse.m_values[ Ky ] = m_values[ Jz ];
	inverse.m_values[ Kz ] = m_values[ Kz ];
	inverse.m_values[ Kw ] = 0.f;

	float const tx = m_values[ Tx ];
	float const ty = m_values[ Ty ];
	float const tz = m_values[ Tz ];
	inverse.m_values[ Tx ] = -( m_values[ Ix ] * tx + m_values[ Iy ] * ty + m_values[ Iz ] * tz );
	inverse.m_values[ Ty ] = -( m_values[ Jx ] * tx + m_values[ Jy ] * ty + m_values[ Jz ] * tz );
	inverse.m_values[ Tz ] = -( m_values[ Kx ] * tx + m_values[ Ky ] * ty + m_values[ Kz ] * tz );
	inverse.m_values[ Tw ] = 1.f;

	return inverse;
}


void Mat44::SetTranslation2D( Vec2 const translationXY ) 
{
	m_values[ Tx ] = translationXY.x;
	m_values[ Ty ] = translationXY.y;
	m_values[ Tz ] = 0.f;
	m_values[ Tw ] = 1.f;
}


void Mat44::SetTranslation3D( Vec3 const translationXYZ ) 
{
	m_values[ Tx ] = translationXYZ.x;
	m_values[ Ty ] = translationXYZ.y;
	m_values[ Tz ] = translationXYZ.z;
	m_values[ Tw ] = 1.f;
}


void Mat44::SetIJ2D( Vec2 const iBasis2D, Vec2 const jBasis2D )
{
	m_values[ Ix ] = iBasis2D.x;
	m_values[ Iy ] = iBasis2D.y;
	m_values[ Iz ] = 0.f;
	m_values[ Iw ] = 0.f;

	m_values[ Jx ] = jBasis2D.x;
	m_values[ Jy ] = jBasis2D.y;
	m_values[ Jz ] = 0.f;
	m_values[ Jw ] = 0.f;
}


void Mat44::SetIJT2D( Vec2 const iBasis2D, Vec2 const jBasis2D, Vec2 const translationXY )
{
	SetIJ2D( iBasis2D, jBasis2D );
	SetTranslation2D( translationXY );
}


void Mat44::SetIJK3D( Vec3 const iBasis3D, Vec3 const jBasis3D, Vec3 const kBasis3D )
{
	m_values[ Ix ] = iBasis3D.x;
	m_values[ Iy ] = iBasis3D.y;
	m_values[ Iz ] = iBasis3D.z;
	m_values[ Iw ] = 0.f;

	m_values[ Jx ] = jBasis3D.x;
	m_values[ Jy ] = jBasis3D.y;
	m_values[ Jz ] = jBasis3D.z;
	m_values[ Jw ] = 0.f;

	m_values[ Kx ] = kBasis3D.x;
	m_values[ Ky ] = kBasis3D.y;
	m_values[ Kz ] = kBasis3D.z;
	m_values[ Kw ] = 0.f;
}


void Mat44::SetIJKT3D( Vec3 const iBasis3D, Vec3 const jBasis3D, Vec3 const kBasis3D, Vec3 const translationXYZ )
{
	SetIJK3D( iBasis3D, jBasis3D, kBasis3D );
	SetTranslation3D( translationXYZ );
}


void Mat44::SetIJKT4D( Vec4 const iBasis4D, Vec4 const jBasis4D, Vec4 const kBasis4D, Vec4 const translation4D )
{
	m_values[ Ix ] = iBasis4D.x;
	m_values[ Iy ] = iBasis4D.y;
	m_values[ Iz ] = iBasis4D.z;
	m_values[ Iw ] = iBasis4D.w;

	m_values[ Jx ] = jBasis4D.x;
	m_values[ Jy ] = jBasis4D.y;
	m_values[ Jz ] = jBasis4D.z;
	m_values[ Jw ] = jBasis4D.w;

	m_values[ Kx ] = kBasis4D.x;
	m_values[ Ky ] = kBasis4D.y;
	m_values[ Kz ] = kBasis4D.z;
	m_values[ Kw ] = kBasis4D.w;

	m_values[ Tx ] = translation4D.x;
	m_values[ Ty ] = translation4D.y;
	m_values[ Tz ] = translation4D.z;
	m_values[ Tw ] = translation4D.w;
}

void Mat44::Transpose()
{
	float temp = m_values[ Iy ];
	m_values[ Iy ] = m_values[ Jx ];
	m_values[ Jx ] = temp;

	temp = m_values[ Iz ];
	m_values[ Iz ] = m_values[ Kx ];
	m_values[ Kx ] = temp;

	temp = m_values[ Iw ];
	m_values[ Iw ] = m_values[ Tx ];
	m_values[ Tx ] = temp;

	temp = m_values[ Jz ];
	m_values[ Jz ] = m_values[ Ky ];
	m_values[ Ky ] = temp;

	temp = m_values[ Jw ];
	m_values[ Jw ] = m_values[ Ty ];
	m_values[ Ty ] = temp;

	temp = m_values[ Kw ];
	m_values[ Kw ] = m_values[ Tz ];
	m_values[ Tz ] = temp;
}

void Mat44::Orthonormalize_XFwd_YLeft_ZUp()
{
	Vec3 iBasis = GetIBasis3D();
	Vec3 jBasis = GetJBasis3D();
	Vec3 kBasis = GetKBasis3D();
	Vec3 translation = GetTranslation3D();

	iBasis = iBasis.GetNormalized();

	kBasis = kBasis - DotProduct3D( kBasis, iBasis ) * iBasis;
	if( kBasis.GetLengthSquared() == 0.f ) {
		kBasis = CrossProduct3D( iBasis, jBasis );
	}
	kBasis = kBasis.GetNormalized();

	jBasis = jBasis - DotProduct3D( jBasis, iBasis ) * iBasis - DotProduct3D( jBasis, kBasis ) * kBasis;
	if( jBasis.GetLengthSquared() == 0.f ) {
		jBasis = CrossProduct3D( kBasis, iBasis );
	}
	jBasis = jBasis.GetNormalized();

	SetIJK3D( iBasis, jBasis, kBasis );
	m_values[ Tx ] = translation.x;
	m_values[ Ty ] = translation.y;
	m_values[ Tz ] = translation.z;
	m_values[ Tw ] = 1.f;
}


//-----------------------------------------------------------------------------------------------
// Append
//
//     M' = M * A
//
void Mat44::Append( Mat44 const appendThis )
{
	float const old[16] = {
		m_values[ Ix ], m_values[ Iy ], m_values[ Iz ], m_values[ Iw ],
		m_values[ Jx ], m_values[ Jy ], m_values[ Jz ], m_values[ Jw ],
		m_values[ Kx ], m_values[ Ky ], m_values[ Kz ], m_values[ Kw ],
		m_values[ Tx ], m_values[ Ty ], m_values[ Tz ], m_values[ Tw ]
	};

	float const* right = appendThis.m_values;

	// I column
	m_values[ Ix ] = old[ Ix ] * right[ Ix ] + old[ Jx ] * right[ Iy ] + old[ Kx ] * right[ Iz ] + old[ Tx ] * right[ Iw ];
	m_values[ Iy ] = old[ Iy ] * right[ Ix ] + old[ Jy ] * right[ Iy ] + old[ Ky ] * right[ Iz ] + old[ Ty ] * right[ Iw ];
	m_values[ Iz ] = old[ Iz ] * right[ Ix ] + old[ Jz ] * right[ Iy ] + old[ Kz ] * right[ Iz ] + old[ Tz ] * right[ Iw ];
	m_values[ Iw ] = old[ Iw ] * right[ Ix ] + old[ Jw ] * right[ Iy ] + old[ Kw ] * right[ Iz ] + old[ Tw ] * right[ Iw ];

	// J column
	m_values[ Jx ] = old[ Ix ] * right[ Jx ] + old[ Jx ] * right[ Jy ] + old[ Kx ] * right[ Jz ] + old[ Tx ] * right[ Jw ];
	m_values[ Jy ] = old[ Iy ] * right[ Jx ] + old[ Jy ] * right[ Jy ] + old[ Ky ] * right[ Jz ] + old[ Ty ] * right[ Jw ];
	m_values[ Jz ] = old[ Iz ] * right[ Jx ] + old[ Jz ] * right[ Jy ] + old[ Kz ] * right[ Jz ] + old[ Tz ] * right[ Jw ];
	m_values[ Jw ] = old[ Iw ] * right[ Jx ] + old[ Jw ] * right[ Jy ] + old[ Kw ] * right[ Jz ] + old[ Tw ] * right[ Jw ];

	// K column
	m_values[ Kx ] = old[ Ix ] * right[ Kx ] + old[ Jx ] * right[ Ky ] + old[ Kx ] * right[ Kz ] + old[ Tx ] * right[ Kw ];
	m_values[ Ky ] = old[ Iy ] * right[ Kx ] + old[ Jy ] * right[ Ky ] + old[ Ky ] * right[ Kz ] + old[ Ty ] * right[ Kw ];
	m_values[ Kz ] = old[ Iz ] * right[ Kx ] + old[ Jz ] * right[ Ky ] + old[ Kz ] * right[ Kz ] + old[ Tz ] * right[ Kw ];
	m_values[ Kw ] = old[ Iw ] * right[ Kx ] + old[ Jw ] * right[ Ky ] + old[ Kw ] * right[ Kz ] + old[ Tw ] * right[ Kw ];

	// T column
	m_values[ Tx ] = old[ Ix ] * right[ Tx ] + old[ Jx ] * right[ Ty ] + old[ Kx ] * right[ Tz ] + old[ Tx ] * right[ Tw ];
	m_values[ Ty ] = old[ Iy ] * right[ Tx ] + old[ Jy ] * right[ Ty ] + old[ Ky ] * right[ Tz ] + old[ Ty ] * right[ Tw ];
	m_values[ Tz ] = old[ Iz ] * right[ Tx ] + old[ Jz ] * right[ Ty ] + old[ Kz ] * right[ Tz ] + old[ Tz ] * right[ Tw ];
	m_values[ Tw ] = old[ Iw ] * right[ Tx ] + old[ Jw ] * right[ Ty ] + old[ Kw ] * right[ Tz ] + old[ Tw ] * right[ Tw ];
}


void Mat44::AppendZRotation( float degreesRotationAboutZ )
{
	float const cosRotationAboutZ = CosDegrees( degreesRotationAboutZ );
	float const sinRotationAboutZ = SinDegrees( degreesRotationAboutZ );

	float const oldIx = m_values[ Ix ];
	float const oldIy = m_values[ Iy ];
	float const oldIz = m_values[ Iz ];
	float const oldIw = m_values[ Iw ];

	float const oldJx = m_values[ Jx ];
	float const oldJy = m_values[ Jy ];
	float const oldJz = m_values[ Jz ];
	float const oldJw = m_values[ Jw ];

	m_values[ Ix ] = cosRotationAboutZ * oldIx + sinRotationAboutZ * oldJx;
	m_values[ Iy ] = cosRotationAboutZ * oldIy + sinRotationAboutZ * oldJy;
	m_values[ Iz ] = cosRotationAboutZ * oldIz + sinRotationAboutZ * oldJz;
	m_values[ Iw ] = cosRotationAboutZ * oldIw + sinRotationAboutZ * oldJw;

	m_values[ Jx ] = -sinRotationAboutZ * oldIx + cosRotationAboutZ * oldJx;
	m_values[ Jy ] = -sinRotationAboutZ * oldIy + cosRotationAboutZ * oldJy;
	m_values[ Jz ] = -sinRotationAboutZ * oldIz + cosRotationAboutZ * oldJz;
	m_values[ Jw ] = -sinRotationAboutZ * oldIw + cosRotationAboutZ * oldJw;
}


void Mat44::AppendYRotation( float degreesRotationAboutY )
{
	float const cosRotationAboutY = CosDegrees( degreesRotationAboutY );
	float const sinRotationAboutY = SinDegrees( degreesRotationAboutY );

	float const oldIx = m_values[ Ix ];
	float const oldIy = m_values[ Iy ];
	float const oldIz = m_values[ Iz ];
	float const oldIw = m_values[ Iw ];

	float const oldKx = m_values[ Kx ];
	float const oldKy = m_values[ Ky ];
	float const oldKz = m_values[ Kz ];
	float const oldKw = m_values[ Kw ];

	m_values[ Ix ] = cosRotationAboutY * oldIx - sinRotationAboutY * oldKx;
	m_values[ Iy ] = cosRotationAboutY * oldIy - sinRotationAboutY * oldKy;
	m_values[ Iz ] = cosRotationAboutY * oldIz - sinRotationAboutY * oldKz;
	m_values[ Iw ] = cosRotationAboutY * oldIw - sinRotationAboutY * oldKw;

	m_values[ Kx ] = sinRotationAboutY * oldIx + cosRotationAboutY * oldKx;
	m_values[ Ky ] = sinRotationAboutY * oldIy + cosRotationAboutY * oldKy;
	m_values[ Kz ] = sinRotationAboutY * oldIz + cosRotationAboutY * oldKz;
	m_values[ Kw ] = sinRotationAboutY * oldIw + cosRotationAboutY * oldKw;
}


void Mat44::AppendXRotation( float degreesRotationAboutX )
{
	float const cosRotationAboutX = CosDegrees( degreesRotationAboutX );
	float const sinRotationAboutX = SinDegrees( degreesRotationAboutX );

	float const oldJx = m_values[ Jx ];
	float const oldJy = m_values[ Jy ];
	float const oldJz = m_values[ Jz ];
	float const oldJw = m_values[ Jw ];

	float const oldKx = m_values[ Kx ];
	float const oldKy = m_values[ Ky ];
	float const oldKz = m_values[ Kz ];
	float const oldKw = m_values[ Kw ];

	m_values[ Jx ] = cosRotationAboutX * oldJx + sinRotationAboutX * oldKx;
	m_values[ Jy ] = cosRotationAboutX * oldJy + sinRotationAboutX * oldKy;
	m_values[ Jz ] = cosRotationAboutX * oldJz + sinRotationAboutX * oldKz;
	m_values[ Jw ] = cosRotationAboutX * oldJw + sinRotationAboutX * oldKw;

	m_values[ Kx ] = -sinRotationAboutX * oldJx + cosRotationAboutX * oldKx;
	m_values[ Ky ] = -sinRotationAboutX * oldJy + cosRotationAboutX * oldKy;
	m_values[ Kz ] = -sinRotationAboutX * oldJz + cosRotationAboutX * oldKz;
	m_values[ Kw ] = -sinRotationAboutX * oldJw + cosRotationAboutX * oldKw;
}


void Mat44::AppendTranslation2D( Vec2 const translationXY )
{
	float const translateX = translationXY.x;
	float const translateY = translationXY.y;

	float const oldTx = m_values[ Tx ];
	float const oldTy = m_values[ Ty ];
	float const oldTz = m_values[ Tz ];
	float const oldTw = m_values[ Tw ];

	m_values[ Tx ] = oldTx + translateX * m_values[ Ix ] + translateY * m_values[ Jx ];
	m_values[ Ty ] = oldTy + translateX * m_values[ Iy ] + translateY * m_values[ Jy ];
	m_values[ Tz ] = oldTz + translateX * m_values[ Iz ] + translateY * m_values[ Jz ];
	m_values[ Tw ] = oldTw + translateX * m_values[ Iw ] + translateY * m_values[ Jw ];
}


void Mat44::AppendTranslation3D( Vec3 const translationXYZ )
{
	float const translateX = translationXYZ.x;
	float const translateY = translationXYZ.y;
	float const translateZ = translationXYZ.z;

	float const oldTx = m_values[ Tx ];
	float const oldTy = m_values[ Ty ];
	float const oldTz = m_values[ Tz ];
	float const oldTw = m_values[ Tw ];

	m_values[ Tx ] = oldTx + translateX * m_values[ Ix ] + translateY * m_values[ Jx ] + translateZ * m_values[ Kx ];
	m_values[ Ty ] = oldTy + translateX * m_values[ Iy ] + translateY * m_values[ Jy ] + translateZ * m_values[ Ky ];
	m_values[ Tz ] = oldTz + translateX * m_values[ Iz ] + translateY * m_values[ Jz ] + translateZ * m_values[ Kz ];
	m_values[ Tw ] = oldTw + translateX * m_values[ Iw ] + translateY * m_values[ Jw ] + translateZ * m_values[ Kw ];
}


void Mat44::AppendScaleUniform2D( float uniformScaleXY )
{
	m_values[ Ix ] *= uniformScaleXY;
	m_values[ Iy ] *= uniformScaleXY;
	m_values[ Iz ] *= uniformScaleXY;
	m_values[ Iw ] *= uniformScaleXY;

	m_values[ Jx ] *= uniformScaleXY;
	m_values[ Jy ] *= uniformScaleXY;
	m_values[ Jz ] *= uniformScaleXY;
	m_values[ Jw ] *= uniformScaleXY;
}


void Mat44::AppendScaleUniform3D( float uniformScaleXYZ )
{
	m_values[ Ix ] *= uniformScaleXYZ;
	m_values[ Iy ] *= uniformScaleXYZ;
	m_values[ Iz ] *= uniformScaleXYZ;
	m_values[ Iw ] *= uniformScaleXYZ;

	m_values[ Jx ] *= uniformScaleXYZ;
	m_values[ Jy ] *= uniformScaleXYZ;
	m_values[ Jz ] *= uniformScaleXYZ;
	m_values[ Jw ] *= uniformScaleXYZ;

	m_values[ Kx ] *= uniformScaleXYZ;
	m_values[ Ky ] *= uniformScaleXYZ;
	m_values[ Kz ] *= uniformScaleXYZ;
	m_values[ Kw ] *= uniformScaleXYZ;
}


void Mat44::AppendScaleNonUniform2D( Vec2 const nonUniformScaleXY )
{
	float const scaleX = nonUniformScaleXY.x;
	float const scaleY = nonUniformScaleXY.y;

	m_values[ Ix ] *= scaleX;
	m_values[ Iy ] *= scaleX;
	m_values[ Iz ] *= scaleX;
	m_values[ Iw ] *= scaleX;

	m_values[ Jx ] *= scaleY;
	m_values[ Jy ] *= scaleY;
	m_values[ Jz ] *= scaleY;
	m_values[ Jw ] *= scaleY;
}


void Mat44::AppendScaleNonUniform3D( Vec3 const nonUniformScaleXYZ )
{
	float const scaleX = nonUniformScaleXYZ.x;
	float const scaleY = nonUniformScaleXYZ.y;
	float const scaleZ = nonUniformScaleXYZ.z;

	m_values[ Ix ] *= scaleX;
	m_values[ Iy ] *= scaleX;
	m_values[ Iz ] *= scaleX;
	m_values[ Iw ] *= scaleX;

	m_values[ Jx ] *= scaleY;
	m_values[ Jy ] *= scaleY;
	m_values[ Jz ] *= scaleY;
	m_values[ Jw ] *= scaleY;

	m_values[ Kx ] *= scaleZ;
	m_values[ Ky ] *= scaleZ;
	m_values[ Kz ] *= scaleZ;
	m_values[ Kw ] *= scaleZ;
}

