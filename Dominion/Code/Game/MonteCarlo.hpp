#pragma once
//#include "Game/TreeNode.hpp"
#include "Game/TreeMapNode.hpp"
#include "Game/Game.hpp"

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

class MonteCarlo
{
	friend class SimulationJob;
public:
	MonteCarlo() = default;
	~MonteCarlo();

	void Startup(  gamestate_t const& newGameState  );
	void Shutdown();

	//Base MCTS methods
	void RunSimulations( int numberOfSimulations );
	inputMove_t GetBestMove();
	void UpdateGame( inputMove_t const& movePlayed, gamestate_t const& newGameState );

protected:
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
};



