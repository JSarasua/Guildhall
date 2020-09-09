#include "Game/MonteCarlo.hpp"
#include "Game/Game.hpp"
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
		BackPropagateResult( didWin );
	}
}

bool MonteCarlo::RunSimulationOnNode( TreeNode* node )
{
	//TODO


	return false;
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

	//TODO

	return nullptr;
}

void MonteCarlo::BackPropagateResult( bool didWin )
{
	//TODO
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
	//TODO


	return false;
}
