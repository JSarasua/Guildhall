#pragma once
#include "Entity.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Time/Timer.hpp"
#include "Game/PlayerBoard.hpp"
#include "Game/CardDefinition.hpp"
#include <vector>
#include <deque>
#include <queue>
#include <array>


class Clock;
class GPUMesh;
class Material;
class Player;
class Shader;
class ShaderState;
class World;
//class MonteCarlo;
class MonteCarloNoTree;
class MonteCarlo;

struct light_t;
struct Vertex_PCUTBN;


enum class SIMMETHOD
{
	RANDOM,
	BIGMONEY,
	SINGLEWITCH,
	SARASUA1,
	GREEDY
};

enum class AIStrategy
{
	RANDOM,
	BIGMONEY,
	SINGLEWITCH,
	SARASUA1,
	MCTS
};

enum eGamePhase
{
	INVALID_PHASE = -1,
	BUY_PHASE,
	ACTION_PHASE,
	CLEANUP_PHASE, //Phase exists but not in use since it happens on ending of phase

	//Non Normal phases
	DISCARD_DOWN_TO_3_PHASE
};

enum eMoveType
{
	INVALID_MOVE = -1,
	BUY_MOVE,
	PLAY_CARD,
	END_PHASE,

	//Non Normal moves
	DISCARD_DOWN_TO_3
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

	bool operator==( inputMove_t const& compare ) const
	{
		return m_moveType == compare.m_moveType && 
			m_cardIndexToBuy == compare.m_cardIndexToBuy && 
			m_cardHandIndexToPlay == compare.m_cardHandIndexToPlay && 
			m_whoseMoveIsIt == compare.m_whoseMoveIsIt;
	}

	bool operator<( inputMove_t const& compare ) const
	{
		if( m_moveType < compare.m_moveType )
		{
			return true;
		}
		else if( m_moveType == compare.m_moveType )
		{
			if( m_cardHandIndexToPlay < compare.m_cardHandIndexToPlay )
			{
				return true;
			}
			else if( m_cardHandIndexToPlay == compare.m_cardHandIndexToPlay )
			{
				if( m_cardIndexToBuy < compare.m_cardIndexToBuy )
				{
					return true;
				}
				else if( m_cardIndexToBuy == compare.m_cardIndexToBuy )
				{
					if( m_whoseMoveIsIt < compare.m_whoseMoveIsIt )
					{
						return true;
					}
				}
			}
		}

		return false;
	}

public:
	eMoveType m_moveType = INVALID_MOVE;
	int m_cardIndexToBuy = -1;
	int m_cardHandIndexToPlay = -1;
	int m_whoseMoveIsIt = -1;

	//Non normal moves
	//std::vector<int> m_cardHandIndexesToDiscard;
};

struct metaData_t
{
public:
	float GetWinRate() { return m_numberOfWins / (float)m_numberOfSimulations; }

	float m_numberOfWins = 0;
	int m_numberOfSimulations = 0;
};

struct pileData_t
{
public:

	bool operator==( pileData_t const& compare ) const
	{
		return m_pileSize == compare.m_pileSize &&
			m_card == compare.m_card;
	}

	int m_pileSize = -1;
	CardDefinition const* m_card = nullptr;
};

struct gamestate_t
{
public:
	gamestate_t() = default;
	gamestate_t( pileData_t gamePiles[NUMBEROFPILES], PlayerBoard const& player1Deck, PlayerBoard const& player2Deck, int whoseMove, eGamePhase const& currentPhase ) :
		m_whoseMoveIsIt( whoseMove ), m_currentPhase( currentPhase )
	{
		m_playerBoards[0] = player1Deck;
		m_playerBoards[1] = player2Deck;
		memcpy( &m_cardPiles[0], gamePiles, NUMBEROFPILES * sizeof(pileData_t) );
	}

	void ResetDecks()
	{
		m_playerBoards[0].ResetBoard();
		m_playerBoards[1].ResetBoard();
	}

	void ShuffleDecks()
	{
		m_playerBoards[0].ShuffleDeck();
		m_playerBoards[1].ShuffleDeck();
	}

	bool UnordereredEqualsOnlyCurrentPlayer( gamestate_t const& compare ) const
	{
		if( m_whoseMoveIsIt == compare.m_whoseMoveIsIt &&
			m_currentPhase == compare.m_currentPhase )
		{
			bool playerCompare = m_playerBoards[m_whoseMoveIsIt].UnorderedCompare( compare.m_playerBoards[m_whoseMoveIsIt] );
			bool pileCompare = (m_cardPiles == compare.m_cardPiles);

			return playerCompare && pileCompare;
		}

		return false;
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

	PlayerBoard& GetEditableCurrentPlayerBoard()
	{
		return m_playerBoards[m_whoseMoveIsIt];
	}
	PlayerBoard const& GetCurrentPlayerBoard()
	{
		return m_playerBoards[m_whoseMoveIsIt];
	}

public:
	//Card Piles depicting what cards are in the game and how many are in that pile
//	pileData_t m_cardPiles[NUMBEROFPILES] {};
	std::array< pileData_t, NUMBEROFPILES > m_cardPiles {};

	PlayerBoard m_playerBoards[2];
	int m_whoseMoveIsIt = PLAYER_1;
	eGamePhase m_currentPhase = CLEANUP_PHASE;
	bool m_isFirstMove = false;

};

struct data_t
{
	data_t() = default;
	data_t( metaData_t const& metaData, gamestate_t* gameState ) :
		m_metaData( metaData ),
		m_currentGamestate( gameState )
	{	}

	metaData_t m_metaData;
	gamestate_t* m_currentGamestate = nullptr;
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
	inputMove_t GetBestMoveUsingAIStrategy( AIStrategy aiStrategy );
	inputMove_t GetRandomMoveAtGameState( gamestate_t const& currentGameState );
	inputMove_t GetMoveUsingBigMoney( gamestate_t const& currentGameState );
	inputMove_t GetMoveUsingSingleWitch( gamestate_t const& currentGameState );
	inputMove_t GetMoveUsingSarasua1( gamestate_t const& currentGameState );
	gamestate_t GetRandomInitialGameState();

private:
	void InitializeGameState();
	void RestartGame();

	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateCamera( float deltaSeconds );
	void RenderGame();
	void RenderUI();
	void CheckButtonPresses(float deltaSeconds);

	void DebugDrawGame();
	void DebugDrawGameStateInfo();
	void AutoPlayGame();

private:
	Clock* m_gameClock = nullptr;

	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	Camera m_camera;

public:
	inputMove_t m_randomMove;


	Rgba8 m_clearColor = Rgba8::BLACK;
	float m_currentTime = 0.f;
	RandomNumberGenerator m_rand;

	gamestate_t* m_currentGameState = nullptr;

	MonteCarlo* m_mcts = nullptr;
	MonteCarloNoTree* m_mc = nullptr;
	bool m_isAutoPlayEnabled = false;
	bool m_isDebugScreenEnabled = false;

	Timer m_timer;
	int m_simCount = 0;
	int m_totalSimCount = 0;

private:
	void RenderDevConsole();
	SIMMETHOD m_mctsSimMethod = SIMMETHOD::RANDOM; 
	AIStrategy m_player1Strategy = AIStrategy::MCTS;
	AIStrategy m_player2Strategy = AIStrategy::SINGLEWITCH;

};
