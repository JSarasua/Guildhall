#pragma once
#include "Game/TreeNode.hpp"
#include "Game/Game.hpp"

constexpr float SQRT_2 = 1.41421356237f;

struct ucbResult_t
{
	ucbResult_t() = default;
	ucbResult_t( float ucb, TreeNode* newNode ) { ucbValue = ucb; node = newNode; }
	float ucbValue = 0.f;
	TreeNode* node = nullptr;
};

struct bestNode_t
{
	float nodeWinRate = 0.f;
	TreeNode const* node = nullptr;
};

class MonteCarlo
{
public:
	MonteCarlo() = default;
	~MonteCarlo();

	void Startup(  gamestate_t const& newGameState  );
	void Shutdown();

	//Base MCTS methods
	void RunSimulations( int numberOfSimulations );
	int RunSimulationOnNode( TreeNode* node ); //Returns result of simulation
	ucbResult_t GetBestNodeToSelect( TreeNode* currentNode ); //Returns null if all nodes have been explored
	TreeNode* ExpandNode( TreeNode* nodeToExpand ); //Returns null if can't expand
	void BackPropagateResult( int whoWon, TreeNode* node );
	
	//Helper Methods
	float GetUCBValueAtNode( TreeNode* node, float explorationParameter = SQRT_2 );
	bool CanExpand( TreeNode const* node );
	inputMove_t GetBestMove();
	bestNode_t GetHighestWinRateChildNode( TreeNode const* node );

	void UpdateGame( inputMove_t const& movePlayed, gamestate_t const& newGameState );
	//void ResetGame();
	TreeNode const* GetCurrentHeadNode();
public:
	TreeNode* m_headNode = nullptr;
	TreeNode* m_currentHeadNode = nullptr;
	//int m_player = CIRCLEPLAYER;
};