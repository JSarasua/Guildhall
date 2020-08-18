#pragma once
#include <vector>
#include <deque>

struct gameState_t
{
	std::deque<int> columns[3];
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

};