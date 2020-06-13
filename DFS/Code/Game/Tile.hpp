#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>

struct AABB2;
struct Vertex_PCU;
class TileDefinition;


class Tile
{
public:
	Tile() = delete;
	~Tile(){}

	Tile(const IntVec2& tileCoords, TileDefinition* tileDef);

	void Render() const;
	void AppendVerts( std::vector<Vertex_PCU>& masterVertexList );
	void SetTileType(TileDefinition* newTileDef);
	AABB2 GetAABB2() const;

public:
	IntVec2 m_tileCoords;
	TileDefinition* m_tileDef = nullptr;
};