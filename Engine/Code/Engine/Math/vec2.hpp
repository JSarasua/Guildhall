#pragma once

struct IntVec2;
struct Vec3;
//-----------------------------------------------------------------------------------------------
struct Vec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;

public:
	// Construction/Destruction
	~Vec2() {}												// destructor (do nothing)
	Vec2() {}												// default constructor (do nothing)
	Vec2( Vec2 const& copyFrom );							// copy constructor (from another vec2)
	Vec2( IntVec2 const& copyFrom );
	Vec2( Vec3 const& copyFrom );
	explicit Vec2( float initialX, float initialY );		// explicit constructor (from x, y)

	// Operators (const)
	bool		operator==( Vec2 const& compare ) const;		// vec2 == vec2
	bool		operator!=( Vec2 const& compare ) const;		// vec2 != vec2
	const Vec2	operator+( Vec2 const& vecToAdd ) const;		// vec2 + vec2
	const Vec2	operator-( Vec2 const& vecToSubtract ) const;	// vec2 - vec2
	const Vec2	operator-() const;								// -vec2, i.e. "unary negation"
	const Vec2	operator*( float uniformScale ) const;			// vec2 * float
	const Vec2	operator*( Vec2 const& vecToMultiply ) const;	// vec2 * vec2
	const Vec2	operator/( float inverseScale ) const;			// vec2 / float

	// Operators (self-mutating / non-const)
	void		operator+=( Vec2 const& vecToAdd );				// vec2 += vec2
	void		operator-=( Vec2 const& vecToSubtract );		// vec2 -= vec2
	void		operator*=( float const uniformScale );			// vec2 *= float
	void		operator/=( float const uniformDivisor );		// vec2 /= float
	void		operator=( Vec2 const& copyFrom );				// vec2 = vec2

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec2::
	friend const Vec2 operator*( float uniformScale, Vec2 const& vecToScale );	// float * vec2


	
	Vec2 static const	MakeFromPolarRadians(float orientationRadians, float radius = 1.f);
	Vec2 static const	MakeFromPolarDegrees(float orientationDegrees, float radius = 1.f);


	float	GetLength() const;
	float	GetLengthSquared() const;
	float	GetAngleRadians() const;
	float	GetAngleDegrees() const;
	Vec2	GetRotated90Degrees() const;
	Vec2	GetRotatedMinus90Degrees() const;
	Vec2	GetRotatedRadians( float deltaRadians ) const;
	Vec2	GetRotatedDegrees( float deltaDegrees ) const;
	Vec2	GetClamped( float maxLength ) const;
	Vec2	GetNormalized() const;
	Vec2	GetReflected( Vec2 const& normalVector ) const;


	void	SetAngleRadians(float orientationRadians);
	void	SetAngleDegrees(float orientationDegrees);
	void	SetPolarRadians(float orientationRadians, float newLength);
	void	SetPolarDegrees(float orientationDegrees, float newLength);
	void	Rotate90Degrees();
	void	RotateMinus90Degrees();
	void	RotateRadians(float deltaRadians);
	void	RotateDegrees(float deltaDegrees);
	void	SetLength(float newLength);
	void	ClampLength(float newLength);
	void	Normalize();
	float	NormalizeAndGetPreviousLength();
	void	Reflect( Vec2 const& normalVector);
	void	SetFromText( char const* text); //Parse "6,4" or " -.3 , 0.05 " to (x,y)

	bool	IsAlmostEqual( Vec2 const& other, float epsilon = 0.001f );

};


