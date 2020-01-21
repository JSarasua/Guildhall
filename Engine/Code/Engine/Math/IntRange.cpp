#include "Engine/Math/IntRange.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

IntRange::IntRange( int minAndMax ) :
	minimum(minAndMax),
	maximum(minAndMax)
{}

IntRange::IntRange( int min, int max ):
	minimum( min ),
	maximum( max )
{}

IntRange::IntRange( const char* asText )
{
	Strings intRangeStrings = SplitStringOnDelimeter( asText, '~' );
	
	GUARANTEE_OR_DIE(intRangeStrings.size() == 2, "IntRange String != 2");

	minimum = atoi( intRangeStrings[0].c_str() );
	maximum = atoi( intRangeStrings[1].c_str() );
}

bool IntRange::IsInRange( int value ) const
{
	if( value >= minimum && value <= maximum )
	{
		return true;
	}
	return false;
}

bool IntRange::DoesOverlap( const IntRange& otherRange ) const
{
	if( (otherRange.minimum > minimum && otherRange.minimum < maximum) ||
		(otherRange.maximum > minimum && otherRange.maximum < maximum) ||
		(otherRange.minimum < minimum && otherRange.maximum > maximum) ||
		(otherRange.minimum > minimum && otherRange.maximum < maximum) )
	{
		return true;
	}
	return false;
}

std::string IntRange::GetAsString() const
{
	std::string intRangeString = Stringf("%i~%i",minimum, maximum);
	return intRangeString;
}

int IntRange::GetRandomInRange( RandomNumberGenerator& rng ) const
{
	int randomNumberInRange = rng.RollRandomIntInRange(minimum,maximum);
	return randomNumberInRange;
}

void IntRange::Set( int newMinimum, int newMaximum )
{
	minimum = newMinimum;
	maximum = newMaximum;
}

bool IntRange::SetFromText( const char* asText )
{
	Strings intRangeStrings = SplitStringOnDelimeter(asText,'~');
	if( intRangeStrings.size() != 2 )
	{
		return false;
	}

	minimum = atoi(intRangeStrings[0].c_str());
	maximum = atoi(intRangeStrings[1].c_str());

	return true;
}

