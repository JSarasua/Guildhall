#pragma once
#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/MonteCarlo_ToH.hpp"
#include <vector>
#include <deque>
#include <queue>


class Clock;
class GPUMesh;
class Material;
class Player;
class Shader;
class ShaderState;
class World;

struct light_t;
struct Vertex_PCUTBN;
struct inputMove_t;
struct gameState_t;





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

private:
	void InitializeGameState();

	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateCamera( float deltaSeconds );
	void RenderGame();
	void RenderUI();
	void CheckButtonPresses(float deltaSeconds);


private:
	Clock* m_gameClock = nullptr;

	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	Camera m_camera;

public:
	
	Rgba8 m_clearColor = Rgba8::BLACK;
	float m_currentTime = 0.f;
	RandomNumberGenerator m_rand;


	bool m_isInputPop = true;

private:
	void RenderDevConsole();
};
