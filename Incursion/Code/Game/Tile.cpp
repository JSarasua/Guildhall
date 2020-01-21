#include "Tile.hpp"
#include "Engine/Math/AABB2.hpp"
#include "App.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Game/TileDefinition.hpp"

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
	TileDefinition tileDef = TileDefinition::s_definitions[m_tileType];
	Rgba8 tint = tileDef.tint;
	Vec2 uvMins(tileDef.m_uvMins);
	Vec2 uvMaxs(tileDef.m_uvMaxs);

	if( g_theApp->GetDebugGameMode() )
	{
		//Never touched
		if( m_validTile == -1 )
		{
			tint = Rgba8( 0, 0, 0 );
		}
		//Solid
		else if( m_validTile == 0 )
		{
			tint = Rgba8( 128, 128, 255 );
		}
		//Not Solid
		else if( m_validTile == 1 )
		{
			tint = Rgba8( 128, 255, 128 );
		}
	}

		g_theRenderer->AppendVertsFromAABB2(masterVertexList, GetAABB2(), tint, uvMins, uvMaxs);

}

AABB2 Tile::GetAABB2() const
{
	return AABB2(Vec2(m_tileCoords), Vec2(IntVec2(m_tileCoords.x+1, m_tileCoords.y+1)));
}

