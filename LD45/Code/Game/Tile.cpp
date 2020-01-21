#include "Tile.hpp"
#include "App.hpp"
#include "Game/Game.hpp"


extern App* g_theApp;
extern RenderContext* g_theRenderer;

Rgba8 g_Left(0,127,127);
Rgba8 g_Right(255,127,127);
Rgba8 g_Up(127,255,127);
Rgba8 g_Down(127,0,127);


Tile::Tile( const IntVec2& tileCoords, const AABB2& tileSize, TileType tileType, int tileIndex, Game* game ) :
	m_tileCoords(tileCoords),
	m_tileSize(tileSize),
	m_tileType(tileType),
	m_tileIndex(tileIndex),
	m_game(game)
{}

void Tile::Update( float deltaSeconds )
{
	if( m_isFlusttering )
	{

		//static float timer = 5.f;
		m_timer -= deltaSeconds;
		if( m_timer < 5.1f && m_timer > 4.9f )
		{
			m_flutter = TILECOLORFLUTTER;
		}
		else if( m_timer < 4.1f && m_timer > 3.9f )
		{
			m_flutter = TILECOLORFLUTTER - 10;
		}
		else if( m_timer < 3.1f && m_timer > 2.9f )
		{
			m_flutter = TILECOLORFLUTTER - 20;
		}
		else if( m_timer < 2.1f && m_timer > 1.9f )
		{
			m_flutter = TILECOLORFLUTTER - 30;
		}
		else if( m_timer < 1.1f && m_timer > 0.9f )
		{
			m_flutter = TILECOLORFLUTTER - 40;
		}
		if( m_timer < 0.f )
		{
			m_isFlusttering = false;
			m_timer = 5.f;
			m_flutter = 0;
		}
	}
	for( int hitIndex = 0; hitIndex < MAXPLAYERCOUNT; hitIndex++ )
	{
		if( m_isHit[hitIndex] )
		{
			m_wasHit[hitIndex] = true;
		}
		else
		{
			m_wasHit[hitIndex] = false;
		}
	}



}

void Tile::Render() const
{
	AABB2 transformedTile = m_tileSize.GetTranslated( Vec2( (float)m_tileCoords.x, (float)m_tileCoords.y ) );

	if( m_game->isDebugMode() )
	{
		if( m_tileType == SOLID || m_tileType == VISIBLESOLID )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, Rgba8( 100 + m_flutter, 33, 33 ) );

		}
		else if( m_tileType == VICTORY )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, Rgba8( 200 + m_flutter, 200, 0 ) );

		}
		else if( m_tileType == START )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, Rgba8( 100 + m_flutter, 100, 100 ) );

		}
		else if( m_tileType == VISIBLELEFT || m_tileType == INVISIBLELEFT )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, g_Left );

		}
		else if( m_tileType == VISIBLERIGHT || m_tileType == INVISIBLERIGHT )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, g_Right );

		}
		else if( m_tileType == VISIBLEUP || m_tileType == INVISIBLEUP )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, g_Up );

		}
		else if( m_tileType == VISIBLEDOWN || m_tileType == INVISIBLEDOWN )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, g_Down );

		}
		else if( m_tileType == DEATH )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, Rgba8( 33 + m_flutter, 255, 33 ) );

		}
		else
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, Rgba8( 33 + m_flutter, 33, 33 ) );

		}
	}
	else
	{
		if( m_tileType == SOLID )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, Rgba8( 33 + m_flutter, 33, 33 ) );

		}
		else if( m_tileType == VISIBLESOLID )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, Rgba8( 100 + m_flutter, 33, 33 ) );

		}
		else if( m_tileType == VICTORY )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, Rgba8( 200 + m_flutter, 200, 0 ) );

		}
		else if( m_tileType == START )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, Rgba8( 100 + m_flutter, 100, 100 ) );

		}
		else if( m_tileType == VISIBLELEFT )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, g_Left );

		}
		else if( m_tileType == VISIBLERIGHT )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, g_Right );

		}
		else if( m_tileType == VISIBLEUP )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, g_Up );

		}
		else if( m_tileType == VISIBLEDOWN )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, g_Down );

		}
		else if( m_tileType == DEATH )
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, Rgba8( 33 + m_flutter, 33, 33 ) );

		}
		else
		{
			g_theRenderer->DrawAABB2Filled( transformedTile, Rgba8( 33 + m_flutter, 33, 33 ) );

		}
	}
}

AABB2 Tile::GetAABB2() const
{
	return m_tileSize.GetTranslated(Vec2((float)m_tileCoords.x, (float)m_tileCoords.y));
}

bool Tile::isSolid() const
{
	if( m_tileType == SOLID || m_tileType == VISIBLESOLID )
	{
		return true;
	}
	return false;
}

void Tile::setHit(int playerID) {
	m_isHit[playerID] = true;
}

bool Tile::isVictory() const
{
	if( m_tileType == VICTORY )
	{
		return true;
	}
	return false;
}

void Tile::SetTileType( TileType newTileType )
{
	m_tileType = newTileType;
}

TileType Tile::getTileType() const
{
	return m_tileType;
}

bool Tile::WasJustHit(int playerID) const
{
	return m_wasHit[playerID] && !m_isHit[playerID];
}

void Tile::setFlutter()
{
	m_isFlusttering = true;
}

void Tile::setNotHit(int playerID)
{
	m_isHit[playerID] = false;
}

