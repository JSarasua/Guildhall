#include "Game/Game.hpp"
#include "Game/TreeNode.hpp"

constexpr float SQRT_2 = 1.41421356237f;

struct moveMetaData_t
{
	float m_numberOfSimulations = 0.f;
	float m_numberOfWins = 0.f;

	inputMove_t m_move;
};

class MonteCarloNoTree
{
public:
	MonteCarloNoTree() = default;
	~MonteCarloNoTree() = default;

	void SetCurrentGameState( gamestate_t const& gameState ) { m_currentGameState = gameState; }
	void ResetPossibleMoves();
	void UpdateBestMove();


	//Base MC methods
	void RunSimulations( int numberOfSimulations );
	int RunSimulationOnMove( inputMove_t const& move ); //Returns result of simulation
	moveMetaData_t& GetBestMoveToSelect(); //Returns null if all nodes have been explored
	//Helper Methods
	float GetUCBValueFromMoveData( moveMetaData_t const& moveData, float explorationParameter = SQRT_2 );
	inputMove_t GetBestMove() { return m_currentBestMove; }

public:
	gamestate_t m_currentGameState;
	std::vector<moveMetaData_t> m_currentPossibleMoves;
	inputMove_t m_currentBestMove;
	float m_totalNumberOfSimulations = 0.f;
};