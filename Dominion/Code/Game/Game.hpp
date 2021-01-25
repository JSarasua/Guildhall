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
	GREEDY,
	DOUBLEWITCH
};

enum class ROLLOUTMETHOD
{
	RANDOM,
	HEURISTIC,
	EPSILONHEURISTIC
};

enum class EXPANSIONSTRATEGY
{
	ALLMOVES,
	HEURISTICS
};

enum class AIStrategy
{
	RANDOM,
	BIGMONEY,
	SINGLEWITCH,
	SARASUA1,
	MCTS,
	DOUBLEWITCH
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
	inputMove_t( eMoveType const& moveType, int whoseMove, int cardIndex = -1 ) :
	m_moveType( moveType ),
	m_whoseMoveIsIt( whoseMove ),
	m_cardIndex( cardIndex )
	{}

	bool operator==( inputMove_t const& compare ) const
	{
		return m_moveType == compare.m_moveType && 
			m_cardIndex == compare.m_cardIndex && 
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
			if( m_cardIndex < compare.m_cardIndex )
			{
				return true;
			}
			else if( m_whoseMoveIsIt < compare.m_whoseMoveIsIt )
			{
				return true;
			}
		}

		return false;
	}

public:
	eMoveType m_moveType = INVALID_MOVE;
	int m_cardIndex = -1;
// 	int m_cardIndexToBuy = -1;
// 	int m_cardHandIndexToPlay = -1;
	int m_whoseMoveIsIt = -1;

	//Non normal moves
	//std::vector<int> m_cardHandIndexesToDiscard;
};

struct metaData_t
{
public:
	float GetWinRate() const { return m_numberOfWins / (float)m_numberOfSimulations; }

	float m_numberOfWins = 0.f;
	int m_numberOfSimulations = 0;
};

struct pileData_t
{
public:
	bool operator==( pileData_t const& compare ) const
	{
		return m_pileSize == compare.m_pileSize &&
			m_cardIndex == compare.m_cardIndex;
	}

	static pileData_t ParsePileDataFromBuffer( byte*& buffer )
	{
		pileData_t pileData;
		pileData.m_pileSize = ParseInt( buffer );

		pileData.m_cardIndex = *(eCards*)buffer;
		buffer += sizeof( pileData.m_cardIndex);

		return pileData;
	}

	void AppendPileDataToBuffer( std::vector<byte>& buffer, size_t& startIndex )
	{
		AppendDataToBuffer( (byte*)&m_pileSize, sizeof(m_pileSize), buffer, startIndex );
		AppendDataToBuffer( (byte*)&m_cardIndex, sizeof(m_cardIndex), buffer, startIndex );
	}

public:
	int m_pileSize = -1;
	eCards m_cardIndex = eCards::INVALID_CARD;
	//CardDefinition const* m_card = nullptr;
};

struct gamestate_t
{
public:
	gamestate_t() = default;
	gamestate_t( gamestate_t const& copyGameState ) :
		m_whoseMoveIsIt( copyGameState.m_whoseMoveIsIt ),
		m_currentPhase( copyGameState.m_currentPhase )
	{
		m_playerBoards[0] = copyGameState.m_playerBoards[0];
		m_playerBoards[1] = copyGameState.m_playerBoards[1];
		memcpy( &m_cardPiles[0], &copyGameState.m_cardPiles[0], NUMBEROFPILES * sizeof(pileData_t) );
	}
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

	static gamestate_t ParseGameStateFromBuffer( byte*& buffer )
	{
		gamestate_t gameState;

		for( pileData_t& pileData : gameState.m_cardPiles )
		{
			pileData = pileData_t::ParsePileDataFromBuffer( buffer );
		}

		gameState.m_playerBoards[0] = PlayerBoard::ParsePlayerBoardFromBuffer( buffer );
		gameState.m_playerBoards[1] = PlayerBoard::ParsePlayerBoardFromBuffer( buffer );
		
		gameState.m_whoseMoveIsIt = ParseInt( buffer );
		
		gameState.m_currentPhase = *(eGamePhase*)buffer;
		buffer += sizeof(gameState.m_currentPhase);

		gameState.m_isFirstMove = ParseBool( buffer );

		return gameState;
	}

	void AppendGameStateToBuffer( std::vector<byte>& buffer, size_t& startIndex ) const
	{
		for( pileData_t pileData : m_cardPiles )
		{
			pileData.AppendPileDataToBuffer( buffer, startIndex );
		}

		m_playerBoards[0].AppendPlayerBoardToBuffer( buffer, startIndex );
		m_playerBoards[1].AppendPlayerBoardToBuffer( buffer, startIndex );
		AppendDataToBuffer( (byte*)&m_whoseMoveIsIt, sizeof(m_whoseMoveIsIt), buffer, startIndex );
		AppendDataToBuffer( (byte*)&m_currentPhase, sizeof(m_currentPhase), buffer, startIndex );
		AppendDataToBuffer( (byte*)&m_isFirstMove, sizeof(m_isFirstMove), buffer, startIndex );
	}

public:
	//Card Piles depicting what cards are in the game and how many are in that pile
	std::array< pileData_t, NUMBEROFPILES > m_cardPiles {};

	PlayerBoard m_playerBoards[2];
	int m_whoseMoveIsIt = PLAYER_1;
	eGamePhase m_currentPhase = CLEANUP_PHASE;
	bool m_isFirstMove = false;

};

struct data_t
{
	data_t() = default;
	data_t( metaData_t const& metaData, gamestate_t gameState ) :
		m_metaData( metaData ),
		m_currentGamestate( gameState )
	{	}

	metaData_t m_metaData;
	//Don't do pointer
	//gamestate_t* m_currentGamestate = nullptr;
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
	int IsGameOverForGameState( gamestate_t const& gameState ) const;
	int IsGameOver();


	std::vector<inputMove_t> GetValidMovesAtGameState( gamestate_t const& gameState ) const;
	int GetNumberOfValidMovesAtGameState( gamestate_t const& gameState );
	gamestate_t GetGameStateAfterMove( gamestate_t const& currentGameState, inputMove_t const& move );
	inputMove_t GetBestMoveUsingAIStrategy( AIStrategy aiStrategy );
	inputMove_t GetRandomMoveAtGameState( gamestate_t const& currentGameState );
	inputMove_t GetMoveUsingBigMoney( gamestate_t const& currentGameState );
	inputMove_t GetMoveUsingSingleWitch( gamestate_t const& currentGameState );
	inputMove_t GetMoveUsingDoubleWitch( gamestate_t const& currentGameState );
	inputMove_t GetMoveUsingSarasua1( gamestate_t const& currentGameState );
	inputMove_t GetMoveUsingHighestVP( gamestate_t const& currentGameState );
	gamestate_t GetRandomInitialGameState();
	void RandomizeUnknownInfoForGameState( gamestate_t& currentGameState );


	int GetCurrentPlayersScore( gamestate_t const& currentGameState );
	int GetOpponentsScore( gamestate_t const& currentGameState );

	std::vector<int> GetCurrentBuyIndexes() const;

	void AppendGameStateToFile( gamestate_t const& gameState, std::string const& filePath );
	gamestate_t ParseGameStateFromBuffer( byte*& buffer );

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
	ROLLOUTMETHOD m_mctsRolloutMethod = ROLLOUTMETHOD::EPSILONHEURISTIC;
	EXPANSIONSTRATEGY m_mctsExpansionStrategy = EXPANSIONSTRATEGY::HEURISTICS;
	float m_mctsExplorationParameter = 0.5f;
	float m_mctsEpsilon = 0.15f;

};
