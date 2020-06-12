#pragma once
#include "Game/MapRegionType.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>

struct AABB2;
struct Vertex_PCU;



class MapTile
{
public:
	MapTile() {}
	~MapTile() {}

	MapTile( const IntVec2& tileCoords, MapRegionType* mapRegionType );

	void GetUVs( Vec2& uvAtMins, Vec2& uvAtMaxs, eMapMaterialArea mapMaterialArea ) const;
	Texture const& GetTexture( eMapMaterialArea mapMaterialArea );

	bool IsSolid() const;


/*	AABB2 GetAABB2() const;*/

public:
	IntVec2 m_tileCoords;
	bool m_tempIsSolid = false;
	MapRegionType* m_mapRegionType = nullptr;
};