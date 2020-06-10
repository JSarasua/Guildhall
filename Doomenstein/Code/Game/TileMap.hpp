#pragma once
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/MapTile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <vector>

class TileMap : public Map
{
public:
	TileMap( Game* game );
	~TileMap();

public:
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;

private:

	void AppendIndexedVertsTestCube( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList, MapTile const& tile );
	void AddTileIndices( std::vector<uint>& tileIndices, MapTile const& tile );

	bool IsTileSolidAtCoords( IntVec2 const& tileCoords );
	MapTile const& GetMapTileAtTileCoords( IntVec2 const& tileCoords );
	
	void RenderTiles();
	void RenderEntities();
	void UpdateTiles( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void SpawnTiles();
	void SpawnEntities();

private:
	//std::vector<Tile> m_tiles;
	std::vector<Vertex_PCUTBN> m_vertsToRender;
	std::vector<uint> m_tileIndices;
	std::vector<MapTile> m_tiles;
	IntVec2 m_mapSize;
	int m_NumOftiles = 0;
};