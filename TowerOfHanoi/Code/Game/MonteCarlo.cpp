#include "Game/MonteCarlo.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"

int g_maxDepth = 30;

void MonteCarlo::SetGameState( gameState_t const& gameState )
{
// 	m_currentGameState = gameState;
// 	m_currentPotentialMoves = g_theGame->GetValidMovesAtGameState( m_currentGameState );
// 	m_currentBestMove = m_currentPotentialMoves[0];
// 	m_potentialMoveMetaData.clear();
// 	m_potentialMoveMetaData.resize( m_currentPotentialMoves.size() );


	if( m_headNode )
	{
		delete m_headNode;
		m_headNode = nullptr;
	}

	m_headNode = new mctsTreeNode_t( nullptr, gameState );
	m_currentHeadNode = m_headNode;
}

void MonteCarlo::SetMaxDepth( int maxDepth )
{
	m_maxDepth = maxDepth;
	g_maxDepth = m_maxDepth;
}

void MonteCarlo::RunSimulations( int numberOfSimulations )
{
// 	if( m_currentPotentialMoves.size() == 1 )
// 	{
// 		m_currentBestMove = m_currentPotentialMoves[0];
// 		return;
// 	}

	for( int simIndex = 0; simIndex < numberOfSimulations; simIndex++ )
	{
		//OLD WAY
// 		//check which move to sim at
// 		int moveToSimIndex = GetMoveToSimIndex();
// 		RunSimulationOnMove( moveToSimIndex );
// 		//gameState_t gameS
// 		//sim

		//Select Node
		mctsTreeNode_t* selectedNode = m_currentHeadNode->GetBestNodeToSelect( g_maxDepth );

		if( nullptr == selectedNode )
		{
			break; //There are no more nodes to check
		}
		//Expand Node
		mctsTreeNode_t* expandedNode = selectedNode->ExpandNode();
		//Simulate
		bool result = RunSimulationOnNode( expandedNode );
		//BackPropagate
		expandedNode->BackPropagateResult( result );
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

bool MonteCarlo::RunSimulationOnNode( mctsTreeNode_t* node )
{
	gameState_t currentGameState = node->GetGameState();
	int currentDepth = node->GetDepth();
	
	while( currentDepth < m_maxDepth )
	{

		std::vector<inputMove_t> potentialMoves = g_theGame->GetValidMovesAtGameState( currentGameState );
		int maxMoves = (int)potentialMoves.size() - 1;
		int moveIndexChoice = g_theGame->m_rand.RollRandomIntInRange( 0, maxMoves );
		inputMove_t const& moveToMake = potentialMoves[moveIndexChoice];

		g_theGame->UpdateGameStateIfValid( moveToMake, currentGameState );

		if( g_theGame->IsGameStateWon( currentGameState ) )
		{
			return true;
		}

		currentDepth++;
	}

	return false;
}

void MonteCarlo::UpdateBestMove()
{
// 	int currentBestIndex = 0;
// 	float currentMaxWinRatio = 0.f;
// 
// 	for( size_t moveIndex = 0; moveIndex < m_potentialMoveMetaData.size(); moveIndex++ )
// 	{
// 		MoveMetaData const& moveData = m_potentialMoveMetaData[moveIndex];
// 
// 		if( moveData.m_doesMoveWin )
// 		{
// 			float moveWinRatio = (float)moveData.m_numberOfWins/(float)moveData.m_numOfGames;
// 			if( moveWinRatio > currentMaxWinRatio )
// 			{
// 				currentMaxWinRatio = moveWinRatio;
// 				currentBestIndex = (int)moveIndex;
// 			}
// 		}
// 	}
// 
// 	m_currentBestMove = m_currentPotentialMoves[currentBestIndex];

	m_currentBestMove = m_currentHeadNode->GetBestInput();

	return;

	
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

void MonteCarlo::UpdateHeadNode( inputMove_t const& input )
{
	mctsTreeNode_t* newHeadNode = m_currentHeadNode->GetOrCreateChildFromInput( input );

	GUARANTEE_OR_DIE( newHeadNode, "new head node doesn't exist" );

	if( newHeadNode )
	{
		m_currentHeadNode->m_isCurrentHead = false;
		m_currentHeadNode = newHeadNode;
		m_currentHeadNode->m_isCurrentHead = true;

	}


}

int MonteCarlo::GetNumberOfWinsAtHead()
{
	return m_currentHeadNode->GetNumberOfWins();
}

int MonteCarlo::GetNumberOfSimulationsAtHead()
{
	return m_currentHeadNode->GetNumberOfSimulations();
}

mctsTreeNode_t::mctsTreeNode_t()
{
	m_isCurrentHead = true;
}

mctsTreeNode_t::mctsTreeNode_t( mctsTreeNode_t* parentNode, gameState_t const& gameState )
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

mctsTreeNode_t::mctsTreeNode_t( mctsTreeNode_t* parentNode, gameState_t const& gameState, inputMove_t const& inputToReachGameState )
{
	m_parentNode = parentNode;
	m_currentGameState = gameState;
	m_inputToReachGameState = inputToReachGameState;

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

int mctsTreeNode_t::GetDepth()
{
	int currentDepth = 0;

	mctsTreeNode_t* currentNode = m_parentNode;
	while( currentNode )
	{
		currentDepth++;
		currentNode = currentNode->m_parentNode;
	}

	return currentDepth;
}

int mctsTreeNode_t::GetMinimumDepthFromCurrentGameState()
{
	bool isGameStateWon = g_theGame->IsGameStateWon( m_currentGameState );

	if( isGameStateWon )
	{
		return GetDepth();
	}
	else
	{
		int bestChildDepth = 99999;
		for( size_t childIndex = 0; childIndex < m_childNodes.size(); childIndex++ )
		{
			int currentChildDepth = m_childNodes[childIndex]->GetMinimumDepthFromCurrentGameState();

			if( currentChildDepth < bestChildDepth )
			{
				bestChildDepth = currentChildDepth;
			}
		}

		return bestChildDepth;
	}
}

float mctsTreeNode_t::GetUCBValueAtNode( float explorationParameter /*= SQRT_2 */ )
{
	if( GetDepth() > g_maxDepth )
	{
		return -1;
	}

	float numberOfSimulations = (float)GetNumberOfSimulations();
	float numberOfSimulationsAtParent = (float)GetNumberOfSimulationsAtParent();
	float numberOfWins = (float)GetNumberOfWins();

	if( numberOfSimulations == 0 )
	{
		return 0;
	}

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
	std::vector<inputMove_t> validMoves = g_theGame->GetValidMovesAtGameState( m_currentGameState );
	
	for( size_t validMoveIndex = 0; validMoveIndex < validMoves.size(); validMoveIndex++ )
	{
		inputMove_t const& currentValidMove = validMoves[validMoveIndex];
		bool canExpand = true;

		for( size_t childIndex = 0; childIndex < m_childNodes.size(); childIndex++ )
		{
			inputMove_t const& childInput = m_childNodes[childIndex]->GetInputToReachNode();

			if( currentValidMove == childInput )
			{
				canExpand = false;
				break;
			}
		}


		if( canExpand )
		{
			gameState_t newGameState = g_theGame->GetGameStateFromInput( currentValidMove, m_currentGameState );
			mctsTreeNode_t* node = new mctsTreeNode_t( this, newGameState, currentValidMove );

			m_childNodes.push_back( node );
			return node;
		}

	}


	return nullptr;
}

mctsTreeNode_t* mctsTreeNode_t::GetBestNodeToSelect( int depthToReach )
{
	if( g_theGame->IsGameStateWon( m_currentGameState ) )
	{
		return nullptr;
	}

	if( GetDepth() >= depthToReach - 1 )
	{
		return nullptr;
	}

	float currentBestUCBValue = -1;
	mctsTreeNode_t* currentBestNode = nullptr;


	for( size_t childNodeIndex = 0; childNodeIndex < m_childNodes.size(); childNodeIndex++ )
	{
		mctsTreeNode_t* childNodeBestUCB = m_childNodes[childNodeIndex]->GetBestNodeToSelect( depthToReach );

		if( nullptr == childNodeBestUCB )
		{
			continue;
		}

		if( !childNodeBestUCB->CanExpand() )
		{
			continue;
		}

		float childNodeUCBValue = childNodeBestUCB->GetUCBValueAtNode();

		if( childNodeUCBValue > currentBestUCBValue )
		{
			currentBestUCBValue = childNodeUCBValue;
			currentBestNode = childNodeBestUCB;
		}
	}

	if( CanExpand() )
	{
		float ucbAtNode = GetUCBValueAtNode();
		ucbAtNode = Max( ucbAtNode, 0.000001f );

		if( ucbAtNode > currentBestUCBValue )
		{
			currentBestUCBValue = ucbAtNode;
			currentBestNode = this;
		}
	}

	if( currentBestUCBValue == -1 )
	{
		currentBestNode = this;
	}
	return currentBestNode;
}

mctsTreeNode_t* mctsTreeNode_t::GetOrCreateChildFromInput( inputMove_t const& input )
{
	for( size_t childIndex = 0; childIndex < m_childNodes.size(); childIndex++ )
	{
		inputMove_t const& childInput = m_childNodes[childIndex]->m_inputToReachGameState;
		if( input == childInput )
		{
			return m_childNodes[childIndex];
		}
	}
	
	gameState_t newChildGameState = g_theGame->GetGameStateFromInput( input, m_currentGameState );
	
	mctsTreeNode_t* newChildNode = new mctsTreeNode_t( this, newChildGameState, input );
	m_childNodes.push_back( newChildNode );
	return newChildNode;
}

inputMove_t const& mctsTreeNode_t::GetBestInput()
{
	int currentMinimumDepth = 999999;
	size_t currentBestIndex = 0;

	for( size_t childIndex = 0; childIndex < m_childNodes.size(); childIndex++ )
	{
		int childMinimumDepth = m_childNodes[childIndex]->GetMinimumDepthFromCurrentGameState();
		if( childMinimumDepth < currentMinimumDepth )
		{
			currentMinimumDepth = childMinimumDepth;
			currentBestIndex = childIndex;
		}
	}

	return m_childNodes[currentBestIndex]->GetInputToReachNode();
}

bool inputMove_t::operator==( inputMove_t const& compare ) const
{
	if( compare.m_columnToPop == m_columnToPop &&
		compare.m_columnToPush == m_columnToPush )
	{
		return true;
	}
	else
	{
		return false;
	}
}

inputMove_t::inputMove_t( inputMove_t const& copyFrom )
{
	m_columnToPop = copyFrom.m_columnToPop;
	m_columnToPush = copyFrom.m_columnToPush;
}

void inputMove_t::operator=( inputMove_t const& copyFrom )
{
	m_columnToPop = copyFrom.m_columnToPop;
	m_columnToPush = copyFrom.m_columnToPush;
}
