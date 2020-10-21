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

class MonteCarlo
{
	friend class SimulationJob;
public:
	MonteCarlo() = default;
	~MonteCarlo();

	void Startup();
	void Shutdown();
	void SetInitialGameState( gamestate_t const& newGameState );

	//Base MCTS methods
	inputMove_t GetBestMove();
	void UpdateGame( inputMove_t const& movePlayed, gamestate_t const& newGameState );
	void SetExplorationParameter( float explorationParameter ) { m_ucbValue = explorationParameter; }
	void AddSimulations( int simulationsToAdd );

	int GetNumberOfSimulationsRun() const { return m_totalNumberOfSimulationsRun; }
	int GetCurrentNumberOfSimulationsLeft() const { return m_numberOfSimulationsToRun; }

protected:
	//Thread
	void WorkerMain();
	
	void RunSimulations( int numberOfSimulations );
	void UpdateBestMove();
	bool UpdateGameIfChanged();
	expand_t GetBestNodeToSelect( TreeMapNode* currentNode ); //Returns null if all nodes have been explored
	TreeMapNode* ExpandNode( expand_t expandData ); //Returns null if can't expand
	int RunSimulationOnNode( TreeMapNode* node ); //Returns result of simulation
	void BackPropagateResult( int whoWon, TreeMapNode* node );

// 	inputMove_t GetBestMoveSafe();
// 	void UpdateGameSafe( inputMove_t const& movePlayed, gamestate_t const& newGameState );

	
	//Helper Methods
	float GetAverageUCBValue( std::vector<TreeMapNode*> const& nodes, float explorationParameter = SQRT_2 );
	float GetUCBValueAtNode( TreeMapNode const* node, float explorationParameter = SQRT_2 );
	bool CanExpand( TreeMapNode const* node );
	//bestNode_t GetHighestWinRateChildNode( TreeMapNode const* node );
	//inputMove_t GetBestInputChoiceFromChildren( TreeMapNode* node );

	//void ResetGame();
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

	float m_ucbValue = 50.f;


	//lock
	std::mutex m_gameStateChangeLock;
	bool m_didGameStateChange = false;
	inputMove_t m_moveToMake = inputMove_t();
	gamestate_t m_newGameState = gamestate_t();

	//lock
	std::mutex m_bestMoveLock;
	inputMove_t m_bestMove = inputMove_t();

	std::atomic<bool> m_isQuitting = false;
	std::atomic<int> m_totalNumberOfSimulationsRun = 0;
	std::atomic<int> m_numberOfSimulationsToRun = 0;
};



