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
#include "Engine/Core/BufferParser.hpp"
#include "Engine/Core/BufferWriter.hpp"
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
class MonteCarloNoTree;
class MonteCarlo;
class Widget;
class WidgetGrid;
class WidgetIncrementer;
class WidgetIncrementerFloat;
class UIManager;

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
	inputMove_t( eMoveType const& moveType, int whoseMove, int cardIndex = -1, int parameterCardIndex1 = -1, int parameterCardIndex2 = -1 ) :
	m_moveType( moveType ),
	m_whoseMoveIsIt( whoseMove ),
	m_cardIndex( cardIndex ),
	m_parameterCardIndex1( parameterCardIndex1 ),
	m_parameterCardIndex2( parameterCardIndex2 )
	{}

	bool operator==( inputMove_t const& compare ) const
	{
		return m_moveType == compare.m_moveType && 
			m_cardIndex == compare.m_cardIndex && 
			m_whoseMoveIsIt == compare.m_whoseMoveIsIt &&
			m_parameterCardIndex1 == compare.m_parameterCardIndex1 &&
			m_parameterCardIndex2 == compare.m_parameterCardIndex2;
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
			else if( m_cardIndex == compare.m_cardIndex )
			{
				if( m_whoseMoveIsIt < compare.m_whoseMoveIsIt )
				{
					return true;
				}
				else if( m_whoseMoveIsIt == compare.m_whoseMoveIsIt )
				{
					if( m_parameterCardIndex1 < compare.m_parameterCardIndex1 )
					{
						return true;
					}
					else if( m_parameterCardIndex1 == compare.m_parameterCardIndex1 )
					{
						if( m_parameterCardIndex2 < compare.m_parameterCardIndex2 )
						{
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	EventArgs ToEventArgs() const
	{
		EventArgs args;
		args.SetValue( "cardIndex", m_cardIndex );
		args.SetValue( "moveType", (int)m_moveType );
		args.SetValue( "parameterIndex1", m_parameterCardIndex1 );
		args.SetValue( "parameterIndex2", m_parameterCardIndex2 );
		args.SetValue( "whoseMove", m_whoseMoveIsIt );

		return args;
	}

	static inputMove_t CreateFromEventArgs( EventArgs const& eventArgs )
	{
		inputMove_t inputMove;
		inputMove.m_cardIndex= eventArgs.GetValue( "cardIndex", -1 );
		inputMove.m_moveType = (eMoveType)eventArgs.GetValue( "moveType", (int)INVALID_MOVE );
		inputMove.m_parameterCardIndex1 = eventArgs.GetValue( "parameterIndex1", -1 );
		inputMove.m_parameterCardIndex2 = eventArgs.GetValue( "parameterIndex2", -1 );
		inputMove.m_whoseMoveIsIt = eventArgs.GetValue( "whoseMove", -1 );

		return inputMove;
	}

	static inputMove_t ParseInputFromBuffer( byte*& buffer )
	{
		inputMove_t move;
		move.m_moveType = *(eMoveType*)buffer;
		buffer += sizeof(m_moveType);

		move.m_cardIndex = ParseInt( buffer );
		move.m_whoseMoveIsIt = ParseInt( buffer );
		move.m_parameterCardIndex1 = ParseInt( buffer );
		move.m_parameterCardIndex2 = ParseInt( buffer );

		return move;
	}

	static inputMove_t ParseInputFromBufferParser( BufferParser& bufferParser )
	{
		inputMove_t move;
		move.m_moveType = (eMoveType)bufferParser.ParseInt32();
		move.m_cardIndex = bufferParser.ParseInt32();
		move.m_whoseMoveIsIt = bufferParser.ParseInt32();
		move.m_parameterCardIndex1 = bufferParser.ParseInt32();
		move.m_parameterCardIndex2 = bufferParser.ParseInt32();

		return move;
	}

	void AppendInputToBuffer( std::vector<byte>& buffer, size_t& startIndex ) const
	{
		AppendDataToBuffer( (byte*)&m_moveType, sizeof( m_moveType ), buffer, startIndex );
		AppendDataToBuffer( (byte*)&m_cardIndex, sizeof(m_cardIndex), buffer, startIndex );
		AppendDataToBuffer( (byte*)&m_whoseMoveIsIt, sizeof( m_whoseMoveIsIt ), buffer, startIndex );
		AppendDataToBuffer( (byte*)&m_parameterCardIndex1, sizeof( m_parameterCardIndex1 ), buffer, startIndex );
		AppendDataToBuffer( (byte*)&m_parameterCardIndex2, sizeof( m_parameterCardIndex2 ), buffer, startIndex );
	}

	void AppendInputToBufferWriter( BufferWriter& bufferWriter ) const
	{
		bufferWriter.AppendInt32( m_moveType );
		bufferWriter.AppendInt32( m_cardIndex );
		bufferWriter.AppendInt32( m_whoseMoveIsIt );
		bufferWriter.AppendInt32( m_parameterCardIndex1 );
		bufferWriter.AppendInt32( m_parameterCardIndex2 );
	}

public:
	eMoveType m_moveType = INVALID_MOVE;
	int m_cardIndex = -1;
// 	int m_cardIndexToBuy = -1;
// 	int m_cardHandIndexToPlay = -1;
	int m_whoseMoveIsIt = -1;
	int m_parameterCardIndex1 = -1;
	int m_parameterCardIndex2 = -1;

	//Non normal moves
	//std::vector<int> m_cardHandIndexesToDiscard;
};

struct metaData_t
{
public:
	float GetWinRate() const { return m_numberOfWins / (float)m_numberOfSimulations; }

	static metaData_t ParseMetaDataFromBuffer( byte*& buffer )
	{
		metaData_t metaData;
		metaData.m_numberOfWins = ParseFloat( buffer );
		metaData.m_numberOfSimulations = ParseInt( buffer );

		return metaData;
	}

	static metaData_t ParseMetaDataFromBufferParser( BufferParser& bufferParser )
	{
		metaData_t metaData;
		metaData.m_numberOfWins = bufferParser.ParseFloat();
		metaData.m_numberOfSimulations = bufferParser.ParseInt32();

		return metaData;
	}

	void AppendMetaDataToBuffer( std::vector<byte>& buffer, size_t& startIndex ) const
	{
		AppendDataToBuffer( (byte*)&m_numberOfWins, sizeof( m_numberOfWins ), buffer, startIndex );
		AppendDataToBuffer( (byte*)&m_numberOfSimulations, sizeof( m_numberOfSimulations ), buffer, startIndex );
	}

	void AppendMetaDataToBufferWriter( BufferWriter& bufferWriter ) const
	{
		bufferWriter.AppendFloat( m_numberOfWins );
		bufferWriter.AppendInt32( m_numberOfSimulations );
	}

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

	static pileData_t ParsePileDataFromBufferParser( BufferParser& bufferParser )
	{
		pileData_t pileData;
		pileData.m_pileSize = bufferParser.ParseInt32();
		pileData.m_cardIndex = (eCards)bufferParser.ParseInt32();

		return pileData;
	}

	void AppendPileDataToBuffer( std::vector<byte>& buffer, size_t& startIndex )
	{
		AppendDataToBuffer( (byte*)&m_pileSize, sizeof(m_pileSize), buffer, startIndex );
		AppendDataToBuffer( (byte*)&m_cardIndex, sizeof(m_cardIndex), buffer, startIndex );
	}

	void AppendPileDataToBufferWriter( BufferWriter& bufferWriter )
	{
		bufferWriter.AppendInt32( m_pileSize );
		bufferWriter.AppendInt32( m_cardIndex );
	}

public:
	int m_pileSize = -1;
	eCards m_cardIndex = eCards::INVALID_CARD;
	//CardDefinition const* m_card = nullptr;
};

struct gamestate_t
{
public:
	//Card Piles depicting what cards are in the game and how many are in that pile
	std::array< pileData_t, NUMBEROFPILES > m_cardPiles{};

	PlayerBoard m_playerBoards[2];
	int m_whoseMoveIsIt = PLAYER_1;
	eGamePhase m_currentPhase = CLEANUP_PHASE;
	bool m_isFirstMove = false;

public:
	gamestate_t() = default;
	gamestate_t( gamestate_t const& copyGameState ) :
		m_whoseMoveIsIt( copyGameState.m_whoseMoveIsIt ),
		m_currentPhase( copyGameState.m_currentPhase ),
		m_isFirstMove( copyGameState.m_isFirstMove )
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

	//MCTS needs a compare where order doesn't matter
	//Because we don't know what the opponent has in their hand and deck we only compare the current player
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

	//Saving and loading of the gamestate
	static gamestate_t ParseGameStateFromBufferParser( BufferParser& bufferParser )
	{
		gamestate_t gameState;

		for( pileData_t& pileData : gameState.m_cardPiles )
		{
			pileData = pileData_t::ParsePileDataFromBufferParser( bufferParser );
		}

		gameState.m_playerBoards[0] = PlayerBoard::ParsePlayerBoardFromBufferParser( bufferParser );
		gameState.m_playerBoards[1] = PlayerBoard::ParsePlayerBoardFromBufferParser( bufferParser );

		gameState.m_whoseMoveIsIt = bufferParser.ParseInt32();
		gameState.m_currentPhase = (eGamePhase)bufferParser.ParseInt32();
		gameState.m_isFirstMove = bufferParser.ParseBool();

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

	void AppendGameStateToBufferWriter( BufferWriter& bufferWriter ) const 
	{
		for( pileData_t pileData : m_cardPiles )
		{
			pileData.AppendPileDataToBufferWriter( bufferWriter );
		}

		m_playerBoards[0].AppendPlayerBoardToBufferWriter( bufferWriter );
		m_playerBoards[1].AppendPlayerBoardToBufferWriter( bufferWriter );
		bufferWriter.AppendInt32( m_whoseMoveIsIt );
		bufferWriter.AppendInt32( m_currentPhase );
		bufferWriter.AppendBool( m_isFirstMove );
	}
};

struct data_t
{
	data_t() = default;
	data_t( metaData_t const& metaData, gamestate_t gameState ) :
		m_metaData( metaData ),
		m_currentGamestate( gameState )
	{	}

	static data_t ParseDataFromBuffer( byte*& buffer )
	{
		data_t data;
		data.m_metaData = metaData_t::ParseMetaDataFromBuffer( buffer );
		data.m_currentGamestate = gamestate_t::ParseGameStateFromBuffer( buffer );

		return data;
	}

	static data_t ParseDataFromBufferParser( BufferParser& bufferParser )
	{
		data_t data;
		data.m_metaData = metaData_t::ParseMetaDataFromBufferParser( bufferParser );
		data.m_currentGamestate = gamestate_t::ParseGameStateFromBufferParser( bufferParser );

		return data;
	}

	void AppendDataToBuffer( std::vector<byte>& buffer, size_t& startIndex ) const
	{
		m_metaData.AppendMetaDataToBuffer( buffer, startIndex );
		m_currentGamestate.AppendGameStateToBuffer( buffer, startIndex );
	}

	void AppendDataToBufferWriter( BufferWriter& bufferWriter )
	{
		m_metaData.AppendMetaDataToBufferWriter( bufferWriter );
		m_currentGamestate.AppendGameStateToBufferWriter( bufferWriter );
	}

public:
	metaData_t m_metaData;
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

	bool PlayMoveIfValid( EventArgs const& args );
	bool ToggleWhoseViewedOnUI( EventArgs const& args );
	bool ToggleAIScreen( EventArgs const& args );
	bool PlayCurrentAIMove( EventArgs const& args );
	bool ToggleAutoPlay( EventArgs const& args );
	bool AddSimsForPlayer1( EventArgs const& args );
	bool AddSimsForPlayer2( EventArgs const& args );
	bool ChangePlayer1Strategy( EventArgs const& args );
	bool ChangePlayer2Strategy( EventArgs const& args );
	bool ChangePlayer1UCTValue( EventArgs const& args );
	bool ChangePlayer2UCTValue( EventArgs const& args );
	bool ChangePlayer1ExpansionMethod( EventArgs const& args );
	bool ChangePlayer2ExpansionMethod( EventArgs const& args );
	bool ChangePlayer1RolloutMethod( EventArgs const& args );
	bool ChangePlayer2RolloutMethod( EventArgs const& args );
	bool TogglePlayer1UseChaosChance( EventArgs const& args );
	bool TogglePlayer2UseChaosChance( EventArgs const& args );

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
	inputMove_t GetMoveUsingSarasua2( gamestate_t const& currentGameState );
	bool TryGetBestRemodelMove( std::vector<inputMove_t> const& validMoves, inputMove_t& inputMoveToUpdate );
	inputMove_t GetMoveUsingHighestVP( gamestate_t const& currentGameState );
	gamestate_t GetRandomInitialGameState();
	void RandomizeUnknownInfoForGameState( gamestate_t& currentGameState );


	int GetCurrentPlayersScore( gamestate_t const& currentGameState );
	int GetOpponentsScore( gamestate_t const& currentGameState );

	std::vector<int> GetCurrentBuyIndexes() const;

	void AppendGameStateToFile( gamestate_t const& gameState, std::string const& filePath );
	gamestate_t ParseGameStateFromBuffer( byte*& buffer );

private:
	void StartupUI();
	void InitializeAISmallPanelWidget();
	void InitializeAILargePanelWidget();
	void InitializeCardPilesWidgets();
	void AddCountToCardWidget( Widget* cardWidget, int cardCount );
	void UpdateCardCountOnWidget( Widget* cardWidget, int cardCount );
	void MatchUIToGameState();
	void UpdateUI();
	void UpdateUIText();
	void UpdateDeckAndDiscardWidgets();
	void UpdateGameStateWidget();
	void UpdateScoreWidgets();
	void UpdateAISmallPanelWidget();
	void UpdateAILargePanelWidget();


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

	AIStrategy StringToAIStrategy( std::string const& strategyStr ) const;

private:
	Clock* m_gameClock = nullptr;

	Camera m_UICamera;
	float m_maxCameraShake = 0.f;
	Camera m_camera;

public:
	int m_whoseUIPlaying = PLAYER_1;

	inputMove_t m_randomMove;
	inputMove_t m_bufferedInputMove;


	Rgba8 m_clearColor = Rgba8::BLACK;
	float m_currentTime = 0.f;
	RandomNumberGenerator m_rand;

	gamestate_t* m_currentGameState = nullptr;

	MonteCarlo* m_player1MCTS = nullptr;
	MonteCarlo* m_player2MCTS = nullptr;
	MonteCarloNoTree* m_mc = nullptr;
	bool m_isAutoPlayEnabled = false;
	bool m_isDebugScreenEnabled = false;

	Timer m_timer;
	int m_player1SimCount = 0;
	int m_player1TotalSimCount = 0;
	int m_player2SimCount = 0;
	int m_player2TotalSimCount = 0;

	Texture const* m_cyanTexture = nullptr;
	Texture const* m_redTexture = nullptr;
	Texture const* m_greenTexture = nullptr;
	Texture const* m_darkRedTexture = nullptr;
	Texture const* m_artichokeGreenTexture = nullptr;
	Texture const* m_forestGreenTexture = nullptr;
	Texture const* m_darkDarkGreenTexture = nullptr;
	Texture const* m_greyGreenTexture = nullptr;
	Texture const* m_darkForestGreenTexture = nullptr;
	
	//Gamestate
	IntVec2 m_gameStateGridDimensions = IntVec2( 1, 4 );
	WidgetGrid* m_gameStateWidget = nullptr;
	Widget* m_currentControlledPlayer = nullptr;
	Widget* m_currentMoney = nullptr;
	Widget* m_currentBuys = nullptr;
	Widget* m_currentActions = nullptr;

	//AI simple screen
	//IntVec2 m_AIGridDimensions = IntVec2( 5, 1 );
	Widget* m_AIWidget = nullptr;
	Widget* m_AIInfoWidget = nullptr;
	Widget* m_ToggleAutoPlayWidget = nullptr;
	Widget* m_currentAIWidget = nullptr;
	Widget* m_playAIMoveWidget = nullptr;
	Widget* m_currentAIBestMoveWidget = nullptr;
	Widget* m_showAIInfoButtonWidget = nullptr;
	Widget* m_player1MCTSSimulationsWidget = nullptr;
	Widget* m_player2MCTSSimulationsWidget = nullptr;
	Widget* m_player1MCTSTimesVisitedWidget = nullptr;
	Widget* m_player2MCTSTimesVisitedWidget = nullptr;
	Widget* m_AddPlayer1Simulations = nullptr;
	Widget* m_AddPlayer2Simulations = nullptr;

	//AI More Info screen
	bool m_isAIMoreInfoScreenActive = false;
	//IntVec2 m_AIMoreInfoDimensions = IntVec2( 3, 1 );
	Widget* m_AIMoreInfoWidget = nullptr;
// 	IntVec2 m_playerAIChoosingColumnDimensions = IntVec2( 1, 4 );
// 	WidgetGrid* m_playerAIChoosingColumnWidget = nullptr;
	Widget* m_player1AITextWidget = nullptr;
	WidgetIncrementer* m_player1ChooseAIWidget = nullptr;
	Widget* m_player2AITextWidget = nullptr;
	WidgetIncrementer* m_player2ChooseAIWidget = nullptr;
\
	Widget* m_player1MCTSAIParametersWidget = nullptr;
	Widget* m_player1SelectionTextWidget = nullptr;
	WidgetIncrementerFloat* m_player1UCTScoreChangerWidget = nullptr;
	Widget* m_player1ExpansionTextWidget = nullptr;
	WidgetIncrementer* m_player1ExpansionChangerWidget = nullptr;
	Widget* m_player1SimulationTextWidget = nullptr;
	WidgetIncrementer* m_player1SimulationChangerWidget = nullptr;
	Widget* m_player1UseChaosChanceWidget = nullptr;
	WidgetIncrementerFloat* m_player1ChaosChanceChangerWidget = nullptr;

	//player 2 AI
	Widget* m_player2MCTSAIParametersWidget = nullptr;
	Widget* m_player2SelectionTextWidget = nullptr;
	Widget* m_player2UCTScoreChangerWidget = nullptr;
	Widget* m_player2ExpansionTextWidget = nullptr;
	Widget* m_player2ExpansionChangerWidget = nullptr;
	Widget* m_player2SimulationTextWidget = nullptr;
	Widget* m_player2SimulationChangerWidget = nullptr;
	Widget* m_player2UseChaosChanceWidget = nullptr;
	Widget* m_player2ChaosChanceChangerWidget = nullptr;


	//Widget data
	Widget* m_toggleCurrentViewedPlayer = nullptr;
	IntVec2 m_playAreaGridDimensions = IntVec2( 1, 12 );
	Widget* m_baseCardWidget = nullptr;
	IntVec2 m_playerScoreGridDimensions = IntVec2( 1, 3 );
	WidgetGrid* m_playerScoreWidget = nullptr;
	Widget* m_player1ScoreWidget = nullptr;
	Widget* m_player2ScoreWidget = nullptr;
	//Player 1
	Widget* m_player1DeckWidget = nullptr;
	Widget* m_player1DiscardWidget = nullptr;
	Widget* m_player1HandWidget = nullptr;
	WidgetGrid* m_player1PlayAreaWidget = nullptr;

	
	//Piles
	IntVec2 m_cardPileDimensions = IntVec2( 8, 2 );
	WidgetGrid* m_cardPilesWidget = nullptr;
	Widget* m_playerNextPhaseWidget = nullptr;
	
	//Player 2
	Widget* m_player2DeckWidget = nullptr;
	Widget* m_player2DiscardWidget = nullptr;
	Widget* m_player2HandWidget = nullptr;

	bool m_isUIDirty = false;
	int m_playerWidgetsToShow = PLAYER_1;
private:
	void RenderDevConsole();
	AIStrategy m_player1Strategy = AIStrategy::MCTS;
	SIMMETHOD m_player1MCTSSimMethod = SIMMETHOD::RANDOM; 
	ROLLOUTMETHOD m_player1MCTSRolloutMethod = ROLLOUTMETHOD::EPSILONHEURISTIC;
	EXPANSIONSTRATEGY m_player1ExpansionStrategy = EXPANSIONSTRATEGY::HEURISTICS;
	float m_player1MCTSExplorationParameter = 0.5f;
	float m_player1MCTSChaosChance = 0.15f;

	AIStrategy m_player2Strategy = AIStrategy::SINGLEWITCH;
	SIMMETHOD m_player2MCTSSimMethod = SIMMETHOD::RANDOM;
	ROLLOUTMETHOD m_player2MCTSRolloutMethod = ROLLOUTMETHOD::EPSILONHEURISTIC;
	EXPANSIONSTRATEGY m_player2ExpansionStrategy = EXPANSIONSTRATEGY::HEURISTICS;
	float m_player2MCTSExplorationParameter = 0.5f;
	float m_player2MCTSChaosChance = 0.15f;
};
