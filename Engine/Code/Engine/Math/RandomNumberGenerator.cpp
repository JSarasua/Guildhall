#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/RawNoise.hpp"
#include <stdlib.h>
#include <math.h>


int RandomNumberGenerator::RollRandomIntLessThan( int maxNotInclusive )
{
	unsigned int randomNumber = Get1dNoiseUint(m_position++, m_seed);

	unsigned int randomNumberLessThanValue = randomNumber % maxNotInclusive;
	return randomNumberLessThanValue;
}

int RandomNumberGenerator::RollRandomIntInRange( int minInclusive, int maxInclusive )
{
	int range = (maxInclusive-minInclusive) + 1;
	int randNumLessThan = RollRandomIntLessThan(range);

	return randNumLessThan + minInclusive;
}

float RandomNumberGenerator::RollRandomFloatLessThan( float maxNotInclusive )
{
	float randomNumber = Get1dNoiseZeroToOne(m_position++, m_seed);
	float randomNumberLessThan = randomNumber * maxNotInclusive;

	return randomNumberLessThan;
}

float RandomNumberGenerator::RollRandomFloatInRange( float minInclusive, float maxInclusive )
{
	float randomNumberZeroToOne = RollRandomFloatZeroToOneInclusive();
	float randomNumberInRange = randomNumberZeroToOne*(maxInclusive - minInclusive) + minInclusive;

	return randomNumberInRange;
}

float RandomNumberGenerator::RollRandomFloatZeroToOneInclusive()
{
	constexpr double ONE_OVER_MAX_UINT_PLUS_ONE = (1.0 / ((double)(0xFFFFFFFF) + 1.));
	return (float)(ONE_OVER_MAX_UINT_PLUS_ONE * (double)Get1dNoiseUint( m_position++, m_seed ));
}

float RandomNumberGenerator::RollRandomFloatZeroToAlmostOne()
{
	return Get1dNoiseZeroToOne(m_position++, m_seed);
}

bool RandomNumberGenerator::RollPercentChance( float probabilityOfReturningTrue )
{
	float rollBetween0And1 = RollRandomFloatZeroToOneInclusive();

	if( probabilityOfReturningTrue >= rollBetween0And1 )
	{
		return true;
	}

	return false;
}

Vec2 RandomNumberGenerator::RollRandomDirection2D()
{
	return Vec2::MakeFromPolarDegrees(RollRandomFloatInRange(0.f,360.f));
}

void RandomNumberGenerator::Reset( unsigned int seed /*= 0 */ )
{
	m_seed = seed;
	m_position = 0;
}

