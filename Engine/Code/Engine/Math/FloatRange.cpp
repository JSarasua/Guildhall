#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

FloatRange::FloatRange( float minAndMax ):
	minimum( minAndMax ),
	maximum( minAndMax )
{

}

FloatRange::FloatRange( float min, float max ):
	minimum( min ),
	maximum( max )
{

}

FloatRange::FloatRange( const char* asText )
{
	Strings floatRangeStrings = SplitStringOnDelimeter( asText, '~' );

	GUARANTEE_OR_DIE( floatRangeStrings.size() == 2, "FloatRange String != 2" );

	minimum = (float)atof( floatRangeStrings[0].c_str() );
	maximum = (float)atof( floatRangeStrings[1].c_str() );
}

bool FloatRange::IsInRange( float value ) const
{
	if( value > minimum && value < maximum )
	{
		return true;
	}
	return false;
}

bool FloatRange::DoesOverlap( const FloatRange& otherRange ) const
{
	if( (otherRange.minimum > minimum&& otherRange.minimum < maximum) ||
		(otherRange.maximum > minimum&& otherRange.maximum < maximum) ||
		(otherRange.minimum < minimum && otherRange.maximum > maximum) ||
		(otherRange.minimum > minimum&& otherRange.maximum < maximum) )
	{
		return true;
	}
	return false;
}

std::string FloatRange::GetAsString() const
{
	std::string floatRangeString = Stringf( "%f~%f", minimum, maximum );
	return floatRangeString;
}

float FloatRange::GetRandomInRange( RandomNumberGenerator& rng ) const
{
	float randomNumberInRange = rng.RollRandomFloatInRange( minimum, maximum );
	return randomNumberInRange;
}

void FloatRange::Set( float newMinimum, float newMaximum )
{
	minimum = newMinimum;
	maximum = newMaximum;
}

bool FloatRange::SetFromText( const char* asText )
{
	Strings floatRangeStrings = SplitStringOnDelimeter( asText, '~' );

	if( floatRangeStrings.size() != 2 )
	{
		return false;
	}

	minimum = (float)atof( floatRangeStrings[0].c_str() );
	maximum = (float)atof( floatRangeStrings[1].c_str() );

	return true;
}

