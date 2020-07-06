#include "Game/EnemySpawner.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Entity.hpp"
#include "Game/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/WeaponDefinition.hpp"

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
	if( !m_isSpawning )
	{
		if( m_player )
		{
			if( IsPointInsideDisc2D( m_player->GetPosition(), m_position, m_playerCheckRadius ) )
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
				Actor* entityToSpawn = new Actor( spawnPosition, Vec2(), 0.f, 0.f, m_enemyTypes[0] );
				entityToSpawn->SetEnemy( m_player );
				entityToSpawn->AddWeapon( m_weapons[0] );
				m_map->SpawnEntity( entityToSpawn );
				m_numberOfEnemiesSpawned++;
			}
		}
	}
}

