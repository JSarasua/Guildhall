#pragma once
#include <vector>
#include <deque>

constexpr float SQRT_2 = 1.41421356237f;

struct gameState_t
{
	std::deque<int> columns[3];
};

struct nodeMetaData_t
{
	int m_numberOfWinsAtNode = 0;
	int m_numberOfSimulationsAtNode = 0;
};
struct mctsTreeNode_t
{
public:
	mctsTreeNode_t();
	mctsTreeNode_t( mctsTreeNode_t* parentNode, gameState_t gameState );
	~mctsTreeNode_t();

	int GetNumberOfSimulationsAtParent();
	int GetNumberOfSimulations();
	int GetNumberOfWins();
	float GetUCBValueAtNode( float explorationParameter = SQRT_2 );
	void BackPropagateResult( bool didWin );
	bool CanExpand();
	mctsTreeNode_t* ExpandNode();
	mctsTreeNode_t* GetBestNodeToSelect();


public:
	nodeMetaData_t m_nodeMetaData;
	gameState_t m_currentGameState;

	mctsTreeNode_t* m_parentNode = nullptr;
	std::vector<mctsTreeNode_t*> m_childNodes;

	bool m_isCurrentHead = true;
};



struct inputMove_t
{
	int m_columnToPop = 0;
	int m_columnToPush = 0;
};

struct MoveMetaData
{
	int m_numberOfWins = 0;
	int m_numOfGames = 0;

	bool m_doesMoveWin = false;
};

class MonteCarlo
{
public:
	MonteCarlo() {}
	~MonteCarlo() {}

	void SetGameState( gameState_t const& gameState );
	void SetMaxDepth( int maxDepth );
	void RunSimulations( int numberOfSimulations );
	void RunSimulationOnMove( int moveIndex );
	void UpdateBestMove();
	inputMove_t GetBestMove();
	int GetMoveToSimIndex();


public:
	int m_maxDepth = 100;
	gameState_t m_currentGameState;
	inputMove_t m_currentBestMove;
	std::vector<inputMove_t> m_currentPotentialMoves;
	std::vector<MoveMetaData> m_potentialMoveMetaData;


	mctsTreeNode_t* m_headNode = nullptr;
};