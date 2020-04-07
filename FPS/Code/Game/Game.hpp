#pragma once
#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


class Player;
class World;
class Shader;
class GPUMesh;
class Clock;

struct light_t;


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

	void Update();
	void Render();
	void RenderCircleOfSpheres();

private:


	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateCamera( float deltaSeconds );
	void RenderGame();
	void RenderUI();
	void CheckButtonPresses(float deltaSeconds);
	IntVec2 GetCurrentMapBounds() const;
	void SetLightPosition( Vec3 const& pos );

private:
	Mat44 m_cubeModelMatrix;
	Mat44 m_circleOfSpheresModelMatrix;
	int m_numberOfCirclingCubes = 0;

	Clock* m_gameClock = nullptr;

	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	Camera m_camera;
	Vec2 m_cameraPosition;

	int m_currentLevel = 0;

	World* m_world = nullptr;
	Player* m_player = nullptr;
	GPUMesh* m_cubeMesh = nullptr;
	GPUMesh* m_sphereMesh = nullptr;
	Texture* m_screenTexture = nullptr;

	Shader* m_invertShader = nullptr;
	Shader* m_litShader = nullptr;

	light_t* m_light = nullptr;

	float m_numTilesInViewVertically = 0.f;
	float m_numTilesInViewHorizontally = 0.f;
public:
	Rgba8 m_clearColor = Rgba8::BLACK;
	float m_currentTime = 0.f;
	RandomNumberGenerator m_rand;

private:
	void RenderDevConsole();
};
