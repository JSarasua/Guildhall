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
		bool didWin = RunSimulationOnNode( expandedNode );
		BackPropagateResult( didWin, expandedNode );
	}
}

bool MonteCarlo::RunSimulationOnNode( TreeNode* node )
{
	gamestate_t currentGameState = node->m_data->m_currentGamestate;
	int isGameOver = g_theGame->IsGameOverForGameState( currentGameState );
	if( isGameOver != 0 )
	{
		if( isGameOver == m_player )
		{
			return true; //You won
		}
		else
		{
			return false; //You lost
		}
	}

	inputMove_t move; 

	while( isGameOver == 0 )
	{
		move = g_theGame->GetRandomMoveAtGameState( currentGameState );
		currentGameState = g_theGame->GetGameStateAfterMove( currentGameState, move );
		isGameOver = g_theGame->IsGameOverForGameState( currentGameState );
	}

	if( isGameOver == m_player )
	{
		return true; //You won
	}
	else if( isGameOver == TIE )
	{
		return false; //You tied, should it be false???
	}
	else
	{
		return false; //You lost
	}
}

ucbResult_t MonteCarlo::GetBestNodeToSelect( TreeNode* currentNode )
{
	float highestUCBValue = 0.f;
	TreeNode* bestTreeNode = nullptr;

	if( CanExpand( currentNode ) )
	{
		highestUCBValue = GetUCBValueAtNode( currentNode );
		bestTreeNode = currentNode;
	}

	std::vector<TreeNode*>& childNodes = currentNode->m_childNodes;
	size_t numChildNodes = currentNode->m_childNodes.size();
	for( size_t childIndex = 0; childIndex < numChildNodes; childIndex++ )
	{
		ucbResult_t result = GetBestNodeToSelect( childNodes[childIndex] );

		if( result.ucbValue > highestUCBValue )
		{
			highestUCBValue = result.ucbValue;
			bestTreeNode = childNodes[childIndex];
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

			return newChildNode;
		}

	}

	return nullptr;
}

void MonteCarlo::BackPropagateResult( bool didWin, TreeNode* node )
{
	metaData_t& metaData = node->m_data->m_metaData;
	metaData.m_numberOfWins += (int)didWin;
	metaData.m_numberOfSimulations++;

	TreeNode* parentNode = node->m_parentNode;
	if( parentNode )
	{
		BackPropagateResult( didWin, parentNode );
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
