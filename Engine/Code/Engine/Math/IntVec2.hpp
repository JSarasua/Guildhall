#pragma once


struct Vec2;

/*
struct IntVec2 (int x, int y)
Similar to Vec2, but add GetTaxicabLength and remove most arbitrary free-rotation and relengthening methods
*/

struct IntVec2
{
public:
	int x = 0;
	int y = 0;

public:
	~IntVec2() {}
	IntVec2() {}
	IntVec2( const IntVec2& copyFrom );
	IntVec2( const Vec2& copyFrom );
	explicit IntVec2( int initialX, int initialY );

	//Operators (const)
	bool			operator==( const IntVec2& compare ) const;
	bool			operator!=( const IntVec2& compare ) const;
	const IntVec2	operator+( const IntVec2& vecToAdd ) const;
	const IntVec2	operator-( const IntVec2& vecToSubtract ) const;
	const IntVec2	operator-() const;
	const IntVec2	operator*( int uniformScale ) const;
	const IntVec2	operator*( const IntVec2& vecToMultiply ) const;
	//Operators (self-mutating / non-const)
	
	void		operator+=( const IntVec2& vecToAdd );
	void		operator-=( const IntVec2& vecToSubtract );
	void		operator*=( const int uniformScale );
	void		operator=( const IntVec2& copyFrom );
	
	//Standalone
	int GetTaxicabLength() const;
	float GetLength() const;
	float GetLengthSquared() const;


	IntVec2 GetRotated90Degrees() const;
	IntVec2 GetRotatedMinus90Degrees() const;
	float GetOrientationDegrees() const;
	float GetOrientationRadians() const;

	void Rotate90Degrees();
	void RotateMinus90Degrees();

	void SetFromText(const char* text);
};