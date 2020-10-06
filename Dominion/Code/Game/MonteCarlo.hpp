#pragma once
//#include "Game/TreeNode.hpp"
#include "Game/TreeMapNode.hpp"
#include "Game/Game.hpp"

constexpr float SQRT_2 = 1.41421356237f;

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
public:
	MonteCarlo() = default;
	~MonteCarlo();

	void Startup(  gamestate_t const& newGameState  );
	void Shutdown();

	//Base MCTS methods
	void RunSimulations( int numberOfSimulations );
	int RunSimulationOnNode( TreeMapNode* node ); //Returns result of simulation
	//TreeMapNode* GetBestNodeToSelectAndExpand( TreeMapNode* currentNode );
	expand_t GetBestNodeToSelect( TreeMapNode* currentNode ); //Returns null if all nodes have been explored
	TreeMapNode* ExpandNode( expand_t expandData ); //Returns null if can't expand
	void BackPropagateResult( int whoWon, TreeMapNode* node );
	//inputMove_t GetBestInputChoiceFromChildren( TreeMapNode* node );
	float GetAverageUCBValue( std::vector<TreeMapNode*> const& nodes, float explorationParameter = SQRT_2 );
	//Helper Methods
	float GetUCBValueAtNode( TreeMapNode const* node, float explorationParameter = SQRT_2 );
	bool CanExpand( TreeMapNode const* node );
	inputMove_t GetBestMove();
	//bestNode_t GetHighestWinRateChildNode( TreeMapNode const* node );

	void UpdateGame( inputMove_t const& movePlayed, gamestate_t const& newGameState );
	//void ResetGame();
	TreeMapNode const* GetCurrentHeadNode();
public:
// 	TreeNode* m_headNode = nullptr;
// 	TreeNode* m_currentHeadNode = nullptr;

	TreeMapNode* m_headNode = nullptr;
	TreeMapNode* m_currentHeadNode = nullptr;
	//int m_player = CIRCLEPLAYER;
};



