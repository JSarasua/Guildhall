#include "Tile.hpp"
#include "Engine/Math/AABB2.hpp"
#include "App.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;



Tile::Tile( const IntVec2& tileCoords, TileDefinition* tileDef ) :
	m_tileCoords(tileCoords),
	m_tileDef(tileDef)
{}

void Tile::Render() const
{

}

void Tile::AppendVerts( std::vector<Vertex_PCU>& masterVertexList )
{
	AABB2 tileUVs = m_tileDef->m_spriteUVs;
	//Fixes floating point error with UVs
	tileUVs.SetDimensions( tileUVs.GetDimensions() * 0.99f );
	Rgba8 tileTint = m_tileDef->m_tint;

	g_theRenderer->AppendVertsFromAABB2(masterVertexList, GetAABB2(), tileTint, tileUVs.mins, tileUVs.maxs);
}

void Tile::SetTileType( TileDefinition* newTileDef )
{
	m_tileDef = newTileDef;
}

AABB2 Tile::GetAABB2() const
{
	//Use floatingpointprotection  if lines occur
	//Vec2 floatingPointProtection = Vec2( 0.001f, 0.001f );
	return AABB2(Vec2(m_tileCoords), Vec2(IntVec2(m_tileCoords.x+1, m_tileCoords.y+1)));
}

