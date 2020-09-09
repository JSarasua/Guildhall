#pragma once

struct Vec2;

class RandomNumberGenerator
{
public:
	int		RollRandomIntLessThan( int maxNotInclusive );
	int		RollRandomIntInRange( int minInclusive, int maxInclusive );
	float	RollRandomFloatLessThan( float maxNotInclusive );
	float	RollRandomFloatInRange( float minInclusive, float maxInclusive );
	float	RollRandomFloatZeroToOneInclusive();
	float	RollRandomFloatZeroToAlmostOne();
	bool	RollPercentChance( float probabilityOfReturningTrue );
	Vec2	RollRandomDirection2D();

	void Reset( unsigned int seed = 0 );

private:
	unsigned int	m_seed = 0;
	int				m_position = 0;

};