#include "Game/MapTile.hpp"

MapTile::MapTile( const IntVec2& tileCoords, MapRegionType* mapRegionType ) : 
	m_tileCoords( tileCoords ), m_mapRegionType( mapRegionType )
{

}

void MapTile::GetUVs( Vec2& uvAtMins, Vec2& uvAtMaxs, eMapMaterialArea mapMaterialArea ) const
{
	m_mapRegionType->GetUVs( uvAtMins, uvAtMaxs, mapMaterialArea );
}

Texture const& MapTile::GetTexture( eMapMaterialArea mapMaterialArea )
{
	return m_mapRegionType->GetTexture( mapMaterialArea );
}

bool MapTile::IsSolid() const
{
	return m_mapRegionType->IsSolid();
}
