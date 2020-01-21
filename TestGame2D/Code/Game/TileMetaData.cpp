#include "Game/TileMetaData.hpp"


bool TileMetaData::IsHeatInRange( FloatRange heatRange )
{
	return heatRange.IsInRange( m_heat );
}

