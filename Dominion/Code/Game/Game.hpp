#pragma once
#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Deck.hpp"
#include "Game/CardDefinition.hpp"
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
//class MonteCarlo;
class MonteCarloNoTree;

struct light_t;
struct Vertex_PCUTBN;


int constexpr PLAYER_1		= 1;
int constexpr PLAYER_2		= 2;
int constexpr TIE			= 3;

int constexpr MONEYPILESIZE = 50;
int constexpr VPPileSize = 8;
int constexpr CURSEPILESIZE = 10;
int constexpr ACTIONPILESIZE = 10;

int constexpr NUMBEROFPILES = 12;
//int constexpr ACTIONPILESIZE = 10;

// enum ePlayers
// {
// 	INVALID_PLAYER = -1,
// 	PLAYER_1,
// 	PLAYER_2,
// 	PLAYER_TIE
// };

enum eGamePhase
{
	INVALID_PHASE = -1,
	BUY_PHASE,
	ACTION_PHASE,
	CLEANUP_PHASE
};

enum eMoveType
{
	INVALID_MOVE = -1,
	BUY_MOVE,
	PLAY_CARD,
	END_PHASE
};

struct inputMove_t
{
public:
	inputMove_t() = default;
	inputMove_t( eMoveType const& moveType, int whoseMove, int cardIndexToBuy = -1, int cardHandIndexToPlay = -1 ) :
	m_moveType( moveType ),
	m_whoseMoveIsIt( whoseMove ),
	m_cardIndexToBuy( cardIndexToBuy ),
	m_cardHandIndexToPlay( cardHandIndexToPlay )
	{}

	eMoveType m_moveType = INVALID_MOVE;
	int m_cardIndexToBuy = -1;
	int m_cardHandIndexToPlay = -1;
	int m_whoseMoveIsIt = -1;
};

struct metaData_t
{
	float m_numberOfWins = 0;
	int m_numberOfSimulations = 0;
};

struct pileData_t
{
	int m_pileSize = -1;
	CardDefinition const* m_card = nullptr;
};

struct gamestate_t
{
public:
	gamestate_t() = default;
	gamestate_t( pileData_t gamePiles[NUMBEROFPILES], Deck const& player1Deck, Deck const& player2Deck, int whoseMove, eGamePhase const& currentPhase ) :
		m_player1Deck( player1Deck ), m_player2Deck( player2Deck ), m_whoseMoveIsIt( whoseMove ), m_currentPhase( currentPhase )
	{
		memcpy( m_cardPiles, gamePiles, NUMBEROFPILES * sizeof(pileData_t) );
	}


	int WhoJustMoved()
	{
		if( m_currentPhase == ACTION_PHASE )
		{
			if( m_whoseMoveIsIt == PLAYER_1 )
			{
				return PLAYER_2;
			}
			else
			{
				return PLAYER_1;
			}
		}
		else
		{
			return m_whoseMoveIsIt;
		}

	}

public:
	//Card Piles depicting what cards are in the game and how many are in that pile
	pileData_t m_cardPiles[NUMBEROFPILES] {};

	Deck m_player1Deck;
	Deck m_player2Deck;
	int m_whoseMoveIsIt = PLAYER_1;
	eGamePhase m_currentPhase = CLEANUP_PHASE;

};

struct data_t
{
	data_t() = default;
	data_t( metaData_t const& metaData, inputMove_t const& move, gamestate_t const& gameState ) :
		m_metaData( metaData ),
		m_moveToReachNode( move ),
		m_currentGamestate( gameState )
	{}

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

	void PlayMoveIfValid( inputMove_t const& moveToPlay );
	bool IsMoveValid( inputMove_t const& moveToPlay ) const;
	bool IsMoveValidForGameState( inputMove_t const&, gamestate_t const& gameState ) const;
	int IsGameOverForGameState( gamestate_t const& gameState );
	int IsGameOver();


	std::vector<inputMove_t> GetValidMovesAtGameState( gamestate_t const& gameState );
	int GetNumberOfValidMovesAtGameState( gamestate_t const& gameState );
	gamestate_t GetGameStateAfterMove( gamestate_t const& currentGameState, inputMove_t const& move );
	inputMove_t GetRandomMoveAtGameState( gamestate_t const& currentGameState );
	inputMove_t GetMoveUsingBigMoney( gamestate_t const& currentGameState );

private:
	void InitializeGameState();

	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateCamera( float deltaSeconds );
	void RenderGame();
	void RenderUI();
	void CheckButtonPresses(float deltaSeconds);

	void DebugDrawGame();
	void AutoPlayGame();

private:
	Clock* m_gameClock = nullptr;

	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	Camera m_camera;

public:
	
	Rgba8 m_clearColor = Rgba8::BLACK;
	float m_currentTime = 0.f;
	RandomNumberGenerator m_rand;

	gamestate_t* m_currentGameState = nullptr;

	//MonteCarlo* m_mcts = nullptr;
	MonteCarloNoTree* m_mc = nullptr;
	bool m_isAutoPlayEnabled = false;

private:
	void RenderDevConsole();
};
