#include "Game/MonteCarlo.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

void MonteCarlo::SetGameState( gameState_t const& gameState )
{
	m_currentGameState = gameState;
	m_currentPotentialMoves = g_theGame->GetValidMovesAtGameState( m_currentGameState );
	m_currentBestMove = m_currentPotentialMoves[0];
	m_potentialMoveMetaData.clear();
	m_potentialMoveMetaData.resize( m_currentPotentialMoves.size() );
}

void MonteCarlo::SetMaxDepth( int maxDepth )
{
	m_maxDepth = maxDepth;
}

void MonteCarlo::RunSimulations( int numberOfSimulations )
{
	if( m_currentPotentialMoves.size() == 1 )
	{
		m_currentBestMove = m_currentPotentialMoves[0];
		return;
	}

	for( int simIndex = 0; simIndex < numberOfSimulations; simIndex++ )
	{
		//check which move to sim at
		int moveToSimIndex = GetMoveToSimIndex();
		RunSimulationOnMove( moveToSimIndex );
		//gameState_t gameS
		//sim
	}
}

void MonteCarlo::RunSimulationOnMove( int moveIndex )
{
	inputMove_t inputMove = m_currentPotentialMoves[moveIndex];
	gameState_t gameState = m_currentGameState;
	g_theGame->UpdateGameStateIfValid( inputMove, gameState );

	MoveMetaData& moveMetaData = m_potentialMoveMetaData[moveIndex];

	if( g_theGame->IsGameStateWon( gameState ) )
	{
		moveMetaData.m_doesMoveWin = true;
		moveMetaData.m_numberOfWins++;
		moveMetaData.m_numOfGames++;
		
		return;
	}
	
	int currentDepth = 0;
	while( currentDepth < m_maxDepth )
	{
		std::vector<inputMove_t> potentialMoves = g_theGame->GetValidMovesAtGameState( gameState );
		int maxMoves = (int)potentialMoves.size() - 1;
		int moveIndexChoice = g_theGame->m_rand.RollRandomIntInRange(0, maxMoves);
		inputMove_t& moveToMake = potentialMoves[moveIndexChoice];

		g_theGame->UpdateGameStateIfValid( moveToMake, gameState );

		if( g_theGame->IsGameStateWon( gameState ) )
		{
			moveMetaData.m_doesMoveWin = true;
			moveMetaData.m_numberOfWins++;
			moveMetaData.m_numOfGames++;

			return;
		}

		currentDepth++;
	}

	moveMetaData.m_numOfGames++;
	return;
}

void MonteCarlo::UpdateBestMove()
{
	int currentBestIndex = 0;
	float currentMaxWinRatio = 0.f;

	for( size_t moveIndex = 0; moveIndex < m_potentialMoveMetaData.size(); moveIndex++ )
	{
		MoveMetaData const& moveData = m_potentialMoveMetaData[moveIndex];

		if( moveData.m_doesMoveWin )
		{
			float moveWinRatio = (float)moveData.m_numberOfWins/(float)moveData.m_numOfGames;
			if( moveWinRatio > currentMaxWinRatio )
			{
				currentMaxWinRatio = moveWinRatio;
				currentBestIndex = (int)moveIndex;
			}
		}
	}

	m_currentBestMove = m_currentPotentialMoves[currentBestIndex];
}

inputMove_t MonteCarlo::GetBestMove()
{
	return m_currentBestMove;
}

int MonteCarlo::GetMoveToSimIndex()
{
	int currentBestMoveToSimIndex = 0;
	int currentMinMoves = 99999999;
	for( int moveMetaIndex = 0; moveMetaIndex < m_potentialMoveMetaData.size(); moveMetaIndex++ )
	{
		//inputMove_t& inputMove = m_currentPotentialMoves[moveMetaIndex];
		MoveMetaData& inputMeta = m_potentialMoveMetaData[moveMetaIndex];
		int numOfGames = inputMeta.m_numOfGames;
		//int numOfWins = inputMeta.m_numOfGames;

		if( numOfGames == 0 )
		{
			return moveMetaIndex;
		}
		else
		{
			if( numOfGames < currentMinMoves )
			{
				currentMinMoves = numOfGames;
				currentBestMoveToSimIndex = moveMetaIndex;
			}
		}
	}

	return currentBestMoveToSimIndex;
}

