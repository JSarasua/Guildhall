#include "Game/MapTile.hpp"
#include "Game/MapRegionType.hpp"

MapTile::MapTile( const IntVec2& tileCoords, MapRegionType* mapRegionType ) : 
	m_tileCoords( tileCoords ), m_mapRegionType( mapRegionType )
{

}

bool MapTile::IsSolid() const
{
	return m_mapRegionType->IsSolid();
}
