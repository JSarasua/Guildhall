#pragma once
#include <string>

class RandomNumberGenerator;

struct FloatRange
{
public:
	float minimum = 0.f;
	float maximum = 1.f;


public:
	FloatRange() = default;
	explicit FloatRange( float minAndMax );
	explicit FloatRange( float min, float max );
	explicit FloatRange( const char* asText );

 	//Accessors
	bool		IsInRange( float value ) const;
	bool		DoesOverlap( const FloatRange& otherRange ) const;
	std::string	GetAsString() const;
	float		GetRandomInRange( RandomNumberGenerator& rng ) const;

	// Mutators
	void		Set( float newMinimum, float newMaximum );
	bool		SetFromText( const char* asText ); //returns false if malformatted
};