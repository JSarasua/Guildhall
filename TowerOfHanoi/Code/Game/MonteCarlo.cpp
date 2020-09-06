#include "Game/MonteCarlo.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"

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

mctsTreeNode_t::mctsTreeNode_t()
{
	m_isCurrentHead = true;
}

mctsTreeNode_t::mctsTreeNode_t( mctsTreeNode_t* parentNode, gameState_t gameState )
{
	m_parentNode = parentNode;
	m_currentGameState = gameState;

	if( m_parentNode )
	{
		m_isCurrentHead = false;
	}
	else
	{
		m_isCurrentHead = true;
	}
}

mctsTreeNode_t::~mctsTreeNode_t()
{
	for( size_t childNodeIndex = 0; childNodeIndex < m_childNodes.size(); childNodeIndex++ )
	{
		delete m_childNodes[childNodeIndex];
	}
}

int mctsTreeNode_t::GetNumberOfSimulationsAtParent()
{
	if( nullptr != m_parentNode )
	{
		return m_parentNode->GetNumberOfSimulations();
	}
	else
	{
		//Not sure if this is correct
		return GetNumberOfSimulations();
	}
}

int mctsTreeNode_t::GetNumberOfSimulations()
{
	return m_nodeMetaData.m_numberOfSimulationsAtNode;
}

int mctsTreeNode_t::GetNumberOfWins()
{
	return m_nodeMetaData.m_numberOfWinsAtNode;
}

float mctsTreeNode_t::GetUCBValueAtNode( float explorationParameter /*= SQRT_2 */ )
{
	float numberOfSimulations = (float)GetNumberOfSimulations();
	float numberOfSimulationsAtParent = (float)GetNumberOfSimulationsAtParent();
	float numberOfWins = (float)GetNumberOfWins();


	float ucb = numberOfWins/numberOfSimulations + explorationParameter * SquareRootFloat( NaturalLog( numberOfSimulationsAtParent ) / numberOfSimulations );

	return ucb;
}

void mctsTreeNode_t::BackPropagateResult( bool didWin )
{
	m_nodeMetaData.m_numberOfWinsAtNode += (int)didWin;
	m_nodeMetaData.m_numberOfSimulationsAtNode++;

	if( m_parentNode && !m_isCurrentHead )
	{
		m_parentNode->BackPropagateResult( didWin );
	}
}

bool mctsTreeNode_t::CanExpand()
{
	int numberOfChildren = (int)m_childNodes.size();
	int numberOfValidMoves = g_theGame->GetNumberOfValidMovesAtGameState( m_currentGameState );

	if( numberOfChildren < numberOfValidMoves )
	{
		return true;
	}
	else
	{
		return false;
	}
}

mctsTreeNode_t* mctsTreeNode_t::ExpandNode()
{
	//GetValidChildNode

	return nullptr;
}

mctsTreeNode_t* mctsTreeNode_t::GetBestNodeToSelect()
{
	float currentBestUCBValue = GetUCBValueAtNode();
	mctsTreeNode_t* currentBestNode = this;


	for( size_t childNodeIndex = 0; childNodeIndex < m_childNodes.size(); childNodeIndex++ )
	{
		mctsTreeNode_t* childNodeBestUCB = m_childNodes[childNodeIndex]->GetBestNodeToSelect();
		float childNodeUCBValue = childNodeBestUCB->GetUCBValueAtNode();

		if( childNodeUCBValue > currentBestUCBValue )
		{
			currentBestUCBValue = childNodeUCBValue;
			currentBestNode = childNodeBestUCB;
		}
	}

	return currentBestNode;
}
