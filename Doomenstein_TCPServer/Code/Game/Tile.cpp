#include "Tile.hpp"
#include "Engine/Math/AABB2.hpp"
#include "App.hpp"
#include "Engine/Math/AABB2.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;


Tile::Tile( const IntVec2& tileCoords, TileType tileType, int tileIndex ) :
	m_tileCoords(tileCoords),
	m_tileType(tileType),
	m_tileIndex(tileIndex)
{}

void Tile::Render() const
{

	AABB2 aabb = AABB2(Vec2((float)m_tileCoords.x, (float)m_tileCoords.y), Vec2((float)(m_tileCoords.x + 1), (float)(m_tileCoords.y + 1)));
	if( m_tileType == STONE )
	{
		g_theRenderer->DrawAABB2Filled( aabb, Rgba8( 127, 127, 127 ) );
	}
	else 
	{
		g_theRenderer->DrawAABB2Filled( aabb, Rgba8( 0, 100, 0 ) );
	}
}

void Tile::AppendVerts( std::vector<Vertex_PCU>& masterVertexList )
{


	if( m_tileType == STONE )
	{
		g_theRenderer->AppendVertsFromAABB2(masterVertexList, GetAABB2(), Rgba8( 127, 127, 127 ), Vec2(0.f,0.f), Vec2(1.f,1.f));
	}
	else
	{
		g_theRenderer->AppendVertsFromAABB2(masterVertexList, GetAABB2(), Rgba8( 0, 100, 0 ), Vec2(0.f,0.f), Vec2(1.f,1.f));
	}
}

AABB2 Tile::GetAABB2() const
{
	return AABB2(Vec2(m_tileCoords), Vec2(IntVec2(m_tileCoords.x+1, m_tileCoords.y+1)));
}

