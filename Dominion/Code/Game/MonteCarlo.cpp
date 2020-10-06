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

void MonteCarlo::Startup( gamestate_t const& newGameState )
{
	m_headNode = new TreeMapNode();
	m_headNode->m_data = new data_t();
	m_headNode->m_data->m_currentGamestate = new gamestate_t( newGameState );

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


		expand_t expandResult = GetBestNodeToSelect( m_currentHeadNode );

		if( nullptr == expandResult.nodeToExpand )
		{
			//can't select
			break;
		}
		TreeMapNode* expandedNode = ExpandNode( expandResult );
		if( nullptr == expandedNode )
		{
			//can't expand
			break;
		}
		//Returns an int to handle case of ties
		int whoWon = RunSimulationOnNode( expandedNode );
		BackPropagateResult( whoWon, expandedNode );
	}
}

int MonteCarlo::RunSimulationOnNode( TreeMapNode* node )
{
	gamestate_t currentGameState = *node->m_data->m_currentGamestate;
	currentGameState.ShuffleDecks();

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

// TreeMapNode* MonteCarlo::GetBestNodeToSelectAndExpand( TreeMapNode* currentNode )
// {
// 	gamestate_t currentGameState = *m_currentHeadNode->m_data->m_currentGamestate;
// 
// 	if( g_theGame->IsGameOverForGameState( currentGameState ) != GAMENOTOVER )
// 	{
// 		return nullptr;
// 	}
// 
// 	if( currentGameState.m_isFirstMove )
// 	{
// 		currentGameState.ResetDecks();
// 
// 		inputMove_t defaultInputMove = inputMove_t();
// 		auto currentIter = m_currentHeadNode->m_possibleOutcomes.find( defaultInputMove );
// 		std::vector<TreeMapNode*> possibleOutcomes = currentIter->second;
// 
// 		for( size_t possibleOutcomesIndex = 0; possibleOutcomesIndex < possibleOutcomes.size(); possibleOutcomesIndex++ )
// 		{
// 			TreeMapNode* outcome = possibleOutcomes[possibleOutcomesIndex];
// 			gamestate_t const& outcomeGamestate = *outcome->m_data->m_currentGamestate;
// 
// 			if( currentGameState.UnordereredEqualsOnlyCurrentPlayer( outcomeGamestate ) )
// 			{
// 				return GetBestNodeToSelectAndExpand( outcome );
// 			}
// 		}
// 
// 		TreeMapNode* newNode = new TreeMapNode();
// 		newNode->m_parentNode = currentNode;
// 		gamestate_t* newGameState = new gamestate_t( currentGameState );
// 		newNode->m_data = new data_t( metaData_t(), defaultInputMove, newGameState );
// 		return newNode;
// 	}
// 	else
// 	{
// 		currentGameState.ShuffleDecks();
// 
// 		if( CanExpand( currentNode ) )
// 		{
// 			//Expand node and return it
// 		}
// 		else
// 		{
// 			//inputMove_t bestInputMove = GetBestInputChoiceFromChildren( currentNode );
// 			//UpdateGame( bestInputMove, currentGameState );
// 
// 			/*
// 			//TreeMapNode* childNode = GetChildAfterMoveFromChildren( bestInputMove, currentGameState )
// 			//if( childNode )
// 			{
// 				return GetBestNodeToSelectAndExpand( childNode );
// 			}
// 			else
// 			{
// 				TreeMapNode* newNode = MakeNewNode
// 				return newNode;
// 			}
// 			*/
// 			//TreeMapNode* bestChildNode = //GetBestNodeFromChildren(currentNode)
// 		}
// 
// 		float highestUCB = -1.f;
// 		for( auto outcome : m_currentHeadNode->m_possibleOutcomes )
// 		{
// 			//GetAverage UCB and compare against highest UCB
// 			float averageUCB = 0;
// 		}
// 	}
// 
// 	
// 
// 
// 
// }

// inputMove_t MonteCarlo::GetBestInputChoiceFromChildren( TreeMapNode* node )
// {
// 	for( auto moveOutcomes: node->m_possibleOutcomes )
// 	{
// 		std::vector<TreeMapNode*>& outcomes = moveOutcomes.second;
// 
// 	}
// }


float MonteCarlo::GetAverageUCBValue( std::vector<TreeMapNode*> const& nodes, float explorationParameter /*= SQRT_2 */ )
{
	float sumOfUCBValues = 0.f;

	for( size_t nodeIndex = 0; nodeIndex < nodes.size(); nodeIndex++ )
	{
		TreeMapNode const* currentNode = nodes[nodeIndex];
		sumOfUCBValues += GetUCBValueAtNode( currentNode, explorationParameter );
	}

	float count = (float)nodes.size();

	return sumOfUCBValues/count;

}

expand_t MonteCarlo::GetBestNodeToSelect( TreeMapNode* currentNode )
{
	gamestate_t currentGameState = *currentNode->m_data->m_currentGamestate;
	
	if( currentGameState.m_isFirstMove )
	{

	}

	else if( CanExpand( currentNode ) )
	{
		expand_t result;
		result.nodeToExpand = currentNode;
		return result;
	}
	else
	{
		float highestUCBValue = -1.f;
		inputMove_t moveToMake;
		for( auto outcomesAfterMove : currentNode->m_possibleOutcomes )
		{
			std::vector<TreeMapNode*> const& childNodesForMove = outcomesAfterMove.second;
			float ucbValue = GetAverageUCBValue( childNodesForMove );
			if( ucbValue > highestUCBValue )
			{
				highestUCBValue = ucbValue;
				moveToMake = outcomesAfterMove.first;
			}
		}
	}

	while( !CanExpand( currentNode ) )
	{
		float highestUCBValue = -1.f;
		inputMove_t moveToMake;
		for( auto outcomesAfterMove : currentNode->m_possibleOutcomes )
		{
			std::vector<TreeMapNode*> const& childNodesForMove = outcomesAfterMove.second;
			float ucbValue = GetAverageUCBValue( childNodesForMove );
			if( ucbValue > highestUCBValue )
			{
				highestUCBValue = ucbValue;
				moveToMake = outcomesAfterMove.first;
			}
		}
		currentGameState = g_theGame->GetGameStateAfterMove( currentGameState, moveToMake );
		std::vector<TreeMapNode*> const& childNodesForMove = currentNode->m_possibleOutcomes[moveToMake];

		bool isGameStateInVector = false;
		for( size_t childNodeIndex = 0; childNodeIndex < childNodesForMove.size(); childNodeIndex++ )
		{
			gamestate_t const& childGameState = *childNodesForMove[childNodeIndex]->m_data->m_currentGamestate;
			if( childGameState.UnordereredEqualsOnlyCurrentPlayer( currentGameState ) )
			{
				currentNode = childNodesForMove[childNodeIndex];
				isGameStateInVector = true;
				break;
			}
		}

		//Returns if game state has never occurred before. An example where this occurs is if your opponent ends his turn, but the hand you draw isn't the same as the last time this node was reached
		if( !isGameStateInVector )
		{
			//Add it
			expand_t result;
			result.nodeToExpand = currentNode;
			result.gameState = currentGameState;
			result.m_input = moveToMake;
			return result;
		}
		//Loop again on new current game state
	}

	if( g_theGame->IsGameOverForGameState( *currentNode->m_data->m_currentGamestate ) != GAMENOTOVER )
	{
		expand_t result;
		result.nodeToExpand = currentNode;
		return result;
	}

	//Can't expand anymore
	expand_t result;
	result.nodeToExpand = nullptr;
	return result;
}

TreeMapNode* MonteCarlo::ExpandNode( expand_t expandData )
{
	TreeMapNode* expandNode = expandData.nodeToExpand;
	inputMove_t const& input = expandData.m_input;
	gamestate_t const& gameState = expandData.gameState;
	
	if( expandNode->m_data->m_currentGamestate->m_isFirstMove )
	{
		TreeMapNode* newNode = new TreeMapNode();
		newNode->m_parentNode = expandNode;
		gamestate_t* newGameState = new gamestate_t( gameState );
		newNode->m_data = new data_t( metaData_t(), input, newGameState );

		expandNode->m_possibleOutcomes[input].push_back( newNode );
		return newNode;
	}

	if( CanExpand( expandNode ) )
	{
		gamestate_t const& currentGameState = *expandNode->m_data->m_currentGamestate;
		std::vector<inputMove_t> validMoves = g_theGame->GetValidMovesAtGameState( currentGameState );
		
		//Find first inputMove that hasn't been created and add it
		for( size_t validMoveIndex = 0; validMoveIndex < validMoves.size(); validMoveIndex++ )
		{
			inputMove_t const& currentMove = validMoves[validMoveIndex];
			auto outcome = expandNode->m_possibleOutcomes.find( currentMove );
			if( outcome == expandNode->m_possibleOutcomes.end() )
			{
				expandNode->m_possibleOutcomes[currentMove] = std::vector<TreeMapNode*>();
				std::vector<TreeMapNode*>& newVectorOfOutComes = expandNode->m_possibleOutcomes[currentMove];

				TreeMapNode* newNode = new TreeMapNode();
				newNode->m_parentNode = expandNode;
				gamestate_t* newGameState = new gamestate_t( g_theGame->GetGameStateAfterMove( currentGameState, currentMove ) );
				newNode->m_data = new data_t( metaData_t(), input, newGameState );
				newVectorOfOutComes.push_back( newNode );
				
				return newNode;
			}
		}
	}
	else
	{
		//This must be a move that already exists but a new gamestate
		TreeMapNode* newNode = new TreeMapNode();
		newNode->m_parentNode = expandNode;
		gamestate_t* newGameState = new gamestate_t( gameState );
		newNode->m_data = new data_t( metaData_t(), input, newGameState );

		expandNode->m_possibleOutcomes[input].push_back( newNode );

		return newNode;
	}

	return nullptr;
}

void MonteCarlo::BackPropagateResult( int whoWon, TreeMapNode* node )
{
	int whoJustMoved = node->m_data->m_currentGamestate->WhoJustMoved();
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

	TreeMapNode* parentNode = node->m_parentNode;
	if( parentNode )
	{
		BackPropagateResult( whoWon, parentNode );
	}
}

float MonteCarlo::GetUCBValueAtNode( TreeMapNode const* node, float explorationParameter )
{
	metaData_t const& metaData = node->m_data->m_metaData;
	TreeMapNode* parentNode = node->m_parentNode;
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

bool MonteCarlo::CanExpand( TreeMapNode const* node )
{
 	gamestate_t const& currentGameState = *node->m_data->m_currentGamestate;

	int isGameOver = g_theGame->IsGameOverForGameState( currentGameState );
	if( isGameOver != GAMENOTOVER )
	{
		return false;
	}

	int numMoves = g_theGame->GetNumberOfValidMovesAtGameState( currentGameState );
	int numChildren = (int)node->m_possibleOutcomes.size();

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


	for( auto validMoveIter : m_currentHeadNode->m_possibleOutcomes )
	{
		std::vector<TreeMapNode*> const& outcomesFromMove = validMoveIter.second;

		float sumOfWins = 0;
		float sumOfSims = 0;

		for( size_t outcomeIndex = 0; outcomeIndex < outcomesFromMove.size(); outcomeIndex++ )
		{
			metaData_t const& metaData = outcomesFromMove[outcomeIndex]->m_data->m_metaData;
			float wins = (float)metaData.m_numberOfWins;
			float sims = (float)metaData.m_numberOfSimulations;

			sumOfWins += wins;
			sumOfSims += sims;
		}

		float childWinRate = sumOfWins/sumOfSims;
		if( childWinRate > 0.999f ) //A winnning move
		{
			bestWinRate = childWinRate;
			bestMove = validMoveIter.first;
			return bestMove;
		}
		else if( bestWinRate < childWinRate )
		{
			bestWinRate = childWinRate;
			bestMove = validMoveIter.first;
		}
	}

	//Think about getting worst opponentNode
	return bestMove;
}

// bestNode_t MonteCarlo::GetHighestWinRateChildNode( TreeMapNode const* node )
// {
// 	bestNode_t bestNode;
// 	bestNode.nodeWinRate = -10000.f;
// 	//Rethink this with regards to average outcome
// 
// 
// // 	std::vector<TreeMapNode*> const& childNodes = node->m_childNodes;
// // 
// // 	for( size_t childIndex = 0; childIndex < childNodes.size(); childIndex++ )
// // 	{
// // 		metaData_t const& metaData = childNodes[childIndex]->m_data->m_metaData;
// // 		float wins = (float)metaData.m_numberOfWins;
// // 		float sims = (float)metaData.m_numberOfSimulations;
// // 
// // 		float childWinRate = wins/sims;
// // 		if( childWinRate > bestNode.nodeWinRate )
// // 		{
// // 			bestNode.nodeWinRate = childWinRate;
// // 			bestNode.node = childNodes[childIndex];
// // 		}
// // 	}
// 
// 	return bestNode;
// }

void MonteCarlo::UpdateGame( inputMove_t const& movePlayed, gamestate_t const& newGameState )
{
	std::map< inputMove_t, std::vector<TreeMapNode*> >& possibleOutcomes = m_currentHeadNode->m_possibleOutcomes;
	auto outcomeIter = possibleOutcomes.find( movePlayed );

	if( outcomeIter != possibleOutcomes.end() )
	{
		std::vector<TreeMapNode*>& outcomesFromMove = outcomeIter->second;
		for( size_t outcomesIndex = 0; outcomesIndex < outcomesFromMove.size(); outcomesIndex++ )
		{
			gamestate_t const& outcomeState = *outcomesFromMove[outcomesIndex]->m_data->m_currentGamestate;
			if( outcomeState.UnordereredEqualsOnlyCurrentPlayer( newGameState ) )
			{
				m_currentHeadNode = outcomesFromMove[outcomesIndex];
				return;
			}
		}

		//outcome doesn't exist for gamestate
		TreeMapNode* newTreeNode = new TreeMapNode();
		newTreeNode->m_parentNode = m_currentHeadNode;
		gamestate_t* gameState = new gamestate_t( newGameState );
		newTreeNode->m_data = new data_t( metaData_t(), movePlayed, gameState );
		outcomesFromMove.push_back( newTreeNode );

		m_currentHeadNode = newTreeNode;
	}
	else
	{
		//input doesn't exist
		possibleOutcomes[movePlayed] = std::vector<TreeMapNode*>();
		TreeMapNode* newTreeNode = new TreeMapNode();
		newTreeNode->m_parentNode = m_currentHeadNode;
		gamestate_t* gameState = new gamestate_t( newGameState );
		newTreeNode->m_data = new data_t( metaData_t(), movePlayed, gameState );
		possibleOutcomes[movePlayed].push_back( newTreeNode );

		m_currentHeadNode = newTreeNode;
	}
}

TreeMapNode const* MonteCarlo::GetCurrentHeadNode()
{
	return m_currentHeadNode;
}

