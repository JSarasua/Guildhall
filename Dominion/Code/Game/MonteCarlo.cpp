#include "Game/MonteCarlo.hpp"
//#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/JobSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/BufferParser.hpp"
#include "Engine/Core/BufferWriter.hpp"
#include <thread>
#include <chrono>
#include <stack>

class SimulationJob : public Job
{
public:
	SimulationJob( MonteCarlo* mctsToUse, TreeMapNode* simNode );
	virtual void Execute() override;
	virtual void CallBackFunction() override;

	MonteCarlo* m_mctsToUse = nullptr;
	TreeMapNode* nodeToSimulate = nullptr;
	int m_whoWonSim = -1;
};

SimulationJob::SimulationJob( MonteCarlo* mctsToUse, TreeMapNode* simNode ) :
	nodeToSimulate( simNode ),
	m_mctsToUse( mctsToUse ),
	Job()
{}




void SimulationJob::Execute()
{
	m_whoWonSim = m_mctsToUse->RunSimulationOnNode( nodeToSimulate );
}

void SimulationJob::CallBackFunction()
{
	m_mctsToUse->BackPropagateResult( m_whoWonSim, nodeToSimulate );
	m_mctsToUse->m_totalNumberOfSimulationsRun++;

	m_mctsToUse->IncrementIterationsForCurrentMovePostBackPropagation();

}





MonteCarlo::~MonteCarlo()
{
}

void MonteCarlo::RestoreFromData()
{
	size_t fileSize = 0;
	byte* buffer = FileReadToNewBuffer( "data/test.dmcts", &fileSize );

// 	int fakeFileInts[2] = {1,2};
// 	byte* buffer = (byte*)fakeFileInts;
	m_totalNumberOfSimulationsRun = ParseInt( buffer );
	m_numberOfSimulationsToRun = ParseInt( buffer );


}

void MonteCarlo::Startup()
{
	if( !m_headNode )
	{
		m_headNode = new TreeMapNode();
		//m_headNode->m_data = new data_t();
		//m_headNode->m_data->m_currentGamestate = new gamestate_t();
		m_headNode->m_data.m_currentGamestate.m_isFirstMove = true;
		m_currentHeadNode = m_headNode;

		m_mcJobSystem = new JobSystem();
		if( m_useSimThreads )
		{
			m_mcJobSystem->AddWorkerThreads( 5 );
		}

		m_mainThread = new std::thread( &MonteCarlo::WorkerMain, this );
	}
	else
	{
		m_currentHeadNode = m_headNode;
	}

}

void MonteCarlo::Shutdown()
{
	if( m_mainThread )
	{
		m_isQuitting = true;
		m_mainThread->join();
		delete m_mainThread;
		m_mainThread = nullptr;
	}

	if( m_mcJobSystem )
	{
		m_mcJobSystem->Shutdown();
		delete m_mcJobSystem;
		m_mcJobSystem = nullptr;
	}

	if( m_headNode )
	{
		DeleteTree();
		//delete m_headNode;
	}
	m_currentHeadNode = nullptr;
}

void MonteCarlo::SetInitialGameState( gamestate_t const& newGameState )
{
	m_currentHeadNode = m_headNode;
	UpdateGame( inputMove_t(), newGameState );
}

void MonteCarlo::FlushJobSystemQueues()
{
	m_mcJobSystem->ClearQueues();
}

void MonteCarlo::RunSimulations( int numberOfSimulations )
{
	for( int currentSimIndex = 0; currentSimIndex < numberOfSimulations; currentSimIndex++ )
	{
		//DebuggerPrintf( "In RunSimulations step\n" );
		m_numberOfVisitsAtCurrentNode = m_currentHeadNode->m_data.m_metaData.m_numberOfSimulations;

		double startSelectTime = GetCurrentTimeSeconds();
		expand_t expandResult = GetBestNodeToSelect( m_currentHeadNode );
		double endSelectTime = GetCurrentTimeSeconds();
		m_selectTime += (endSelectTime - startSelectTime);
		m_totalTime += (endSelectTime - startSelectTime);

// 		if( nullptr == expandResult.nodeToExpand )
// 		{
// 			//can't select
// 			break;
// 		}

		double startExpandTime = GetCurrentTimeSeconds();
		TreeMapNode* expandedNode = ExpandNode( expandResult );
		double endExpandTime = GetCurrentTimeSeconds();
		m_expandTime += (endExpandTime - startExpandTime);
		m_totalTime += (endExpandTime - startExpandTime);

		if( nullptr == expandedNode )
		{
			//can't expand
			break;
		}

		if( m_headNode->m_data.m_metaData.m_numberOfSimulations > 100 && m_useSimThreads )
		{
			SimulationJob* simJob = new SimulationJob( this, expandedNode );
			m_mcJobSystem->PostJob( simJob );
			m_mcJobSystem->ClaimAndDeleteCompletedJobs();
		}
		else
		{
			//Returns an int to handle case of ties
			double startSimTime = GetCurrentTimeSeconds();
			// 		SimulationJob* simJob = new SimulationJob( this, expandedNode );
			// 		m_mcJobSystem->PostJob( simJob );
			int whoWon = RunSimulationOnNode( expandedNode );
			double endSimTime = GetCurrentTimeSeconds();
			m_simTime += (endSimTime - startSimTime);
			m_totalTime += (endSimTime - startSimTime);

			double startBackPropTime = GetCurrentTimeSeconds();
			//m_mcJobSystem->ClaimAndDeleteCompletedJobs();
			BackPropagateResult( whoWon, expandedNode );
			m_totalNumberOfSimulationsRun++;

			//DebuggerPrintf( "Right Before Increment Iterations\n" );
			if( m_iterationsPerMove > 0 )
			{
				IncrementIterationsForCurrentMovePostBackPropagation();
			}

			double endBackPropTime = GetCurrentTimeSeconds();
			m_backpropagationTime += (endBackPropTime - startBackPropTime);
			m_totalTime += (endBackPropTime - startBackPropTime);
		}
		
		int numberOfJobsQueue = m_mcJobSystem->GetNumberOfJobsQueued();
		while( numberOfJobsQueue > 10 )
		{
			std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
			numberOfJobsQueue = m_mcJobSystem->GetNumberOfJobsQueued();
			//Sleep( 1 );
		}
	}
}

int MonteCarlo::RunSimulationOnNode( TreeMapNode* node )
{
	double randomMoveTime = 0;
	double updateGameStateTime = 0;
	double isGameOverTime = 0;

	gamestate_t currentGameState = node->m_data.m_currentGamestate;
	currentGameState.ShuffleDecks();

	int isGameOver = g_theGame->IsGameOverForGameState( currentGameState );
	if( isGameOver != GAMENOTOVER )
	{
		return isGameOver;
	}

	inputMove_t move; 

	while( isGameOver == GAMENOTOVER )
	{
		double randomMoveStart = GetCurrentTimeSeconds();
		move = GetMoveUsingCurrentRolloutMethod( currentGameState );
		double randomMoveEnd = GetCurrentTimeSeconds();
		randomMoveTime += randomMoveEnd - randomMoveStart;

		double gameStateStart = GetCurrentTimeSeconds();
		currentGameState = g_theGame->GetGameStateAfterMove( currentGameState, move );
		double gameStateEnd = GetCurrentTimeSeconds();
		updateGameStateTime += gameStateEnd - gameStateStart;

		double gameOverStart = GetCurrentTimeSeconds();
		isGameOver = g_theGame->IsGameOverForGameState( currentGameState );
		double gameOverEnd = GetCurrentTimeSeconds();
		isGameOverTime += gameOverEnd - gameOverStart;

	}

	return isGameOver;
}

inputMove_t MonteCarlo::GetBestMoveToDepth( int depth, TreeMapNode* currentNode )
{
	if( depth == 0 )
	{
		g_theConsole->ErrorString( "Depth cannot be 0" );
		return inputMove_t();
	}

	float bestWinRate = -999999.f;
	bool hasFirstMoveBeenCheck = false;
	inputMove_t bestMove;

	int whoseMove = currentNode->m_data.m_currentGamestate.m_whoseMoveIsIt;
	int whoseMoveAtDepth = GetWhoseMoveAtDepth( depth, currentNode );

	for( auto move : currentNode->m_possibleOutcomes )
	{
		std::vector<TreeMapNode*> const& outcomes = move.second;
		float totalSims = 0.f;
		float averageWinRate = 0.f;
		for( TreeMapNode const* outcome : outcomes )
		{
			totalSims += (float)outcome->m_data.m_metaData.m_numberOfSimulations;
		}

		if( totalSims == 0.f )
		{
			continue;
		}

		for( TreeMapNode const* outcome : outcomes )
		{
			playerWinRate_t currentWinRateResult = GetBestWinRateAtDepth( depth - 1, outcome );
			float currentWinRate = currentWinRateResult.winRate;
			int whoseTurn = currentWinRateResult.whoseMove;
			if( whoseTurn != currentNode->m_data.m_currentGamestate.m_whoseMoveIsIt )
			{
				currentWinRate = 1.f - currentWinRate;
			}
			float currentSims = (float)outcome->m_data.m_metaData.m_numberOfSimulations;
			averageWinRate += currentWinRate * ( currentSims / totalSims );
		}

		if( !hasFirstMoveBeenCheck )
		{
			hasFirstMoveBeenCheck = true;

			bestWinRate = averageWinRate;
			bestMove = move.first;
		}
		else
		{
			//If its the opponents move at depth then flip then we want the worst win rate
			if( whoseMove == whoseMoveAtDepth )
			{
				if( averageWinRate > bestWinRate )
				{
					averageWinRate = bestWinRate;
					bestMove = move.first;
				}
			}
			else
			{
				if( averageWinRate < bestWinRate )
				{
					averageWinRate = bestWinRate;
					bestMove = move.first;
				}
			}
		}
	}

	return bestMove;
}

//Get the move that will put the opponent in the worst position
// inputMove_t MonteCarlo::GetBestMoveNegaMax( TreeMapNode const* currentNode )
// {
// 	int whoseMove = currentNode->m_data->m_currentGamestate->m_whoseMoveIsIt;
// 	inputMove_t bestMove;
// 	for( auto move : currentNode->m_possibleOutcomes )
// 	{
// 		std::vector<TreeMapNode*> const& outcomes = move.second;
// 		for( TreeMapNode const* outcome : outcomes )
// 		{
// 
// 		}
// 	}
// 
// 	return bestMove;
// }

// float MonteCarlo::GetBestWinRateForPlayer( TreeMapNode const* currentNode, int playerToCheck, bool hasTurnFlipped )
// {
// 	int whoseMove = currentNode->m_data->m_currentGamestate->m_whoseMoveIsIt;
// 	if( hasTurnFlipped  && playerToCheck == whoseMove )
// 	{
// 		float winrate = currentNode->m_data->m_metaData.GetWinRate();
// 		return winrate;
// 	}
// 	else
// 	{
// 		if( !hasTurnFlipped && whoseMove != playerToCheck )
// 		{
// 			hasTurnFlipped = true;
// 		}
// 
// 		for( auto move : currentNode->m_possibleOutcomes )
// 		{
// 			//Get Average Winrate for each node
// 			for( TreeMapNode const* outcome : move.second )
// 			{
// 
// 			}
// 
// 			//Get the best for the current player
// 		}
// 	}
// 
// 	//Return best for current player
// }

playerWinRate_t MonteCarlo::GetBestWinRateAtDepth( int depth, TreeMapNode const* node )
{
	size_t moveCount = node->m_possibleOutcomes.size();
	if( moveCount == 0 || depth == 0 )
	{
		playerWinRate_t playerWinRate;
		playerWinRate.winRate = node->m_data.m_metaData.GetWinRate();
		playerWinRate.whoseMove = node->m_data.m_currentGamestate.m_whoseMoveIsIt;
		return playerWinRate;
	}

	float bestWinRate = -9999999.f;
	int whoseMove = node->m_data.m_currentGamestate.m_whoseMoveIsIt;
	for( auto move : node->m_possibleOutcomes )
	{

		std::vector<TreeMapNode*> const& outcomesFromMove = move.second;
		float sims = 0.f;
		float currentAverageWinRate = 0.f;
		for( auto outcome : outcomesFromMove )
		{
			sims += outcome->m_data.m_metaData.m_numberOfSimulations;
		}

		for( auto outcome : outcomesFromMove )
		{
			TreeMapNode const* childNode = outcome;
			playerWinRate_t currentNodeWinRate = GetBestWinRateAtDepth( depth - 1, childNode );
			float currentWinRate = currentNodeWinRate.winRate;
			int currentNodeWhoseMove = currentNodeWinRate.whoseMove;
			if( currentNodeWhoseMove != whoseMove )
			{
				currentWinRate = 1.f - currentWinRate;
			}
			float currentSims = (float)childNode->m_data.m_metaData.m_numberOfSimulations;
			currentAverageWinRate += currentWinRate * ( currentSims / sims );
		}

		if( currentAverageWinRate > bestWinRate )
		{
			bestWinRate = currentAverageWinRate;
		}
	}
	playerWinRate_t bestResult;
	bestResult.winRate = bestWinRate;
	bestResult.whoseMove = whoseMove;
	return bestResult;
}

int MonteCarlo::GetWhoseMoveAtDepth( int depth, TreeMapNode const* node )
{
	TreeMapNode const* currentNode = node;
	if( currentNode )
	{
		while( depth > 0 )
		{
			if( node->m_possibleOutcomes.size() > 0 )
			{
				auto move = node->m_possibleOutcomes.begin();
				std::vector<TreeMapNode*> outcomes = move->second;
				if( outcomes.size() > 0 )
				{
					currentNode = outcomes[0];

				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}

			depth--;
		}

		return currentNode->m_data.m_currentGamestate.m_whoseMoveIsIt;
	}
	else
	{
		g_theConsole->ErrorString( "Can't get depth at node because node is null" );
		return -1;
	}
}

inputMove_t MonteCarlo::GetMoveUsingCurrentRolloutMethod( gamestate_t const& gameState )
{
	m_rolloutStrategyLock.lock();
	ROLLOUTMETHOD rolloutMethod = m_rolloutMethod;
	m_rolloutStrategyLock.unlock();

	switch( rolloutMethod )
	{
	case ROLLOUTMETHOD::RANDOM: return g_theGame->GetRandomMoveAtGameState( gameState );
		break;
	case ROLLOUTMETHOD::HEURISTIC: return GetMoveForSimsUsingHeuristic( gameState );
		break;
	case ROLLOUTMETHOD::EPSILONHEURISTIC: return GetMoveForSimsUsingEpsilonHeuristic( gameState );
		break;
	default: ERROR_AND_DIE( "Invalid rollout method" );
		break;
	}
}

inputMove_t MonteCarlo::GetMoveForSimsUsingHeuristic( gamestate_t const& gameState )
{
	m_simMethodLock.lock();
	SIMMETHOD simMethod = m_simMethod;
	m_simMethodLock.unlock();

	switch( simMethod )
	{
	case SIMMETHOD::RANDOM: return g_theGame->GetRandomMoveAtGameState( gameState );
		break;
	case SIMMETHOD::BIGMONEY: return g_theGame->GetMoveUsingBigMoney( gameState );
		break;
	case SIMMETHOD::SINGLEWITCH: return g_theGame->GetMoveUsingSingleWitch( gameState );
		break;
	case SIMMETHOD::DOUBLEWITCH: return g_theGame->GetMoveUsingDoubleWitch( gameState );
		break;
	case SIMMETHOD::SARASUA1: return g_theGame->GetMoveUsingSarasua1( gameState );
		break;
	case SIMMETHOD::GREEDY: return g_theGame->GetMoveUsingGreedy( gameState );
		break;
	case SIMMETHOD::RANDOMPLUS: return g_theGame->GetMoveUsingRandomPlus( gameState );
		break;
	default: return inputMove_t(); //Invalid
		break;
	}

}

inputMove_t MonteCarlo::GetMoveForSimsUsingEpsilonHeuristic( gamestate_t const& gameState )
{
	RandomNumberGenerator& rng = g_theGame->m_rand;
	bool playheuristicMove = rng.RollPercentChance( 1.f - m_epsilon );

	if( playheuristicMove )
	{
		return GetMoveForSimsUsingHeuristic( gameState );
	}
	else
	{
		return g_theGame->GetRandomMoveAtGameState( gameState );
	}
}

std::vector<inputMove_t> MonteCarlo::GetMovesUsingAllHeuristics( gamestate_t const& gameState )
{
	std::vector<inputMove_t> allMoves;
	allMoves.reserve( 10 );

	std::vector<inputMove_t> moves;
	moves.reserve( 10 );
	
// 	inputMove_t bigMoneyMove = g_theGame->GetMoveUsingBigMoney( gameState );
// 	inputMove_t singleWitchMove = g_theGame->GetMoveUsingSingleWitch( gameState );
// 	inputMove_t doubleWitchMove = g_theGame->GetMoveUsingDoubleWitch( gameState );
// 	inputMove_t sarasua1Move = g_theGame->GetMoveUsingSarasua1( gameState );
// 	inputMove_t highestVPMove = g_theGame->GetMoveUsingHighestVP( gameState );
	inputMove_t endPhaseMove = g_theGame->GetEndPhaseMove( gameState );
	inputMove_t highestTreasureMove = g_theGame->GetBuyMoveUsingHighestGoldValue( gameState );
	inputMove_t highestVPMove = g_theGame->GetMoveUsingHighestVP( gameState );
	std::vector<inputMove_t> highestActionMoves = g_theGame->GetHighestCostBuyActionMoves( gameState );
	std::vector<inputMove_t> playActionMoves = g_theGame->GetPlayActionMoves( gameState );

// 	if( bigMoneyMove.m_moveType != INVALID_MOVE )
// 	{
// 		allMoves.push_back( bigMoneyMove );
// 	}
// 	if( singleWitchMove.m_moveType != INVALID_MOVE )
// 	{
// 		allMoves.push_back( singleWitchMove );
// 	}
// 	if( doubleWitchMove.m_moveType != INVALID_MOVE )
// 	{
// 		allMoves.push_back( doubleWitchMove );
// 	}
// 	if( sarasua1Move.m_moveType != INVALID_MOVE )
// 	{
// 		allMoves.push_back( sarasua1Move );
// 	}
// 	if( highestVPMove.m_moveType != INVALID_MOVE )
// 	{
// 		allMoves.push_back( highestVPMove );
// 	}
	if( endPhaseMove.m_moveType != INVALID_MOVE )
	{
		allMoves.push_back( endPhaseMove );
	}
	if( highestTreasureMove.m_moveType != INVALID_MOVE )
	{
		allMoves.push_back( highestTreasureMove );
	}
	if( highestVPMove.m_moveType != INVALID_MOVE )
	{
		allMoves.push_back( highestVPMove );
	}
	allMoves.insert( allMoves.end(), highestActionMoves.begin(), highestActionMoves.end() );
	allMoves.insert( allMoves.end(), playActionMoves.begin(), playActionMoves.end() );




	for( inputMove_t const& move : allMoves )
	{
		bool isMoveInList = false;
		for( inputMove_t const& noDupMove : moves )
		{
			if( move.m_moveType == INVALID_MOVE )
			{
				break;
			}

			if( move == noDupMove )
			{
				isMoveInList = true;
				break;
			}
		}

		if( move.m_moveType == INVALID_MOVE )
		{
			break;
		}

		if( !g_theGame->IsMoveValidForGameState( move, gameState ) )
		{
			break;
		}

		if( !isMoveInList )
		{
			moves.push_back( move );
		}
	}

	return moves;
}

void MonteCarlo::SetExpansionStrategy( EXPANSIONSTRATEGY expansionStrategy )
{
	m_expansionStrategyLock.lock();
	m_expansionStrategy = expansionStrategy;
	m_expansionStrategyLock.unlock();
}

void MonteCarlo::SetRolloutMethod( ROLLOUTMETHOD rolloutMethod )
{
	m_rolloutStrategyLock.lock();
	m_rolloutMethod = rolloutMethod;
	m_rolloutStrategyLock.unlock();
}

void MonteCarlo::SetUCBValue( float ucbValue )
{
	m_ucbLock.lock();
	m_ucbValue = ucbValue;
	m_ucbLock.unlock();
}

void MonteCarlo::SetEpsilonValueZeroToOne( float epsilonValue )
{
	m_epsilonLock.lock();
	m_epsilon = epsilonValue;
	m_epsilonLock.unlock();
}

void MonteCarlo::SetSimMethod( SIMMETHOD simMethod )
{
	m_simMethodLock.lock();
	m_simMethod = simMethod;
	m_simMethodLock.unlock();
}

void MonteCarlo::StopThreads()
{
	if( m_mainThread )
	{
		m_isQuitting = true;
		m_mainThread->join();
		delete m_mainThread;
		m_mainThread = nullptr;
	}

	if( m_mcJobSystem )
	{
		m_mcJobSystem->StopWorkerThreads();
	}
}

void MonteCarlo::StartThreads()
{
	m_isQuitting = false;
	m_mcJobSystem->StartWorkerThreads();

	if( m_useSimThreads )
	{
		m_mcJobSystem->AddWorkerThreads( 3 );
	}

	if( nullptr == m_mainThread )
	{
		m_mainThread = new std::thread( &MonteCarlo::WorkerMain, this );
	}
}

void MonteCarlo::SaveTree()
{
	DeleteFile( "test.mcts" );

	std::vector<byte> buffer;
	BufferWriter bufferWriter = BufferWriter( buffer );
	bufferWriter.AppendInt32( m_totalNumberOfSimulationsRun );
	bufferWriter.AppendInt32( m_numberOfSimulationsToRun );

	m_headNode->AppendTreeToBufferWriter( bufferWriter );
	
	SaveBinaryFileFromBuffer( "test.mcts", bufferWriter.GetBuffer() );
}

void MonteCarlo::LoadTree()
{
	std::vector<byte> bufferVector;
	LoadBinaryFileToExistingBuffer( "test.mcts", bufferVector );
	BufferParser bufferParser = BufferParser( bufferVector );

	m_totalNumberOfSimulationsRun = bufferParser.ParseInt32();
	m_numberOfSimulationsToRun = bufferParser.ParseInt32();
	TreeMapNode* newHeadNode = TreeMapNode::ParseDataFromBufferParser( bufferParser );

	if( m_headNode )
	{
		//delete m_headNode;
		DeleteTree();
	}

	m_headNode = newHeadNode;
	m_currentHeadNode = m_headNode;
}

void MonteCarlo::ResetTree()
{
	StopThreads();
	FlushJobSystemQueues();

	if( m_headNode )
	{
		/*delete m_headNode;*/
		DeleteTree();
	}

	m_headNode = new TreeMapNode();
	m_headNode->m_data.m_currentGamestate.m_isFirstMove = true;
	m_currentHeadNode = m_headNode;

	m_numberOfSimulationsToRun = 0;
	m_totalNumberOfSimulationsRun = 0;
	m_numberOfVisitsAtCurrentNode = 0;

	StartThreads();
}

void MonteCarlo::DeleteTree()
{
	TreeMapNode::DeleteTree( m_headNode );
	m_headNode = nullptr;
}

float MonteCarlo::GetAverageUCBValue( std::vector<TreeMapNode*> const& nodes, float explorationParameter /*= SQRT_2 */ )
{
	float totalSimulations = 0.f;
	float averageUCBValue = 0.f;
	for( size_t nodeIndex = 0; nodeIndex < nodes.size(); nodeIndex++ )
	{
		totalSimulations += nodes[nodeIndex]->m_data.m_metaData.m_numberOfSimulations;
	}

	if( totalSimulations == 0.f )
	{
		return explorationParameter;
	}

	for( size_t nodeIndex = 0; nodeIndex < nodes.size(); nodeIndex++ )
	{
		TreeMapNode const* currentNode = nodes[nodeIndex];
		float currentNumberOfSims = (float)nodes[nodeIndex]->m_data.m_metaData.m_numberOfSimulations;
		float weight = currentNumberOfSims / totalSimulations;
		float currentUCBValue = GetUCBValueAtNode( currentNode, explorationParameter );

		averageUCBValue += currentUCBValue * weight;
		//sumOfUCBValues += GetUCBValueAtNode( currentNode, explorationParameter );
	}

// 	if( averageUCBValue < 0.f )
// 	{
// 		ERROR_AND_DIE("Should never be less than 0");
// 	}

	return averageUCBValue;

}

void MonteCarlo::WorkerMain()
{
	while( !m_isQuitting )
	{
		if( m_numberOfSimulationsToRun > 0 )
		{
			RunSimulations( 1 );
			UpdateBestMove();
			DecrementationIterationsToRunPreSimulation();
			//m_numberOfSimulationsToRun--;
		}
		else
		{
			std::this_thread::sleep_for( std::chrono::microseconds( 10 ) );
		}
		m_mcJobSystem->ClaimAndDeleteCompletedJobs();

		if( UpdateGameIfChanged() )
		{
			UpdateBestMove();
		}
	}
}

void MonteCarlo::UpdateBestMove()
{
	inputMove_t bestMove = GetMostPlayedMove( m_currentHeadNode );

	m_bestMoveLock.lock();
	m_bestMove = bestMove;
	m_bestMoveLock.unlock();
}

bool MonteCarlo::UpdateGameIfChanged()
{
	m_gameStateChangeLock.lock();
	bool didGameStateChange = m_didGameStateChange;
	std::vector<gamestate_t> newGameStates;
	std::vector<inputMove_t> inputMoves;

	newGameStates.swap( m_newGameStates );
	inputMoves.swap( m_movesToMake );
// 	gamestate_t newGameState = m_newGameState;
// 	inputMove_t newInputMove = m_moveToMake;

	if( m_didGameStateChange )
	{
		m_didGameStateChange = false;
	}
	m_gameStateChangeLock.unlock();

	if( didGameStateChange )
	{
		for( size_t moveIndex = 0; moveIndex < inputMoves.size(); moveIndex++ )
		{
			inputMove_t const& newInputMove = inputMoves[moveIndex];
			gamestate_t const& newGameState = newGameStates[moveIndex];

			std::map< inputMove_t, std::vector<TreeMapNode*> >& possibleOutcomes = m_currentHeadNode->m_possibleOutcomes;
			auto outcomeIter = possibleOutcomes.find( newInputMove );

			if( outcomeIter != possibleOutcomes.end() )
			{
				std::vector<TreeMapNode*>& outcomesFromMove = outcomeIter->second;
				for( size_t outcomesIndex = 0; outcomesIndex < outcomesFromMove.size(); outcomesIndex++ )
				{
					gamestate_t const& outcomeState = outcomesFromMove[outcomesIndex]->m_data.m_currentGamestate;
					if( outcomeState.UnordereredEqualsOnlyCurrentPlayer( newGameState ) )
					{
						m_currentHeadNode = outcomesFromMove[outcomesIndex];
						return didGameStateChange;
					}
				}

				//outcome doesn't exist for gamestate
				TreeMapNode* newTreeNode = new TreeMapNode();
				newTreeNode->m_parentNode = m_currentHeadNode;
				//gamestate_t* gameState = new gamestate_t( newGameState );
				newTreeNode->m_data = data_t( metaData_t(), newGameState );
				outcomesFromMove.push_back( newTreeNode );

				m_currentHeadNode = newTreeNode;
			}
			else
			{
				//input doesn't exist
				possibleOutcomes[newInputMove] = std::vector<TreeMapNode*>();
				TreeMapNode* newTreeNode = new TreeMapNode();
				newTreeNode->m_parentNode = m_currentHeadNode;
				//gamestate_t* gameState = new gamestate_t( newGameState );
				newTreeNode->m_data = data_t( metaData_t(), newGameState );
				possibleOutcomes[newInputMove].push_back( newTreeNode );

				m_currentHeadNode = newTreeNode;
			}
		}

	}

	return didGameStateChange;
}

//The best node to select follows these rules
//1. Can the current node be expanded (input that haven't been tried)?
//2. Has the current Gamestate never been reached before? (Previous input has created multiple cases)
expand_t MonteCarlo::GetBestNodeToSelect( TreeMapNode* currentNode )
{
	TreeMapNode* nodeToCheck = currentNode;
	gamestate_t currentGameState = nodeToCheck->m_data.m_currentGamestate;
	
	if( currentGameState.m_isFirstMove && CanExpand( nodeToCheck ) )
	{
		//Handle moving on to next state
		//Get a game state from game
		expand_t result;
		result.gameState = g_theGame->GetRandomInitialGameState();
		result.nodeToExpand = nodeToCheck;
		return result;
	}
	else if( currentGameState.m_isFirstMove )
	{
		//Roll a new game and check if this game state has been done before,
		//if not, add it
		//otherwise move on
		currentGameState = g_theGame->GetRandomInitialGameState();

		inputMove_t defaultInputMove = inputMove_t();
		std::vector<TreeMapNode*> const& childNodesForMove = nodeToCheck->m_possibleOutcomes[defaultInputMove];
		bool isGameStateInVector = false;
		for( size_t childNodeIndex = 0; childNodeIndex < childNodesForMove.size(); childNodeIndex++ )
		{
			gamestate_t const& childGameState = childNodesForMove[childNodeIndex]->m_data.m_currentGamestate;
			if( childGameState.UnordereredEqualsOnlyCurrentPlayer( currentGameState ) )
			{
				nodeToCheck = childNodesForMove[childNodeIndex];
				isGameStateInVector = true;
				break;
			}
		}

		if( !isGameStateInVector )
		{
			//Add it
			expand_t result;
			result.nodeToExpand = nodeToCheck;
			result.gameState = currentGameState;
			//result.m_input = moveToMake;
			return result;
		}
	}

	g_theGame->RandomizeUnknownInfoForGameState( currentGameState );

	//Always choose the current Node if it can be expanded
	if( CanExpand( nodeToCheck ) )
	{
		expand_t result;
		result.nodeToExpand = nodeToCheck;
		return result;
	}

	m_ucbLock.lock();
	float ucbValue = m_ucbValue;
	m_ucbLock.unlock();

	while( !CanExpand( nodeToCheck ) )
	{
		//Get Best move to make
		float highestUCBValue = -999999.f;
		inputMove_t moveToMake;
		for( auto outcomesAfterMove : nodeToCheck->m_possibleOutcomes )
		{
			std::vector<TreeMapNode*> const& childNodesForMove = outcomesAfterMove.second;
			float averageUCBValue = GetAverageUCBValue( childNodesForMove, ucbValue );
			if( averageUCBValue > highestUCBValue )
			{
				highestUCBValue = averageUCBValue;
				moveToMake = outcomesAfterMove.first;
			}
		}
		
		//Will happen at end of games
		if( moveToMake.m_moveType == INVALID_MOVE )
		{
			//g_theConsole->ErrorString( "Selected move to make is invalid" );
			break;
			//ERROR_AND_DIE("Should never give an invalid move");
		}

		//Find if the best move makes a gamestate that has existed before
		//If not, the new gamestate is what we return
		currentGameState = g_theGame->GetGameStateAfterMove( currentGameState, moveToMake );
		std::vector<TreeMapNode*> const& childNodesForMove = nodeToCheck->m_possibleOutcomes[moveToMake];

		bool isGameStateInVector = false;
		for( size_t childNodeIndex = 0; childNodeIndex < childNodesForMove.size(); childNodeIndex++ )
		{
			gamestate_t const& childGameState = childNodesForMove[childNodeIndex]->m_data.m_currentGamestate;
			if( childGameState.UnordereredEqualsOnlyCurrentPlayer( currentGameState ) )
			{
				nodeToCheck = childNodesForMove[childNodeIndex];
				isGameStateInVector = true;

				if( CanExpand( nodeToCheck ) )
				{
					break;
				}
				break;
			}
		}

		if( !isGameStateInVector )
		{
			//Add it
			expand_t result;
			result.nodeToExpand = nodeToCheck;
			result.gameState = currentGameState;
			result.m_input = moveToMake;
			return result;
		}

		//We can't go any farther
		if( g_theGame->IsGameOverForGameState( currentNode->m_data.m_currentGamestate ) != GAMENOTOVER )
		{
			break;
		}
		//Loop again on new current game state
	}

	//Always choose the current Node if it can be expanded
	if( CanExpand( nodeToCheck ) )
	{
		expand_t result;
		result.nodeToExpand = nodeToCheck;
		return result;
	}

	//Can't expand anymore, repeat selection
	expand_t result;
	result.nodeToExpand = nullptr;
	result.nodeToSelect = nodeToCheck;
	return result;
}

TreeMapNode* MonteCarlo::ExpandNode( expand_t expandData )
{
	EXPANSIONSTRATEGY strategy;
	m_expansionStrategyLock.lock();
	strategy = m_expansionStrategy;
	m_expansionStrategyLock.unlock();

	switch( strategy )
	{
	case EXPANSIONSTRATEGY::ALLMOVES: return ExpandNodeUsingAllMoves( expandData );
		break;
	case EXPANSIONSTRATEGY::HEURISTICS: return ExpandNodeUsingHeuristics( expandData );
		break;
	default: return ExpandNodeUsingAllMoves( expandData );
		break;
	}
}

TreeMapNode* MonteCarlo::ExpandNodeUsingAllMoves( expand_t expandData )
{
	TreeMapNode* expandNode = expandData.nodeToExpand;
	TreeMapNode* nodeToSelect = expandData.nodeToSelect;
	inputMove_t const& input = expandData.m_input;
	gamestate_t const& gameState = expandData.gameState;

	if( !expandNode )
	{
		if( nodeToSelect )
		{
			return nodeToSelect;
		}
		else
		{
			ERROR_AND_DIE( "expand and select node null" );
		}
	}
	if( expandNode->m_data.m_currentGamestate.m_isFirstMove )
	{
		TreeMapNode* newNode = new TreeMapNode();
		newNode->m_parentNode = expandNode;
		//gamestate_t* newGameState = new gamestate_t( gameState );
		newNode->m_data = data_t( metaData_t(), gameState );

		expandNode->m_possibleOutcomes[input].push_back( newNode );

		return newNode;
	}

	if( CanExpand( expandNode ) )
	{
		gamestate_t const& currentGameState = expandNode->m_data.m_currentGamestate;
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
				gamestate_t newGameState = gamestate_t( g_theGame->GetGameStateAfterMove( currentGameState, currentMove ) );
				newNode->m_data = data_t( metaData_t(), newGameState );
				newVectorOfOutComes.push_back( newNode );

				return newNode;
			}
		}

	}
	else if( nodeToSelect )
	{
		return nodeToSelect;
	}
	else
	{
		//This must be a move that already exists but a new gamestate
		TreeMapNode* newNode = new TreeMapNode();
		newNode->m_parentNode = expandNode;
		//gamestate_t* newGameState = new gamestate_t( gameState );
		newNode->m_data = data_t( metaData_t(), gameState );

		expandNode->m_possibleOutcomes[input].push_back( newNode );

		return newNode;
	}

	return nullptr;
}

TreeMapNode* MonteCarlo::ExpandNodeUsingHeuristics( expand_t expandData )
{
	TreeMapNode* expandNode = expandData.nodeToExpand;
	TreeMapNode* nodeToSelect = expandData.nodeToSelect;
	inputMove_t const& input = expandData.m_input;
	gamestate_t const& gameState = expandData.gameState;

	if( !expandNode )
	{
		if( nodeToSelect )
		{
			return nodeToSelect;
		}
		else
		{
			ERROR_AND_DIE( "expand and select node null" );
		}
	}
	if( expandNode->m_data.m_currentGamestate.m_isFirstMove )
	{
		TreeMapNode* newNode = new TreeMapNode();
		newNode->m_parentNode = expandNode;
		//gamestate_t* newGameState = new gamestate_t( gameState );
		newNode->m_data = data_t( metaData_t(), gameState );

		expandNode->m_possibleOutcomes[input].push_back( newNode );

		return newNode;
	}

	if( CanExpandUsingHeuristic( expandNode ) )
	{
		gamestate_t const& currentGameState = expandNode->m_data.m_currentGamestate;
		std::vector<inputMove_t> validMoves = GetMovesUsingAllHeuristics( currentGameState );

		//Find first inputMove that hasn't been created and add it
		for( size_t validMoveIndex = 0; validMoveIndex < validMoves.size(); validMoveIndex++ )
		{
			inputMove_t const& currentMove = validMoves[validMoveIndex];
			auto outcome = expandNode->m_possibleOutcomes.find( currentMove );
			if( outcome == expandNode->m_possibleOutcomes.end() )
			{
				if( currentMove.m_moveType == INVALID_MOVE )
				{
					break;
				}
				expandNode->m_possibleOutcomes[currentMove] = std::vector<TreeMapNode*>();
				std::vector<TreeMapNode*>& newVectorOfOutComes = expandNode->m_possibleOutcomes[currentMove];

				TreeMapNode* newNode = new TreeMapNode();
				newNode->m_parentNode = expandNode;
				gamestate_t newGameState = g_theGame->GetGameStateAfterMove( currentGameState, currentMove );
				newNode->m_data = data_t( metaData_t(), newGameState );
				newVectorOfOutComes.push_back( newNode );

				return newNode;
			}
		}
		ERROR_AND_DIE( "Said could expand but couldn't" );

	}
	else if( nodeToSelect )
	{
		return nodeToSelect;
	}
	else
	{
		//This must be a move that already exists but a new gamestate
		TreeMapNode* newNode = new TreeMapNode();
		newNode->m_parentNode = expandNode;
		//gamestate_t* newGameState = new gamestate_t( gameState );
		newNode->m_data = data_t( metaData_t(), gameState );

		if( input.m_moveType == INVALID_MOVE )
		{
			return nullptr;
		}

		expandNode->m_possibleOutcomes[input].push_back( newNode );

		return newNode;
	}

	return nullptr;
}

void MonteCarlo::BackPropagateResult( int whoWon, TreeMapNode* node )
{
	TreeMapNode* currentNode = node;
	while( currentNode != m_currentHeadNode )
	{
		int whoJustMoved = -1;
		if( currentNode->m_parentNode )
		{
			if( currentNode->m_parentNode->m_data.m_currentGamestate.m_isFirstMove )
			{
				whoJustMoved = PLAYER_1;
			}
			else
			{
				whoJustMoved = currentNode->m_parentNode->m_data.m_currentGamestate.m_whoseMoveIsIt;
			}
		}
		else
		{
			whoJustMoved = PLAYER_1;
		}

		metaData_t& metaData = currentNode->m_data.m_metaData;
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

		currentNode = currentNode->m_parentNode;
	}

	int whoJustMoved = -1;
	if( currentNode->m_parentNode )
	{
		if( currentNode->m_parentNode->m_data.m_currentGamestate.m_isFirstMove )
		{
			whoJustMoved = PLAYER_1;
		}
		else
		{
			whoJustMoved = currentNode->m_parentNode->m_data.m_currentGamestate.m_whoseMoveIsIt;
		}
	}
	else
	{
		whoJustMoved = PLAYER_1;
	}

	metaData_t& metaData = currentNode->m_data.m_metaData;
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

// 	int whoJustMoved = -1;
// 	if( node->m_parentNode )
// 	{
// 		if( node->m_parentNode->m_data.m_currentGamestate.m_isFirstMove )
// 		{
// 			whoJustMoved = PLAYER_1;
// 		}
// 		else
// 		{
// 			whoJustMoved = node->m_parentNode->m_data.m_currentGamestate.m_whoseMoveIsIt;
// 		}
// 	}
// 	else
// 	{
// 		whoJustMoved = PLAYER_1;
// 	}
// 
// 	metaData_t& metaData = node->m_data.m_metaData;
// 	if( whoJustMoved == whoWon )
// 	{
// 		metaData.m_numberOfWins++;
// 	}
// 	else if( whoWon == TIE )
// 	{
// 		metaData.m_numberOfWins += 0.5f;
// 	}
// 	else
// 	{
// 		//metaData.m_numberOfWins -= 100.f;
// 	}
// 
// 	metaData.m_numberOfSimulations++;
// 
// 	TreeMapNode* parentNode = node->m_parentNode;
// 	if( parentNode )
// 	{
// 		BackPropagateResult( whoWon, parentNode );
// 	}
}



inputMove_t MonteCarlo::GetMostPlayedMove( TreeMapNode* currentNode )
{
	int mostSims = -99999;
	float bestWinRate = -999999.f;
	inputMove_t bestMove;

	for( auto move : currentNode->m_possibleOutcomes )
	{
		std::vector<TreeMapNode*> const& outcomesFromMove = move.second;
		int currentSims = 0;
		float currentWins = 0.f;
		for( TreeMapNode const* outcome : outcomesFromMove )
		{
			currentSims += outcome->m_data.m_metaData.m_numberOfSimulations;
			currentWins += outcome->m_data.m_metaData.m_numberOfWins;
		}

		float currentWinRate = currentWins / (float)currentSims;

		if( currentSims > mostSims )
		{
			//If the move ends the turn, then end phase will have a larger number of possible outcomes than other moves.
			if( move.first.m_moveType == END_PHASE && currentNode->m_data.m_currentGamestate.m_currentPhase == BUY_PHASE )
			{
				if( currentWinRate > bestWinRate )
				{
					mostSims = currentSims;
					bestWinRate = currentWinRate;
					bestMove = move.first;
				}
			}
			else
			{
				mostSims = currentSims;
				bestWinRate = currentWinRate;
				bestMove = move.first;
			}
		}
	}

	return bestMove;
}

inputMove_t MonteCarlo::GetBestWinRateMove( TreeMapNode* currentNode )
{
	float bestWinRate = -10000.f;
	float lowestOpponentWinRate = 10000.f;
	inputMove_t bestMove;
	bestNode_t worstOpponentNode;
	worstOpponentNode.nodeWinRate = lowestOpponentWinRate;

// 	if( currentNode->m_data->m_currentGamestate->m_isFirstMove )
// 	{
// 		//Need an initial game state to be able to choose best move
// 		return;
// 		//UpdateGame( inputMove_t(), *g_theGame->m_currentGameState );
// 	}

	for( auto validMoveIter : currentNode->m_possibleOutcomes )
	{
		std::vector<TreeMapNode*> const& outcomesFromMove = validMoveIter.second;

		float sumOfWins = 0;
		float sumOfSims = 0;

		for( size_t outcomeIndex = 0; outcomeIndex < outcomesFromMove.size(); outcomeIndex++ )
		{
			metaData_t const& metaData = outcomesFromMove[outcomeIndex]->m_data.m_metaData;
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
			break;
		}
		else if( bestWinRate < childWinRate )
		{
			bestWinRate = childWinRate;
			bestMove = validMoveIter.first;
		}
	}

	return bestMove;
}

float MonteCarlo::GetUCBValueAtNode( TreeMapNode const* node, float explorationParameter )
{
	metaData_t const& metaData = node->m_data.m_metaData;
	TreeMapNode* parentNode = node->m_parentNode;
	float numberOfSimulations = (float)metaData.m_numberOfSimulations;
	float numberOfWins = (float)metaData.m_numberOfWins;
	float numberOfSimulationsAtParent = 0.f;

	if( parentNode )
	{
		metaData_t const& parentMetaData = parentNode->m_data.m_metaData;
		numberOfSimulationsAtParent = (float)parentMetaData.m_numberOfSimulations;
	}

	if( numberOfSimulations == 0.f )
	{
		return SQRT_2;
	}

	float score = 0.f;

	SCORESTRATEGY scoreStategy;
	m_scoreStrategyLock.lock();
	scoreStategy = m_scoreStrategy;
	m_scoreStrategyLock.unlock();


	//Winpoint = Wins/Total Games Player
	//Diff = My VP score - their VP score


	switch( scoreStategy )
	{
		//WinPercentage = Winpoint
	case SCORESTRATEGY::WINPERCENTAGE:
	{
		score = numberOfWins/numberOfSimulations;
		break;
	}
		//MCDOM = Winpoint + Diff/100
	case SCORESTRATEGY::MCDOM:
	{
		gamestate_t const& currentGameState = node->m_data.m_currentGamestate;
		int myVPs = g_theGame->GetCurrentPlayersScore( currentGameState );
		int theirVPs = g_theGame->GetOpponentsScore( currentGameState );
		int diffVPs = myVPs - theirVPs;
		score = numberOfWins/numberOfSimulations + ((float)diffVPs)/100.f;
		break;
	}
	default: ERROR_AND_DIE("Score stategy for UCB calculation is invalid");
		break;
	}

	float ucb = score + explorationParameter * SquareRootFloat( NaturalLog(numberOfSimulationsAtParent) / numberOfSimulations ); 

	return ucb;
}

bool MonteCarlo::CanExpand( TreeMapNode const* node )
{
	m_expansionStrategyLock.lock();
	EXPANSIONSTRATEGY strategy = m_expansionStrategy;
	m_expansionStrategyLock.unlock();

	switch( strategy )
	{
	case EXPANSIONSTRATEGY::ALLMOVES: return CanExpandUsingMoves( node );
		break;
	case EXPANSIONSTRATEGY::HEURISTICS: return CanExpandUsingHeuristic( node );
		break;
	default: return CanExpandUsingMoves( nullptr );
		break;
	}
}

bool MonteCarlo::CanExpandUsingMoves( TreeMapNode const* node )
{
	gamestate_t const& currentGameState = node->m_data.m_currentGamestate;

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

bool MonteCarlo::CanExpandUsingHeuristic( TreeMapNode const* node )
{
	gamestate_t const& currentGameState = node->m_data.m_currentGamestate;
	std::vector<inputMove_t> moves = GetMovesUsingAllHeuristics( currentGameState );

	for( inputMove_t const& move : moves )
	{
		if( move.m_moveType == INVALID_MOVE )
		{
			break;
		}

		auto nodeIter = node->m_possibleOutcomes.find( move );
		if( nodeIter == node->m_possibleOutcomes.end() )
		{
			return true;
		}
	}

	return false;
}

inputMove_t MonteCarlo::GetBestMove()
{
	if( m_isMoveReady || m_iterationsPerMove < 0 )
	{
		m_isMoveReady = false;

		inputMove_t bestMove;
		m_bestMoveLock.lock();
		bestMove = m_bestMove;
		m_bestMoveLock.unlock();

		return bestMove;
	}
	else
	{
		return inputMove_t();
	}
}

void MonteCarlo::UpdateGame( inputMove_t const& movePlayed, gamestate_t const& newGameState )
{
	m_gameStateChangeLock.lock();
	m_didGameStateChange = true;
	m_movesToMake.push_back( movePlayed );
	m_newGameStates.push_back( newGameState );
// 	m_moveToMake = movePlayed;
// 	m_newGameState = newGameState;
	m_gameStateChangeLock.unlock();
}

void MonteCarlo::AddSimulations( int simulationsToAdd )
{
	m_numberOfSimulationsToRun += simulationsToAdd;
}

TreeMapNode const* MonteCarlo::GetCurrentHeadNode()
{
	return m_currentHeadNode;
}

void MonteCarlo::SetIterationCountPerMove( int moveCount )
{
	m_iterationsPerMove = moveCount;
}

void MonteCarlo::IncrementIterationsForCurrentMovePostBackPropagation()
{
	m_iterationLock.lock();
	m_numberOfIterationsForCurrentMove++;
	//DebuggerPrintf( "Move %i\n", (int)m_numberOfIterationsForCurrentMove );
	if( m_numberOfIterationsForCurrentMove >= m_iterationsPerMove )
	{
		m_numberOfIterationsForCurrentMove = 0;

		if( m_numberOfSimulationsToRun == 0 )
		{
			m_isMoveReady = true;
		}
		//GUARANTEE_OR_DIE( m_numberOfIterationsForCurrentMove == m_numberOfSimulationsToRun, "Iterations is off count")
	}
	m_iterationLock.unlock();
}

void MonteCarlo::DecrementationIterationsToRunPreSimulation()
{
	m_iterationLock.lock();
	m_numberOfSimulationsToRun--;
	//DebuggerPrintf( "Sim %i\n", (int)m_numberOfSimulationsToRun );
	if( m_iterationsPerMove > 0 )
	{
		GUARANTEE_OR_DIE( m_numberOfSimulationsToRun + m_numberOfIterationsForCurrentMove == m_iterationsPerMove ||
			((m_numberOfSimulationsToRun == m_numberOfIterationsForCurrentMove) && (m_numberOfSimulationsToRun == 0))
			, "Iterations and moves to make don't match" );
	}

	if( m_numberOfSimulationsToRun == 0 && m_numberOfIterationsForCurrentMove == 0 )
	{
		m_isMoveReady = true;
	}
	m_iterationLock.unlock();
}

void MonteCarlo::RunMCTSForCurrentMoveIterationCount()
{
	m_iterationLock.lock();
	m_numberOfSimulationsToRun += m_iterationsPerMove;
	m_iterationLock.unlock();
}

