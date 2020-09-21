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
class MonteCarlo;

struct light_t;
struct Vertex_PCUTBN;


int constexpr PLAYER_1		= 1;
int constexpr PLAYER_2		= 2;
int constexpr TIE			= 3;




struct inputMove_t
{
public:
	inputMove_t() = default;
	inputMove_t( int move ) { m_move = move; }

	int m_move = -1;
};

struct metaData_t
{
	float m_numberOfWins = 0;
	int m_numberOfSimulations = 0;
};


struct gamestate_t
{
public:
	gamestate_t() = default;
	gamestate_t( int* gameArrayToCopy, int whoseMoveNow ) { memcpy(gameArray, gameArrayToCopy, 9); whoseMoveIsIt = whoseMoveNow; }

	int WhoJustMoved() 
	{
		if( whoseMoveIsIt == PLAYER_1 )
		{
			return PLAYER_2;
		}
		else
		{
			return PLAYER_1;
		}
	}
public:
	int gameArray[9]{};
	int whoseMoveIsIt = PLAYER_1;
};

struct data_t
{
	data_t() = default;
	data_t( metaData_t const& metaData, inputMove_t const& move, gamestate_t const& gameState )
	{
		m_metaData = metaData;
		m_moveToReachNode = move;
		m_currentGamestate = gameState;
	}

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

	void PlayMoveIfValid( int moveToPlay );
	bool IsMoveValid( int moveToPlay );
	bool IsMoveValidForGameState( int moveToPlay, gamestate_t const& gameState );
	int IsGameOverForGameState( gamestate_t const& gameState );
	int IsGameOver();


	std::vector<int> GetValidMovesAtGameState( gamestate_t const& gameState );
	int GetNumberOfValidMovesAtGameState( gamestate_t const& gameState );
	gamestate_t GetGameStateAfterMove( gamestate_t const& currentGameState, inputMove_t const& move );
	inputMove_t GetRandomMoveAtGameState( gamestate_t const& currentGameState );

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

	gamestate_t m_currentGameState;

	MonteCarlo* m_mcts = nullptr;

private:
	void RenderDevConsole();
};
