#pragma once
//#include "Game/TreeNode.hpp"
#include "Game/TreeMapNode.hpp"
#include "Game/Game.hpp"
#include "Engine/Queues/SynchronizedLockFreeQueue.hpp"

class JobSystem;

struct ucbResult_t
{
	ucbResult_t() = default;
	ucbResult_t( float ucb, TreeMapNode* newNode ) { ucbValue = ucb; node = newNode; }
	float ucbValue = 0.f;
	TreeMapNode* node = nullptr;
};

struct expand_t
{
	inputMove_t m_input;
	gamestate_t gameState;
	TreeMapNode* nodeToExpand = nullptr;
	TreeMapNode* nodeToSelect = nullptr; //Only used if repeating a node
};

struct bestNode_t
{
	float nodeWinRate = 0.f;
	TreeMapNode const* node = nullptr;
};

struct sim_t
{
	TreeMapNode* nodeToSim = nullptr;
	int result = -1;
};

struct playerWinRate_t
{
	float winRate = 0.f;
	int whoseMove = -1;
};

enum class SCORESTRATEGY
{
	WINPERCENTAGE,
	MCDOM
};

class MonteCarlo
{
	friend class SimulationJob;
public:
	MonteCarlo() = default;
	~MonteCarlo();

	void RestoreFromData();

	void Startup();
	void Shutdown();
	void SetInitialGameState( gamestate_t const& newGameState );

	void FlushJobSystemQueues();

	//Base MCTS methods
	inputMove_t GetBestMove();
	void UpdateGame( inputMove_t const& movePlayed, gamestate_t const& newGameState );
	void SetExplorationParameter( float explorationParameter ) { m_ucbValue = explorationParameter; }
	void AddSimulations( int simulationsToAdd );

	int GetNumberOfSimulationsRun() const { return m_totalNumberOfSimulationsRun; }
	int GetCurrentNumberOfSimulationsLeft() const { return m_numberOfSimulationsToRun; }

	//Setting methods
	void SetExpansionStrategy( EXPANSIONSTRATEGY expansionStrategy );
	void SetRolloutMethod( ROLLOUTMETHOD rolloutMethod );
	void SetUCBValue( float ucbValue );
	void SetEpsilonValueZeroToOne( float epsilonValue );
	void SetSimMethod( SIMMETHOD simMethod );

	void StopThreads();
	void StartThreads();
	void SaveTree();
	void LoadTree();


protected:
	//Thread
	void WorkerMain();
	
	void RunSimulations( int numberOfSimulations );
	void UpdateBestMove();
	bool UpdateGameIfChanged();
	expand_t GetBestNodeToSelect( TreeMapNode* currentNode ); //Returns null if all nodes have been explored
	TreeMapNode* ExpandNode( expand_t expandData ); //Returns null if can't expand
	TreeMapNode* ExpandNodeUsingAllMoves( expand_t expandData );
	TreeMapNode* ExpandNodeUsingHeuristics( expand_t expandData );
	int RunSimulationOnNode( TreeMapNode* node ); //Returns result of simulation
	void BackPropagateResult( int whoWon, TreeMapNode* node );

	//Methods of Getting the "Best Move"
	inputMove_t GetMostPlayedMove( TreeMapNode* currentNode ); 
	inputMove_t GetBestWinRateMove( TreeMapNode* currentNode ); //Best average winrate
	inputMove_t GetBestMoveToDepth( int depth, TreeMapNode* currentNode );
	playerWinRate_t GetBestWinRateAtDepth( int depth, TreeMapNode const* node );
	int GetWhoseMoveAtDepth( int depth, TreeMapNode const* node );

	inputMove_t GetMoveUsingCurrentRolloutMethod( gamestate_t const& gameState );
	inputMove_t GetMoveForSimsUsingHeuristic( gamestate_t const& gameState );
	inputMove_t GetMoveForSimsUsingEpsilonHeuristic( gamestate_t const& gameState );

	//Heuristic Expansion strategy
	std::vector<inputMove_t> GetMovesUsingAllHeuristics( gamestate_t const& gameState );

	//Helper Methods
	float GetAverageUCBValue( std::vector<TreeMapNode*> const& nodes, float explorationParameter = SQRT_2 );
	float GetUCBValueAtNode( TreeMapNode const* node, float explorationParameter = SQRT_2 );
	bool CanExpand( TreeMapNode const* node );
	bool CanExpandUsingMoves( TreeMapNode const* node );
	bool CanExpandUsingHeuristic( TreeMapNode const* node );

	TreeMapNode const* GetCurrentHeadNode();



public:

	TreeMapNode* m_headNode = nullptr;
	TreeMapNode* m_currentHeadNode = nullptr;
	//int m_player = CIRCLEPLAYER;

	JobSystem* m_mcJobSystem = nullptr;
	std::thread* m_mainThread = nullptr;

	double m_totalTime = 0;
	double m_selectTime = 0;
	double m_expandTime = 0;
	double m_simTime = 0;
	double m_backpropagationTime = 0;

	std::mutex m_ucbLock;
	float m_ucbValue = 0.5f;

	std::mutex m_epsilonLock;
	float m_epsilon = 0.15f;

	//lock
	std::mutex m_gameStateChangeLock;
	bool m_didGameStateChange = false;
	inputMove_t m_moveToMake = inputMove_t();
	gamestate_t m_newGameState = gamestate_t();

	//lock
	std::mutex m_bestMoveLock;
	inputMove_t m_bestMove = inputMove_t();

	std::mutex m_simMethodLock;
	SIMMETHOD m_simMethod = SIMMETHOD::RANDOM;

	std::mutex m_scoreStrategyLock;
	SCORESTRATEGY m_scoreStrategy = SCORESTRATEGY::MCDOM;

	std::mutex m_expansionStrategyLock;
	EXPANSIONSTRATEGY m_expansionStrategy = EXPANSIONSTRATEGY::HEURISTICS;

	std::mutex m_rolloutStrategyLock;
	ROLLOUTMETHOD m_rolloutMethod = ROLLOUTMETHOD::EPSILONHEURISTIC;

	std::atomic<bool> m_isQuitting = false;
	std::atomic<int> m_totalNumberOfSimulationsRun = 0;
	std::atomic<int> m_numberOfSimulationsToRun = 0;
};



