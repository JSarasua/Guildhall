#include "Game/MonteCarlo.hpp"
//#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"




MonteCarlo::~MonteCarlo()
{
	delete m_headNode;
	m_headNode = nullptr;
	m_currentHeadNode = nullptr;
}

void MonteCarlo::Startup( int whoseMoveIsIt )
{
	m_headNode = new TreeNode();
	m_headNode->m_data = new data_t();
	m_headNode->m_data->m_currentGamestate.whoseMoveIsIt = whoseMoveIsIt;

	m_currentHeadNode = m_headNode;
}

void MonteCarlo::Shutdown()
{
	delete m_headNode;
	m_currentHeadNode = nullptr;
}

void MonteCarlo::RunSimulations( int numberOfSimulations )
{
	for( int currentSimIndex = 0; currentSimIndex < numberOfSimulations; currentSimIndex++ )
	{
		ucbResult_t bestResult = GetBestNodeToSelect( m_currentHeadNode );
		TreeNode* bestNode = bestResult.node;
		if( !bestNode )
		{
			break;
		}
		TreeNode* expandedNode = ExpandNode( bestNode );
		if( !expandedNode )
		{
			break;
		}
		int whoWon = RunSimulationOnNode( expandedNode );
		BackPropagateResult( whoWon, expandedNode );
	}
}

int MonteCarlo::RunSimulationOnNode( TreeNode* node )
{
	gamestate_t currentGameState = node->m_data->m_currentGamestate;
	int isGameOver = g_theGame->IsGameOverForGameState( currentGameState );
	if( isGameOver != 0 )
	{
		return isGameOver;
	}

	inputMove_t move; 

	while( isGameOver == 0 )
	{
		move = g_theGame->GetRandomMoveAtGameState( currentGameState );
		currentGameState = g_theGame->GetGameStateAfterMove( currentGameState, move );
		isGameOver = g_theGame->IsGameOverForGameState( currentGameState );
	}

	return isGameOver;
}

ucbResult_t MonteCarlo::GetBestNodeToSelect( TreeNode* currentNode )
{
	float highestUCBValue = -10000.f;
	TreeNode* bestTreeNode = nullptr;

	//Return itself if it can be expanded
	if( CanExpand( currentNode ) )
	{
		highestUCBValue = GetUCBValueAtNode( currentNode );
		bestTreeNode = currentNode;
		return ucbResult_t( highestUCBValue, bestTreeNode );
	}

	std::vector<TreeNode*>& childNodes = currentNode->m_childNodes;
	size_t numChildNodes = currentNode->m_childNodes.size();
	for( size_t childIndex = 0; childIndex < numChildNodes; childIndex++ )
	{
		ucbResult_t result = GetBestNodeToSelect( childNodes[childIndex] );

		if( result.ucbValue > highestUCBValue )
		{
			highestUCBValue = result.ucbValue;
			bestTreeNode = result.node;
		}
	}

	ucbResult_t result;
	result.ucbValue = highestUCBValue;
	result.node = bestTreeNode;
	return result;
}

TreeNode* MonteCarlo::ExpandNode( TreeNode* nodeToExpand )
{
	if( !CanExpand( nodeToExpand ) )
	{
		return nullptr;
	}
	TreeNode* newChildNode = nullptr;

	gamestate_t const& currentGameState = nodeToExpand->m_data->m_currentGamestate;
	std::vector<int> validMoves = g_theGame->GetValidMovesAtGameState( currentGameState );
	std::vector<TreeNode*> const& childNodes = nodeToExpand->m_childNodes;
	for( size_t moveIndex = 0; moveIndex < validMoves.size(); moveIndex++ )
	{
		int currentMove = validMoves[moveIndex];
		bool canExpand = true;
		for( size_t childIndex = 0; childIndex < childNodes.size(); childIndex++ )
		{
			int moveAtChild = childNodes[childIndex]->m_data->m_moveToReachNode.m_move;

			if( currentMove == moveAtChild )
			{
				canExpand = false;
				break;
			}
		}

		if( canExpand )
		{
			newChildNode = new TreeNode();
			newChildNode->m_parentNode = nodeToExpand;
			inputMove_t newMove = inputMove_t( currentMove );
			gamestate_t childGameState = g_theGame->GetGameStateAfterMove( currentGameState, newMove );
			data_t* childData = new data_t( metaData_t(), newMove, childGameState );
			newChildNode->m_data = childData;

			nodeToExpand->m_childNodes.push_back( newChildNode );

			return newChildNode;
		}

	}

	return nullptr;
}

void MonteCarlo::BackPropagateResult( int whoWon, TreeNode* node )
{
	int whoJustMoved = node->m_data->m_currentGamestate.WhoJustMoved();
	metaData_t& metaData = node->m_data->m_metaData;
	if( whoJustMoved == whoWon )
	{
		metaData.m_numberOfWins++;
	}
	else if( whoWon == TIE )
	{
		metaData.m_numberOfWins += 0.5f;
	}
	else
	{
		//metaData.m_numberOfWins -= 100.f;
	}

	metaData.m_numberOfSimulations++;

	TreeNode* parentNode = node->m_parentNode;
	if( parentNode )
	{
		BackPropagateResult( whoWon, parentNode );
	}
}

float MonteCarlo::GetUCBValueAtNode( TreeNode* node, float explorationParameter )
{
	metaData_t const& metaData = node->m_data->m_metaData;
	TreeNode* parentNode = node->m_parentNode;
	float numberOfSimulations = (float)metaData.m_numberOfSimulations;
	float numberOfWins = (float)metaData.m_numberOfWins;
	float numberOfSimulationsAtParent = 0.f;

	if( parentNode )
	{
		metaData_t const& parentMetaData = parentNode->m_data->m_metaData;
		numberOfSimulationsAtParent = (float)parentMetaData.m_numberOfSimulations;
	}

	if( numberOfSimulations == 0.f )
	{
		return SQRT_2;
	}


	float ucb = numberOfWins/numberOfSimulations + explorationParameter * SquareRootFloat( NaturalLog(numberOfSimulationsAtParent) / numberOfSimulations ); 

	return ucb;
}

bool MonteCarlo::CanExpand( TreeNode const* node )
{
	gamestate_t const& currentGameState = node->m_data->m_currentGamestate;

	int isGameOver = g_theGame->IsGameOverForGameState( currentGameState );
	if( isGameOver != 0 )
	{
		return false;
	}

	int numMoves = g_theGame->GetNumberOfValidMovesAtGameState( currentGameState );
	int numChildren = (int)node->m_childNodes.size();

	if( numMoves > numChildren )
	{
		return true;
	}
	else
	{
		return false;
	}
}

inputMove_t MonteCarlo::GetBestMove()
{
	float bestWinRate = -10000.f;
	float lowestOpponentWinRate = 10000.f;
	inputMove_t bestMove;
	bestNode_t worstOpponentNode;
	worstOpponentNode.nodeWinRate = lowestOpponentWinRate;


	std::vector<TreeNode*> const& childNodes = m_currentHeadNode->m_childNodes;

	for( size_t childIndex = 0; childIndex < childNodes.size(); childIndex++ )
	{
		metaData_t const& metaData = childNodes[childIndex]->m_data->m_metaData;
		float wins = (float)metaData.m_numberOfWins;
		float sims = (float)metaData.m_numberOfSimulations;

		float childWinRate = wins/sims;
		if( childWinRate > 0.999f ) //A winnning move
		{
			bestWinRate = childWinRate;
			bestMove = childNodes[childIndex]->m_data->m_moveToReachNode;
			return bestMove;
		}
		else if( bestWinRate < childWinRate )
		{
			bestWinRate = childWinRate;
			bestMove = childNodes[childIndex]->m_data->m_moveToReachNode;
		}

		bestNode_t bestOpponentNode = GetHighestWinRateChildNode( childNodes[childIndex] );
		if( bestOpponentNode.nodeWinRate < worstOpponentNode.nodeWinRate &&
			bestOpponentNode.node )
		{
			worstOpponentNode = bestOpponentNode;
		}
	}

	if( worstOpponentNode.node )
	{
		bestMove = worstOpponentNode.node->m_parentNode->m_data->m_moveToReachNode;
		//bestMove = worstOpponentNode.node->m_data->m_moveToReachNode;
	}

	return bestMove;
}

bestNode_t MonteCarlo::GetHighestWinRateChildNode( TreeNode const* node )
{
	bestNode_t bestNode;
	bestNode.nodeWinRate = -10000.f;

	std::vector<TreeNode*> const& childNodes = node->m_childNodes;

	for( size_t childIndex = 0; childIndex < childNodes.size(); childIndex++ )
	{
		metaData_t const& metaData = childNodes[childIndex]->m_data->m_metaData;
		float wins = (float)metaData.m_numberOfWins;
		float sims = (float)metaData.m_numberOfSimulations;

		float childWinRate = wins/sims;
		if( childWinRate > bestNode.nodeWinRate )
		{
			bestNode.nodeWinRate = childWinRate;
			bestNode.node = childNodes[childIndex];
		}
	}

	return bestNode;
}

void MonteCarlo::UpdateGame( inputMove_t const& movePlayed, gamestate_t const& newGameState )
{
	std::vector<TreeNode*> const& childNodes = m_currentHeadNode->m_childNodes;
	for( size_t childIndex = 0; childIndex < childNodes.size(); childIndex++ )
	{
		inputMove_t const& childMove = childNodes[childIndex]->m_data->m_moveToReachNode;

		if( movePlayed.m_move == childMove.m_move )
		{
			m_currentHeadNode = childNodes[childIndex];
			return;
		}
	}

	TreeNode* newTreeNode = new TreeNode();
	newTreeNode->m_parentNode = m_currentHeadNode;
	newTreeNode->m_data = new data_t( metaData_t(), movePlayed, newGameState );
	
	m_currentHeadNode->m_childNodes.push_back( newTreeNode );
	m_currentHeadNode = newTreeNode;
}

TreeNode const* MonteCarlo::GetCurrentHeadNode()
{
	return m_currentHeadNode;
}

