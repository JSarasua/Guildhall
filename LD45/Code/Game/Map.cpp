#include "Map.hpp"
#include "Tile.hpp"
#include "Entity.hpp"
#include "Game.hpp"
#include "Game/Player.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/App.hpp"

extern App* g_theApp;
extern InputSystem* g_theInput;

Map::Map() : 
	m_mapSize(0,0)
{}

Map::Map( const IntVec2& mapSize, int tileSize, Game* game ) :
	m_mapSize(mapSize),
	m_tileSize(tileSize),
	m_game(game)
{
	m_NumOftiles = m_mapSize.y * m_mapSize.x;
	int x = 0;
	int y = 0;
	AABB2 tileAABB2 = AABB2(Vec2(0.f,0.f), Vec2((float)m_tileSize, (float)m_tileSize));

	int randomInt = 0;

	for( int mapIndex = 0; mapIndex < m_NumOftiles; mapIndex++ )
	{
		y = mapIndex / (m_mapSize.x );
		x = mapIndex - y * m_mapSize.x;
		y *= m_tileSize;
		x *= m_tileSize;
		randomInt = m_game->m_rand.RollRandomIntInRange(0,100);
		if( x < SAFEZONE && y < SAFEZONE)
		{
			m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, START, mapIndex, m_game ) );

		}
		else if( (x > INTWORLD_SIZE_X * INTWORLD_SIZE_PIECES - SAFEZONE && y > INTWORLD_SIZE_Y * INTWORLD_SIZE_PIECES - SAFEZONE))
		{
			m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, VICTORY, mapIndex, m_game ) );
		}
		else 
		{
			if( randomInt > 95 )
			{
				m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, INVISIBLEUP, mapIndex, m_game ) );

			}
			else if( randomInt > 90 )
			{
				m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, INVISIBLEDOWN, mapIndex, m_game ) );

			}
			else if( randomInt > 85 )
			{
				m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, INVISIBLELEFT, mapIndex, m_game ) );

			}
			else if( randomInt > 80 )
			{
				m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, INVISIBLERIGHT, mapIndex, m_game ) );

			}
			else if( randomInt > 75 )
			{
				m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, SOLID, mapIndex, m_game ) );

			}
			else
			{
				m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, NOTSOLID, mapIndex, m_game ) );

			}
		}

	}

	for( int entityIndex = 0; entityIndex < MAXPLAYERCOUNT; entityIndex++ )
	{
		m_entities.push_back(new Player( Vec2( m_game->m_rand.RollRandomFloatInRange(2.f,15.f), m_game->m_rand.RollRandomFloatInRange(2.f,15.f) ), entityIndex,  m_game ));
	}

}

Map::~Map()
{

}

Map::Map( int mapWidth, int mapHeight, int tileSize, Game* game ) :
	m_mapSize(mapWidth, mapHeight),
	m_tileSize(tileSize),
	m_game(game)
{
	m_NumOftiles = m_mapSize.y * m_mapSize.x;
	int x = 0;
	int y = 0;
	AABB2 tileAABB2 = AABB2( Vec2( 0.f, 0.f ), Vec2( (float)m_tileSize, (float)m_tileSize ) );

	int randomInt = 0;

	for( int mapIndex = 0; mapIndex < m_NumOftiles; mapIndex++ )
	{
		y = mapIndex / (m_mapSize.x);
		x = mapIndex - y * m_mapSize.x;
		y *= m_tileSize;
		x *= m_tileSize;
		randomInt = m_game->m_rand.RollRandomIntInRange( 0, 100 );
		if( x < SAFEZONE && y < SAFEZONE )
		{
			m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, START, mapIndex, m_game ) );

		}
		else if( (x > INTWORLD_SIZE_X * INTWORLD_SIZE_PIECES - SAFEZONE && y > INTWORLD_SIZE_Y * INTWORLD_SIZE_PIECES - SAFEZONE) )
		{
			m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, VICTORY, mapIndex, m_game ) );
		}
		else
		{
			if( randomInt > 90 )
			{
				m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, INVISIBLEUP, mapIndex, m_game ) );

			}
			else if( randomInt > 80 )
			{
				m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, INVISIBLEDOWN, mapIndex, m_game ) );

			}
			else if( randomInt > 70 )
			{
				m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, INVISIBLELEFT, mapIndex, m_game ) );

			}
			else if( randomInt > 60 )
			{
				m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, INVISIBLERIGHT, mapIndex, m_game ) );

			}
			else if( randomInt > 50 )
			{
				m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, SOLID, mapIndex, m_game ) );

			}
			else
			{
				m_tiles.push_back( Tile( IntVec2( x, y ), tileAABB2, NOTSOLID, mapIndex, m_game ) );

			}
		}

	}


	for( int entityIndex = 0; entityIndex < MAXPLAYERCOUNT; entityIndex++ )
	{
		m_entities.push_back( new Player( Vec2( m_game->m_rand.RollRandomFloatInRange( 2.f, 15.f ), m_game->m_rand.RollRandomFloatInRange( 2.f, 15.f ) ), entityIndex, m_game ) );
	}
}

void Map::Startup()
{

}

void Map::Shutdown()
{

	while( m_entities.size() > 0 )
	{
		m_entities.pop_back();
	}
}

void Map::RunFrame()
{

}

void Map::Update( float deltaSeconds )
{


	static float winTimer = 3.f;
	static int counter = 1;
	for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		m_entities[entityIndex]->Update(deltaSeconds);
		for( int playerIndex = 0; playerIndex < m_entities.size(); playerIndex++ )
		{
			if( entityIndex != playerIndex && m_entities[entityIndex]->IsAlive() && m_entities[playerIndex]->IsAlive())
			{
				PushDiscsOutOfEachOther2D(m_entities[entityIndex]->GetPosition(),PLAYER_SIZE,m_entities[playerIndex]->GetPosition(), PLAYER_SIZE);
			}
		}
	}
	for( int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		m_tiles[tileIndex].Update(deltaSeconds);
	}
	CheckCollisions();
	if( m_StartNextLevel )
	{
		
		winTimer -= deltaSeconds;
		if( winTimer < 0.f )
		{
			StartNextLevel();
			m_StartNextLevel = false;
			winTimer = 3.f;
		}
	}
}

void Map::Render()
{
	for( int mapIndex = 0; mapIndex < m_tiles.size(); mapIndex++ )
	{
		m_tiles[mapIndex].Render();
	}
	

	for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		m_entities[entityIndex]->Render();
	}
}

void Map::CheckCollisions()
{
	
	for( int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ )
	{
		for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
		{
			if( m_entities[entityIndex]->IsAlive() )
			{
				if( DoDiscAndAABBOverlap2D( m_entities[entityIndex]->GetPosition(), PLAYER_SIZE, m_tiles[tileIndex].GetAABB2() ) )
				{
					m_tiles[tileIndex].setFlutter();
					if( m_tiles[tileIndex].getTileType() == DEATH )
					{
						static_cast<Player*>(m_entities[entityIndex])->ResetPosition();
						m_tiles[tileIndex].setFlutter();
						m_tiles[tileIndex].setHit(entityIndex);

					}
					else if( m_tiles[tileIndex].getTileType() == SOLID || m_tiles[tileIndex].getTileType() == VISIBLESOLID )
					{
						PushDiscOutOfAABB2D( m_entities[entityIndex]->GetPosition(), PLAYER_SIZE, m_tiles[tileIndex].GetAABB2() );
						m_tiles[tileIndex].SetTileType( VISIBLESOLID );
						m_tiles[tileIndex].setFlutter();
						m_tiles[tileIndex].setHit(entityIndex);

					}
					else if( m_tiles[tileIndex].isVictory() )
					{
						m_StartNextLevel = true;
					}

					else if( m_tiles[tileIndex].getTileType() == INVISIBLELEFT || m_tiles[tileIndex].getTileType() == VISIBLELEFT )
					{
						m_tiles[tileIndex].SetTileType( VISIBLELEFT );
						m_tiles[tileIndex].setFlutter();
						m_tiles[tileIndex].setHit(entityIndex);

					}

					else if( m_tiles[tileIndex].getTileType() == INVISIBLERIGHT || m_tiles[tileIndex].getTileType() == VISIBLERIGHT )
					{
						m_tiles[tileIndex].SetTileType( VISIBLERIGHT );
						m_tiles[tileIndex].setFlutter();
						m_tiles[tileIndex].setHit(entityIndex);

					}
					else if( m_tiles[tileIndex].getTileType() == INVISIBLEUP || m_tiles[tileIndex].getTileType() == VISIBLEUP )
					{
						m_tiles[tileIndex].SetTileType( VISIBLEUP );
						m_tiles[tileIndex].setFlutter();
						m_tiles[tileIndex].setHit(entityIndex);

					}
					else if( m_tiles[tileIndex].getTileType() == INVISIBLEDOWN || m_tiles[tileIndex].getTileType() == VISIBLEDOWN )
					{
						m_tiles[tileIndex].SetTileType( VISIBLEDOWN );
						m_tiles[tileIndex].setFlutter();
						m_tiles[tileIndex].setHit(entityIndex);

					}

				}
				else
				{
					m_tiles[tileIndex].setNotHit(entityIndex);
					if( m_tiles[tileIndex].WasJustHit(entityIndex) )
					{
						if( m_tiles[tileIndex].getTileType() == INVISIBLELEFT || m_tiles[tileIndex].getTileType() == VISIBLELEFT )
						{
							if( m_entities[entityIndex]->GetPosition().x + PLAYER_SIZE > m_tiles[tileIndex].GetAABB2().mins.x )
							{
								static_cast<Player*>(m_entities[entityIndex])->ResetPosition();
							}
						}
						else if( m_tiles[tileIndex].getTileType() == INVISIBLERIGHT || m_tiles[tileIndex].getTileType() == VISIBLERIGHT )
						{
							if( m_entities[entityIndex]->GetPosition().x - PLAYER_SIZE < m_tiles[tileIndex].GetAABB2().maxs.x )
							{
								static_cast<Player*>(m_entities[entityIndex])->ResetPosition();
							}
						}
						else if( m_tiles[tileIndex].getTileType() == INVISIBLEUP || m_tiles[tileIndex].getTileType() == VISIBLEUP )
						{
							if( m_entities[entityIndex]->GetPosition().y - PLAYER_SIZE < m_tiles[tileIndex].GetAABB2().maxs.y )
							{
								static_cast<Player*>(m_entities[entityIndex])->ResetPosition();
							}
						}
						else if( m_tiles[tileIndex].getTileType() == INVISIBLEDOWN || m_tiles[tileIndex].getTileType() == VISIBLEDOWN )
						{
							if( m_entities[entityIndex]->GetPosition().y + PLAYER_SIZE > m_tiles[tileIndex].GetAABB2().mins.y )
							{
								static_cast<Player*>(m_entities[entityIndex])->ResetPosition();
							}
						}
					}
				}
			}

		}
		
	}
	
}

void Map::StartNextLevel()
{
	for( int entityIndex = 0; entityIndex < MAXPLAYERCOUNT; entityIndex++ )
	{
		static_cast<Player*>(m_entities[entityIndex])->ResetPosition();
	}

	int x = 0;
	int y = 0;
	int randomInt = 0;

	for( int mapIndex = 0; mapIndex < m_NumOftiles; mapIndex++ )
	{
		y = mapIndex / (m_mapSize.x);
		x = mapIndex - y * m_mapSize.x;
		y *= m_tileSize;
		x *= m_tileSize;
		randomInt = m_game->m_rand.RollRandomIntInRange( 0, 100 );
		if( x < SAFEZONE && y < SAFEZONE )
		{
			//Don't generate tiles in the Safe zone
		}
		else if( (x > INTWORLD_SIZE_X * INTWORLD_SIZE_PIECES - SAFEZONE && y > INTWORLD_SIZE_Y * INTWORLD_SIZE_PIECES - SAFEZONE) )
		{
			//Don't generate tiles in the End zone
		}
		else
		{
			if( randomInt > 93 )
			{
				m_tiles[mapIndex].SetTileType(INVISIBLEUP);
			}
			else if( randomInt > 85 )
			{
				m_tiles[mapIndex].SetTileType( INVISIBLEDOWN );
			}
			else if(randomInt > 75 )
			{
				m_tiles[mapIndex].SetTileType( INVISIBLELEFT );
			}
			else if( randomInt > 67 )
			{
				m_tiles[mapIndex].SetTileType( INVISIBLERIGHT );

			}
			else if( randomInt > 60 )
			{
				m_tiles[mapIndex].SetTileType( SOLID );
			}
			else
			{
				m_tiles[mapIndex].SetTileType(NOTSOLID);
			}
		}

	}

}

