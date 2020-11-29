#include "Game/EnemySpawner.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Entity.hpp"
#include "Game/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Game/EntityFactory.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"

EnemySpawner::EnemySpawner( Vec2 const& position, FloatRange const& spawnRadius, IntRange const& numberOfEnemiesToSpawn, float playerCheckRadius, Actor* player, Map* currentMap ) :
	m_position(position),
	m_spawnRadius( spawnRadius ),
	m_playerCheckRadius( playerCheckRadius ),
	m_player( player ),
	m_map( currentMap )
{
	m_numberOfEnemiesToSpawn = numberOfEnemiesToSpawn.GetRandomInRange( g_theGame->m_rand );
	m_isSpawning = false;
	m_spawnTimer.SetSeconds( 0.5 );
}

void EnemySpawner::AddEnemyType( ActorDefinition const* enemyType )
{
	m_enemyTypes.push_back( enemyType );
}

void EnemySpawner::AddWeaponType( WeaponDefinition const* weaponType )
{
	m_weapons.push_back( weaponType );
}

void EnemySpawner::SetPlayer( Actor* player )
{
	m_player = player;
}

void EnemySpawner::Update()
{
	std::vector<Actor*> enemies;
	g_theGame->m_world->GetPlayers( enemies );

	Vec2 closestEnemyPos;
	float minimumDistance = 999999.f;
	for( Actor* enemy : enemies )
	{
		if( enemy )
		{
			Vec2 const& enemyPos = enemy->GetPosition();
			float distance = GetDistance2D( m_position, enemyPos );
			if( distance < minimumDistance )
			{
				closestEnemyPos = enemyPos;
				minimumDistance = distance;
			}
		}
	}


	if( !m_isSpawning )
	{
		if( m_player )
		{
			if( IsPointInsideDisc2D( closestEnemyPos, m_position, m_playerCheckRadius ) )
			{
				m_isSpawning = true;
				m_spawnTimer.Reset();
			}
		}
	}
	else
	{
		if( m_numberOfEnemiesSpawned < m_numberOfEnemiesToSpawn )
		{
			if( m_spawnTimer.CheckAndDecrement() )
			{
				RandomNumberGenerator& randGen = g_theGame->m_rand;
				float randOrientation = randGen.RollRandomFloatInRange( 0.f, 359.9f );
				float spawnRadius = m_spawnRadius.GetRandomInRange( randGen );
				Vec2 randomOffset = Vec2::MakeFromPolarDegrees( randOrientation, spawnRadius );
				Vec2 spawnPosition = m_position + randomOffset;

				while( !m_map->IsTileSolid( IntVec2( spawnPosition ) ) )
				{
					randOrientation = randGen.RollRandomFloatInRange( 0.f, 359.9f );
					spawnRadius = m_spawnRadius.GetRandomInRange( randGen );
					randomOffset = Vec2::MakeFromPolarDegrees( randOrientation, spawnRadius );
					spawnPosition = m_position + randomOffset;
				}

				int numOfEnemyTypes = (int)m_enemyTypes.size() - 1;
				int randomEnemyType = randGen.RollRandomIntInRange( 0, numOfEnemyTypes );

				int numOfWeaponTypes = (int)m_weapons.size() - 1;
				int randomWeaponType = randGen.RollRandomIntInRange( 0, numOfWeaponTypes );

				//Actor* entityToSpawn = new Actor( spawnPosition, Vec2(), 0.f, 0.f, m_enemyTypes[randomEnemyType] );
				Actor* entityToSpawn = EntityFactory::CreateActor( m_enemyTypes[randomEnemyType], spawnPosition );
				entityToSpawn->SetEnemy( m_player );
				entityToSpawn->AddWeapon( m_weapons[randomWeaponType] );
				m_map->SpawnEntity( entityToSpawn );
				m_numberOfEnemiesSpawned++;
			}
		}
	}
}

