#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Time/Timer.hpp"
#include <vector>

class Map;
class Actor;
class ActorDefinition;
class WeaponDefinition;

class EnemySpawner
{
public:
	EnemySpawner( Vec2 const& position, FloatRange const& spawnRadius, IntRange const& numberOfEnemiesToSpawn, float playerCheckRadius, Actor* player, Map* currentMap );
	~EnemySpawner() {}

	void AddEnemyType( ActorDefinition const* enemyType );
	void AddWeaponType( WeaponDefinition const* weaponType );

	void Update();

private:
	Vec2 m_position;
	FloatRange m_spawnRadius;
	int m_numberOfEnemiesToSpawn;
	int m_numberOfEnemiesSpawned = 0;
	float m_playerCheckRadius;

	Timer m_spawnTimer;
	bool m_isSpawning = false;

	Actor* m_player = nullptr;
	Map* m_map = nullptr;

	std::vector<ActorDefinition const*> m_enemyTypes;
	std::vector<WeaponDefinition const*> m_weapons;
};