#pragma once
#include "Game/TreeNode.hpp"

constexpr float SQRT_2 = 1.41421356237f;

struct ucbResult_t
{
	float ucbValue = 0.f;
	TreeNode* node = nullptr;
};

class MonteCarlo
{
public:
	MonteCarlo() = default;
	~MonteCarlo();

	//Base MCTS methods
	void RunSimulations( int numberOfSimulations );
	bool RunSimulationOnNode( TreeNode* node ); //Returns result of simulation
	ucbResult_t GetBestNodeToSelect( TreeNode* currentNode ); //Returns null if all nodes have been explored
	TreeNode* ExpandNode( TreeNode* nodeToExpand ); //Returns null if can't expand
	void BackPropagateResult( bool didWin );
	
	//Helper Methods
	float GetUCBValueAtNode( TreeNode* node, float explorationParameter = SQRT_2 );
	bool CanExpand( TreeNode const* node );


public:
	TreeNode* m_headNode = nullptr;
	TreeNode* m_currentHeadNode = nullptr;
	bool m_doIStart = false;
};