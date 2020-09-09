#pragma once
#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
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

int constexpr TOPLEFT		= 0;
int constexpr TOPMIDDLE		= 1;
int constexpr TOPRIGHT		= 2;
int constexpr MIDDLELEFT	= 3;
int constexpr MIDDLEMIDDLE	= 4;
int constexpr MIDDLERIGHT	= 5;
int constexpr BOTTOMLEFT	= 6;
int constexpr BOTTOMMIDDLE	= 7;
int constexpr BOTTOMRIGHT	= 8;

int constexpr CIRCLEPLAYER = 1;
int constexpr XPLAYER = 2;


struct inputMove_t
{
	int m_move = -1;
};

struct metaData_t
{
	int m_numberOfWins = 0;
	int m_numberOfSimulations = 0;
};


struct gamestate_t
{
public:
	int gameArray[9]{};
	bool m_isCirclesMove = true;
};

struct data_t
{
	metaData_t m_metaData;

	inputMove_t m_moveToReachNode;
	gamestate_t m_currentGamestate;
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

private:
	void InitializeGameState();

	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateCamera( float deltaSeconds );
	void RenderGame();
	void RenderUI();
	void CheckButtonPresses(float deltaSeconds);

	void PlayMoveIfValid( int moveToPlay );
	bool IsMoveValid( int moveToPlay );
	bool IsMoveValidForGameState( int moveToPlay, gamestate_t const& gameState );
	int IsGameStateWon( gamestate_t const& gameState );
	int IsGameStateWon();

	std::vector<int> GetValidMovesAtGameState( gamestate_t const& gameState );
	int GetNumberOfValidMovesAtGameState( gamestate_t const& gameState );



private:
	Clock* m_gameClock = nullptr;

	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	Camera m_camera;

public:
	
	Rgba8 m_clearColor = Rgba8::BLACK;
	float m_currentTime = 0.f;
	RandomNumberGenerator m_rand;


	bool m_isCirclesTurn = true;
	gamestate_t m_currentGameState;

private:
	void RenderDevConsole();
};
