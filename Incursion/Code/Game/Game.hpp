#pragma once
#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


class Player;
class World;

enum GameState
{
	LOADING,
	ATTRACT,
	DEATH,
	VICTORY,
	PAUSED,
	PLAYING,

	NUM_OF_GAMESTATES
};

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

private:


	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateCameraPlaying( float deltaSeconds );
	void RenderGame();
	void RenderUI();

	IntVec2 GetCurrentMapBounds() const;

	void UpdateLoading( float deltaSeconds );
	void UpdateAttract( float deltaSeconds );
	void UpdateDeath( float deltaSeconds );
	void UpdateVictory( float deltaSeconds );
	void UpdatePaused( float deltaSeconds );
	void UpdatePlaying( float deltaSeconds );


	void CheckButtonPressesLoading( float deltaSeconds );
	void CheckButtonPressesAttract( float deltaSeconds );
	void CheckButtonPressesDeath( float deltaSeconds );
	void CheckButtonPressesVictory( float deltaSeconds );
	void CheckButtonPressesPaused( float deltaSeconds );
	void CheckButtonPressesPlaying(float deltaSeconds);

	void RenderLoading();
	void RenderAttract();
	void RenderDeath();
	void RenderVictory();
	void RenderPaused();
	void RenderPlaying();
private:

	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	Camera m_camera;
	Vec2 m_cameraPosition;

	int m_currentLevel = 0;

	World* m_world = nullptr;
	Player* m_player = nullptr;
	Vec2 m_lastPlayerPosition;
	GameState m_currentGameState = LOADING;


	float m_numTilesInViewVertically = 0.f;
	float m_numTilesInViewHorizontally = 0.f;
public:

	RandomNumberGenerator m_rand;

private:
	void LoadAssets();
};
