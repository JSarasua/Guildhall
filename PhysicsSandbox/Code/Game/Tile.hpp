#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>

struct AABB2;
struct Vertex_PCU;

enum TileType
{
	INVALID_TILETYPE = -1,

	STONE,
	GRASS,

	NUM_TILETYPES
};

class Tile
{
public:
	Tile() {}
	~Tile(){}

	Tile(const IntVec2& tileCoords, TileType tileType, int tileIndex);

	void Render() const;
	void AppendVerts( std::vector<Vertex_PCU>& masterVertexList );

	AABB2 GetAABB2() const;

public:
	IntVec2 m_tileCoords;
	TileType m_tileType = INVALID_TILETYPE;
	int m_tileIndex = 0;
};