#pragma once
#include "Game/Map.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Game/Game.hpp"

class TileMap : public Map
{
public:
	TileMap( Game* game );
	~TileMap();

public:
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;

private:

	void RenderTiles();
	void RenderEntities();
	void UpdateTiles( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void SpawnTiles();
	void SpawnEntities();

private:
	//std::vector<Tile> m_tiles;
	std::vector<Vertex_PCUTBN> m_vertsToRender;
	IntVec2 m_mapSize;
	int m_NumOftiles = 0;
};