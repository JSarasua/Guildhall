#include "Game/MonteCarloNoTree.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

void MonteCarloNoTree::ResetPossibleMoves()
{
	m_currentPossibleMoves.clear();
	std::vector<inputMove_t> possibleMoves = g_theGame->GetValidMovesAtGameState( m_currentGameState );
	m_currentPossibleMoves.reserve( possibleMoves.size() );

	for( size_t moveIndex = 0; moveIndex < possibleMoves.size(); moveIndex++ )
	{
		moveMetaData_t metaData;
		metaData.m_move = possibleMoves[moveIndex];

		m_currentPossibleMoves.push_back( metaData );
	}
}

void MonteCarloNoTree::UpdateBestMove()
{
	if( m_currentPossibleMoves.size() == 1 )
	{
		m_currentBestMove = m_currentPossibleMoves[0].m_move;
		return;
	}

	float bestWinRate = -1.f;
	inputMove_t const* bestMove = nullptr;
	for( size_t moveDataIndex = 0; moveDataIndex < m_currentPossibleMoves.size(); moveDataIndex++ )
	{
		moveMetaData_t const& moveData = m_currentPossibleMoves[moveDataIndex];
		float winRate = moveData.m_numberOfWins / moveData.m_numberOfSimulations;

		if( winRate > bestWinRate )
		{
			bestWinRate = winRate;
			bestMove = &moveData.m_move;
		}
	}

	m_currentBestMove = *bestMove;
}

void MonteCarloNoTree::RunSimulations( int numberOfSimulations )
{
	if( g_theGame->IsGameOverForGameState( m_currentGameState ) )
	{
		return;
	}

	if( m_currentPossibleMoves.size() == 1 )
	{
		UpdateBestMove();
		return;
	}


	int currentSimulation = 0;
	while ( currentSimulation < numberOfSimulations )
	{
		moveMetaData_t& moveData = GetBestMoveToSelect();
		inputMove_t const& moveToMake = moveData.m_move;
		int whoseMove = m_currentGameState.m_whoseMoveIsIt;

		int result = RunSimulationOnMove( moveToMake );


		if( result == TIE )
		{
			moveData.m_numberOfWins += 0.5f;

		}
		else if( result == whoseMove )
		{
			moveData.m_numberOfWins += 1.f;
		}
		moveData.m_numberOfSimulations += 1.f;

		currentSimulation++;
		m_totalNumberOfSimulations++;
	}

	UpdateBestMove();
}

int MonteCarloNoTree::RunSimulationOnMove( inputMove_t const& move )
{
	gamestate_t currentGameState = g_theGame->GetGameStateAfterMove( m_currentGameState, move );

	int gameResultAfterMove = g_theGame->IsGameOverForGameState( currentGameState );


	while( gameResultAfterMove == 0 )
	{
		inputMove_t currentMove = g_theGame->GetRandomMoveAtGameState( currentGameState );
		currentGameState = g_theGame->GetGameStateAfterMove( currentGameState, currentMove );

		gameResultAfterMove = g_theGame->IsGameOverForGameState( currentGameState );
	}

	return gameResultAfterMove;
}

moveMetaData_t& MonteCarloNoTree::GetBestMoveToSelect()
{
	float bestUCBValue = -1.f;
	moveMetaData_t* bestMove = nullptr;

	for( size_t moveDataIndex = 0; moveDataIndex < m_currentPossibleMoves.size(); moveDataIndex++ )
	{
		float currentUCBValue = GetUCBValueFromMoveData( m_currentPossibleMoves[moveDataIndex] );
		if( currentUCBValue > bestUCBValue )
		{
			bestUCBValue = currentUCBValue;
			bestMove = &m_currentPossibleMoves[moveDataIndex];
		}
	}

	return *bestMove;
}

float MonteCarloNoTree::GetUCBValueFromMoveData( moveMetaData_t const& moveData, float explorationParameter /*= SQRT_2 */ )
{
	float numberOfSimulations = (float)moveData.m_numberOfSimulations;
	float numberOfWins = (float)moveData.m_numberOfWins;

	if( numberOfSimulations == 0.f )
	{
		return explorationParameter;
	}

	float ucb = numberOfWins/numberOfSimulations + explorationParameter * SquareRootFloat( NaturalLog( m_totalNumberOfSimulations ) / numberOfSimulations );

	return ucb;
}

