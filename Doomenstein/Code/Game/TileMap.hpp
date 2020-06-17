#pragma once
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/MapTile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <vector>
#include <map>

class TileMap : public Map
{
public:
	TileMap( XmlElement const& element, Game* game );
	~TileMap();

public:
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void SetPlayerToStart() override;
	virtual Vec2 const& GetPlayerStartPosition() override;
	virtual float GetPlayerStartYaw() override;

private:

	void AppendIndexedVertsTestCube( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList, MapTile const& tile );
	void AddTileIndices( std::vector<uint>& tileIndices, MapTile const& tile );

	bool IsTileSolidAtCoords( IntVec2 const& tileCoords );
	MapTile const& GetMapTileAtTileCoords( IntVec2 const& tileCoords );
	
	void ParseLegend( XmlElement const& legendElement );
	void SpawnTiles( XmlElement const& mapRowsElement );
	void SpawnMapRow( std::string const& mapRow, uint heightIndex );
	void ParseEntities( XmlElement const& entitiesElement );

	void RenderTiles();
	void RenderEntities();
	void UpdateTiles( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void SpawnEntities();

private:
	//std::vector<Tile> m_tiles;
	std::vector<Vertex_PCUTBN> m_vertsToRender;
	std::vector<uint> m_tileIndices;
	std::vector<MapTile> m_tiles;
	IntVec2 m_mapSize;
	int m_NumOftiles = 0;

	Vec2 m_playerStartPosition;
	float m_playerStartYaw = 0.f;



	std::map< unsigned char, std::string > m_legend;
};