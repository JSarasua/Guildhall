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


struct inputMove_t
{
public:
	inputMove_t() = default;
	inputMove_t( inputMove_t const& copyFrom );

	bool	operator==( inputMove_t const& compare ) const;
	void	operator=( inputMove_t const& copyFrom );

public:
	int m_columnToPop = 0;
	int m_columnToPush = 0;
};


struct mctsTreeNode_t
{
public:
	mctsTreeNode_t();
	mctsTreeNode_t( mctsTreeNode_t* parentNode, gameState_t const& gameState );
	mctsTreeNode_t( mctsTreeNode_t* parentNode, gameState_t const& gameState, inputMove_t const& inputToReachGameState );
	~mctsTreeNode_t();

	int GetNumberOfSimulationsAtParent();
	int GetNumberOfSimulations();
	int GetNumberOfWins();
	int GetDepth();
	int GetMinimumDepthFromCurrentGameState();
	float GetUCBValueAtNode( float explorationParameter = SQRT_2 );
	void BackPropagateResult( bool didWin );
	bool CanExpand();
	mctsTreeNode_t* ExpandNode();
	mctsTreeNode_t* GetBestNodeToSelect();
	mctsTreeNode_t* GetOrCreateChildFromInput( inputMove_t const& input );
	inputMove_t const& GetInputToReachNode() { return m_inputToReachGameState; }
	gameState_t GetGameState() { return m_currentGameState; }
	inputMove_t const& GetBestInput();

public:
	nodeMetaData_t m_nodeMetaData;
	gameState_t m_currentGameState;
	inputMove_t m_inputToReachGameState;

	mctsTreeNode_t* m_parentNode = nullptr;
	std::vector<mctsTreeNode_t*> m_childNodes;

	bool m_isCurrentHead = true;
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
	bool RunSimulationOnNode( mctsTreeNode_t* node );
	void UpdateBestMove();
	inputMove_t GetBestMove();
	int GetMoveToSimIndex();
	void UpdateHeadNode( inputMove_t const& input );


public:
	int m_maxDepth = 100;
	gameState_t m_currentGameState;
	inputMove_t m_currentBestMove;
	std::vector<inputMove_t> m_currentPotentialMoves;
	std::vector<MoveMetaData> m_potentialMoveMetaData;


	mctsTreeNode_t* m_headNode = nullptr;
	mctsTreeNode_t* m_currentHeadNode = nullptr;
};