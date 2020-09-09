#pragma once
#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/MonteCarlo.hpp"
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
class MonteCarlo;

struct light_t;
struct Vertex_PCUTBN;
struct inputMove_t;
struct gameState_t;




struct fresnel_t
{
	Vec3 color;
	float power;
	float factor;

	Vec3 pad00;
};

struct dissolve_t
{
	Vec3 edgeNearColor;
	float amount;
	Vec3 edgeFarColor;
	float edgeWidth;
};

struct projection_t
{
	Mat44 projection;

	Vec3 position;
	//float pad00;

	//Vec3 normal;
	float strength;
};

struct transformColor_t
{
	Mat44 transformColor;

	Vec3 tint;
	float tintPower = 0.f;

	Vec3 pad00;
	float transformPower = 0.f;
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

	std::vector<inputMove_t> GetValidMovesAtGameState( gameState_t const& gameState );
	int GetNumberOfValidMovesAtGameState( gameState_t const& gameState );
	bool IsGameStateWon( gameState_t const& gameState );
	void UpdateGameStateIfValid( inputMove_t const& inputMove );
	void UpdateGameStateIfValid( inputMove_t const& inputMove, gameState_t& gameState );
	bool IsMoveValidForGameState( inputMove_t const& inputMove, gameState_t const& gameState );
	gameState_t GetGameStateFromInput( inputMove_t const& inputMove, gameState_t const& currentGameState );
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

	MonteCarlo m_mcAI;
	gameState_t m_currentGameState;
	inputMove_t m_currentInputMove;
	bool m_isInputPop = true;

private:
	void RenderDevConsole();
};
