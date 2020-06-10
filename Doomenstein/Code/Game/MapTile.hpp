#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>

struct AABB2;
struct Vertex_PCU;

class MapTile
{
public:
	MapTile() {}
	~MapTile() {}

	MapTile( const IntVec2& tileCoords );


/*	AABB2 GetAABB2() const;*/

public:
	IntVec2 m_tileCoords;
	bool m_tempIsSolid = false;
};