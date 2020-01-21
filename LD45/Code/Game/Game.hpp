#pragma once
#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


class Map;

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
	bool isDebugMode();
	void toggleDebugMode();
private:


	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateCamera( float deltaSeconds );
	void RenderGame();
	void RenderUI();
	void CheckButtonPresses(float deltaSeconds);

private:

	Camera m_camera;
	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	bool m_debugMode = false;
	int m_currentLevel = 0;
	Map* m_map = nullptr;



public:

	RandomNumberGenerator m_rand;
};
