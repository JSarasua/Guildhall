#pragma once
#include "Entity.hpp"
#include "PlayerShip.hpp"
#include "Bullet.hpp"
#include "Asteroid.hpp"
#include "Beetle.hpp"
#include "Wasp.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Debris.hpp"
#include "PowerUp.hpp"



enum PowerUps
{
	RAPID_FIRE,
	BOUNCING_BULLET,
	SCATTER_SHOT,


	NUM_POWERUPS
};

class Game
{
public:
	Game();
	~Game();
	void Startup();
	void Shutdown();
	void RunFrame();

	void Update(float deltaSeconds);
	void Render();


	int CollidingWithWall(Vec2 m_position, float radius, bool isPlayer);
	void ShootBullet();
	void ShootBulletFromPosition(Vec2 initialPosition);
	void CreateAsteroid();

	void ResurrectPlayer();
	void FlipDebugMode();
	const Vec2 GetPlayerPosition();
	int GetCurrentLevel();
	void IncrementLevel();
private:


	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateCamera( float deltaSeconds );
	void CreateDebris( const Vec2 initialPosition, const Rgba8& debrisColor, Vec2 initialVelocity, int debrisCount );
	void RenderGame();
	void RenderUI();
	void CheckButtonPresses(float deltaSeconds);

	void RenderLogo();
	bool IsLevelOver();
	void SpawnEnemies();
	Vec2 GetRandomPositionOutsideWorld();

	void AddAPowerUp();
	void DeactivatePowerUps();
private:

	Camera m_camera;
	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	bool m_debugMode = false;
	PlayerShip m_player;
	Bullet m_bullets[MAX_BULLETS];
	Asteroid m_asteroids[MAX_ASTEROIDS];
	Beetle m_beetles[MAX_BEETLES];
	Wasp m_wasps[MAX_WASPS];
	Debris m_debris[MAX_DEBRIS];
	int m_currentLevel = 0;
	PowerUp m_powerUps[MAX_POWERUPS];
	bool m_activePowerUps[MAX_POWERUPS];



public:

	RandomNumberGenerator m_rand;
};
