#pragma once

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
	~Vec3() {}												// destructor (do nothing)
	Vec3() {}												// default constructor (do nothing)
	Vec3( const Vec3& copyFrom );							// copy constructor (from another Vec3)
	Vec3( const Vec2& copyFrom );
	Vec3( const Vec2& copyFrom, float z );
	explicit Vec3( float initialX, float initialY, float initialZ );		// explicit constructor (from x, y, z)

															// Operators (const)
	bool		operator==( const Vec3& compare ) const;		// Vec3 == Vec3
	bool		operator!=( const Vec3& compare ) const;		// Vec3 != Vec3
	const Vec3	operator+( const Vec3& vecToAdd ) const;		// Vec3 + Vec3
	const Vec3	operator-( const Vec3& vecToSubtract ) const;	// Vec3 - Vec3
	const Vec3	operator-() const;								// -Vec3, i.e. "unary negation"
	const Vec3	operator*( float uniformScale ) const;			// Vec3 * float
	const Vec3	operator*( const Vec3& vecToMultiply ) const;	// Vec3 * Vec3
	const Vec3	operator/( float inverseScale ) const;			// Vec3 / float

																// Operators (self-mutating / non-const)
	void		operator+=( const Vec3& vecToAdd );				// Vec3 += Vec3
	void		operator-=( const Vec3& vecToSubtract );		// Vec3 -= Vec3
	void		operator*=( const float uniformScale );			// Vec3 *= float
	void		operator/=( const float uniformDivisor );		// Vec3 /= float
	void		operator=( const Vec3& copyFrom );				// Vec3 = Vec3

																// Standalone "friend" functions that are conceptually, but not actually, part of Vec3::
	friend const Vec3 operator*( float uniformScale, const Vec3& vecToScale );	// float * Vec3



	Vec3 static const	MakeFromPolarRadians( float orientationRadians, float radius = 1.f );
	Vec3 static const	MakeFromPolarDegrees( float orientationDegrees, float radius = 1.f );
	Vec3 static const	MakeFromSphericalDegrees( float thetaDegrees, float phiDegrees, float radius = 1.f );


	float	GetLength() const;
	float	GetLengthXY() const;
	float	GetLengthSquared() const;
	float	GetLengthXYSquared() const;
	float	GetAngleAboutZRadians() const;
	float	GetAngleAboutZDegrees() const;

	Vec3	GetRotatedAboutZRadians(float deltaRadians) const;
	Vec3	GetRotatedAboutZDegrees(float deltaDegrees) const;
	Vec3	GetClamped( float maxLength ) const;
	Vec3	GetNormalized() const;

	bool	IsAlmostEqual( Vec3 const& other, float epsilon = 0.001f );

	void	SetFromText(const char* text, char const& delimeter = ',' );

	/*
	void	SetAngleRadians( float orientationRadians );
	void	SetAngleDegrees( float orientationDegrees );
	void	SetPolarRadians( float orientationRadians, float newLength );
	void	SetPolarDegrees( float orientationDegrees, float newLength );
	void	Rotate90Degrees();
	void	RotateMinus90Degrees();
	void	RotateRadians( float deltaRadians );
	void	RotateDegrees( float deltaDegrees );
	void	SetLength( float newLength );
	void	ClampLength( float newLength );
	void	Normalize();
	float	NormalizeAndGetPreviousLength();
	*/
	void Normalize();
};
