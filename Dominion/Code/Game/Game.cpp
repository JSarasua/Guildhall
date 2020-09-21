#pragma once
#include "Game/Game.hpp"
#include "Game/MonteCarlo.hpp"
#include "Engine/Math/AABB2.hpp"
#include "App.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Game/Player.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/World.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/Vec4.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;


int constexpr maxDepth = 15;
int constexpr g_WhoStarts = PLAYER_2;

Game::Game()
{
	//m_world = new World(this);
}

Game::~Game(){}

void Game::Startup()
{
	EnableDebugRendering();
	m_camera = Camera();
	m_camera.SetColorTarget(nullptr); // we use this
	m_camera.CreateMatchingDepthStencilTarget( g_theRenderer );
	m_camera.SetOutputSize( Vec2( 16.f, 9.f ) );
	m_camera.SetProjectionPerspective( 60.f, -0.09f, -100.f );
	m_camera.Translate( Vec3( -2.f, 0.f, 1.5f ) );


	m_gameClock = new Clock();
	m_gameClock->SetParent( Clock::GetMaster() );

	g_theRenderer->Setup( m_gameClock );

	m_mcts = new MonteCarlo();
	//m_mcts->Startup( CIRCLEPLAYER );
	InitializeGameState();
}

void Game::Shutdown()
{
	m_mcts->Shutdown();
}

void Game::RunFrame(){}

void Game::Update()
{
	float dt = (float)m_gameClock->GetLastDeltaSeconds();

	m_currentTime += dt;
	if( m_currentTime > 255.f )
	{
		m_currentTime = 0.f;
	}
	Rgba8 clearColor;

	clearColor.g = 0;
	clearColor.r = 0;
	clearColor.b = 0;

	m_camera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT, clearColor, 0.f, 0 );

	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses( dt );
	}

// 	float baseHeight = 0.7f;
// 	float baseHeightIncr = 0.02f;
// 	float col0X = 0.01f;
// 	float col1X = 0.1f;
// 	float col2X = 0.3f;
// 	float victoryHeight = 0.9f;
// 
// 	float currentCol0Height = baseHeight;
// 	float currentCol1Height = baseHeight;
// 	float currentCol2Height = baseHeight;
	Vec2 winOffset = Vec2(0.0f, -0.02f );
	Vec2 simOffset = Vec2(0.0f, -0.03f );
	Vec2 winPercentOffset = Vec2( 0.0f, -0.04f );


	AABB2 gameBoard = DebugGetScreenBounds();
	Vec2 gameDims = gameBoard.GetDimensions();
	gameDims.y = gameDims.x;
	gameDims *= 0.5f;
	gameBoard.SetDimensions( gameDims );

	AABB2 carvingBoard = gameBoard;

	AABB2 gameTopThird = carvingBoard.CarveBoxOffTop( 0.333f );
	AABB2 gameTopLeft = gameTopThird.CarveBoxOffLeft( 0.333f );
	AABB2 gameTopMiddle = gameTopThird.CarveBoxOffLeft( 0.5f );
	AABB2 gameTopRight = gameTopThird;

	AABB2 gameMiddleThird = carvingBoard.CarveBoxOffTop( 0.5f );
	AABB2 gameMiddleLeft = gameMiddleThird.CarveBoxOffLeft( 0.333f );
	AABB2 gameMiddleMiddle = gameMiddleThird.CarveBoxOffLeft( 0.5f );
	AABB2 gameMiddleRight = gameMiddleThird;

	AABB2 gameBottomThird = carvingBoard;
	AABB2 gameBottomLeft = gameBottomThird.CarveBoxOffLeft( 0.333f );
	AABB2 gameBottomMiddle = gameBottomThird.CarveBoxOffLeft( 0.5f );
	AABB2 gameBottomRight = gameBottomThird;


	DebugAddScreenAABB2( gameTopLeft,		Rgba8( 0, 170, 255 ) );
	DebugAddScreenAABB2( gameTopMiddle,		Rgba8( 50, 205, 50 ) );
	DebugAddScreenAABB2( gameTopRight,		Rgba8( 0, 170, 255 ) );
	DebugAddScreenAABB2( gameMiddleLeft,	Rgba8( 50, 205, 50 ) );
	DebugAddScreenAABB2( gameMiddleMiddle,	Rgba8( 0, 170, 255 ) );
	DebugAddScreenAABB2( gameMiddleRight,	Rgba8( 50, 205, 50 ) );
	DebugAddScreenAABB2( gameBottomLeft,	Rgba8( 0, 170, 255 ) );
	DebugAddScreenAABB2( gameBottomMiddle,	Rgba8( 50, 205, 50 ) );
	DebugAddScreenAABB2( gameBottomRight,	Rgba8( 0, 170, 255 ) );

	//CenterOfTiles
	Vec4 positionArr[] = {
	Vec4( Vec2(),		gameTopLeft.GetCenter() ),
	Vec4( Vec2(),		gameTopMiddle.GetCenter() ),
	Vec4( Vec2(),		gameTopRight.GetCenter() ),
	Vec4( Vec2(),	gameMiddleLeft.GetCenter() ),
	Vec4( Vec2(),	gameMiddleMiddle.GetCenter() ),
	Vec4( Vec2(),	gameMiddleRight.GetCenter() ),
	Vec4( Vec2(),	gameBottomLeft.GetCenter() ),
	Vec4( Vec2(),	gameBottomMiddle.GetCenter() ),
	Vec4( Vec2(),	gameBottomRight.GetCenter() )
};
	float fontSize = 100.f;
	float metaFontSize = 10.f;

	int positionIndex = 0;
	int const* gameArray = m_currentGameState.gameArray;
	while( positionIndex < 9 )
	{
		Vec4 const& position = positionArr[positionIndex];
		if( gameArray[positionIndex] == PLAYER_1 )
		{
			DebugAddScreenText( position, Vec2( 0.5f, 0.5f ), fontSize, Rgba8::RED, Rgba8::RED, 0.f, "O" );
		}
		else if( gameArray[positionIndex] == PLAYER_2 )
		{
			DebugAddScreenText( position, Vec2( 0.5f, 0.5f ), fontSize, Rgba8::RED, Rgba8::RED, 0.f, "X" );
		}

		positionIndex++;
	}

	TreeNode const* headNode = m_mcts->GetCurrentHeadNode();
	for( size_t childIndex = 0; childIndex < headNode->m_childNodes.size(); childIndex++ )
	{
		data_t const* data = headNode->m_childNodes[childIndex]->m_data;
		if( nullptr == data )
		{
			continue;
		}
		int move = data->m_moveToReachNode.m_move;
		float wins = data->m_metaData.m_numberOfWins;
		int sims = data->m_metaData.m_numberOfSimulations;
		float winPercent = wins / (float)sims;
		Vec4 position = positionArr[move];
		Vec4 winPosition = position;
		Vec4 simPosition = position;
		Vec4 winPercentPosition = position;

		winPosition.x += winOffset.x;
		winPosition.y += winOffset.y;
		simPosition.x += simOffset.x;
		simPosition.y += simOffset.y;
		winPercentPosition.x += winPercentOffset.x;
		winPercentPosition.y += winPercentOffset.y;

		DebugAddScreenText( winPosition, Vec2( 0.f, 0.f ), metaFontSize, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Wins: %.1f", wins ).c_str() );
		DebugAddScreenText( simPosition, Vec2( 0.f, 0.f ), metaFontSize, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Sims: %i", sims ).c_str() );
		DebugAddScreenText( winPercentPosition, Vec2( 0.f, 0.f ), metaFontSize, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Win Rate: %.1f", winPercent ).c_str() );


	}


	if( IsGameOver() == PLAYER_1 )
	{
		DebugAddScreenText( Vec4( 0.f, 0.8f, 0.f, 0.f ), Vec2(), 20.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Game Over: O won!" ).c_str() );
	}
	else if( IsGameOver() == PLAYER_2 )
	{
		DebugAddScreenText( Vec4( 0.f, 0.8f, 0.f, 0.f ), Vec2(), 20.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Game Over: X won!" ).c_str() );
	}
	else if( IsGameOver() == TIE )
	{
		DebugAddScreenText( Vec4( 0.f, 0.8f, 0.f, 0.f ), Vec2(), 20.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf( "Game Over: Tie!" ).c_str() );
	}

// 	DebugAddScreenText( topLeft, Vec2( 0.5f, 0.5f ), fontSize, Rgba8::RED, Rgba8::RED, 0.f, "X" );
// 	DebugAddScreenText( topMiddle, Vec2( 0.5f, 0.5f ), fontSize, Rgba8::RED, Rgba8::RED, 0.f, "O" );
// 	DebugAddScreenText( topRight, Vec2( 0.5f, 0.5f ), fontSize, Rgba8::RED, Rgba8::RED, 0.f, "X" );
// 	DebugAddScreenText( middleLeft, Vec2( 0.5f, 0.5f ), fontSize, Rgba8::RED, Rgba8::RED, 0.f, "O" );
// 	DebugAddScreenText( middleMiddle, Vec2( 0.5f, 0.5f ), fontSize, Rgba8::RED, Rgba8::RED, 0.f, "X" );
// 	DebugAddScreenText( middleRight, Vec2( 0.5f, 0.5f ), fontSize, Rgba8::RED, Rgba8::RED, 0.f, "X" );
// 	DebugAddScreenText( bottomLeft, Vec2( 0.5f, 0.5f ), fontSize, Rgba8::RED, Rgba8::RED, 0.f, "X" );
// 	DebugAddScreenText( bottomMiddle, Vec2( 0.5f, 0.5f ), fontSize, Rgba8::RED, Rgba8::RED, 0.f, "X" );
// 	DebugAddScreenText( bottomRight, Vec2( 0.5f, 0.5f ), fontSize, Rgba8::RED, Rgba8::RED, 0.f, "X" );



}

void Game::Render()
{
	Texture* backbuffer = g_theRenderer->GetBackBuffer();
	Texture* colorTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	
	m_camera.SetColorTarget( 0, colorTarget );

	g_theRenderer->BeginCamera(m_camera);

	g_theRenderer->DisableLight( 0 );

// 	Texture* testTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
// 	g_theRenderer->BindTexture( testTexture );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->EndCamera(m_camera);

	DebugRenderBeginFrame();
	DebugRenderWorldToCamera( &m_camera );


	
	g_theRenderer->CopyTexture( backbuffer, colorTarget );

	m_camera.SetColorTarget( nullptr );
	g_theRenderer->ReleaseRenderTarget( colorTarget );


	GUARANTEE_OR_DIE( g_theRenderer->GetTotalRenderTargetPoolSize() < 8, "Created too many render targets" );

	DebugRenderScreenTo( g_theRenderer->GetBackBuffer() );
	DebugRenderEndFrame();
}

void Game::InitializeGameState()
{
	//Setup gamestate
	memset( m_currentGameState.gameArray, 0, 9 * sizeof(int) );
	m_currentGameState.whoseMoveIsIt = g_WhoStarts;

	m_mcts->Shutdown();
	m_mcts->Startup( g_WhoStarts );

}

void Game::CheckCollisions()
{}

void Game::UpdateEntities( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::RenderGame()
{
	//m_world->Render();
}

void Game::RenderUI()
{
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	const XboxController& controller = g_theInput->GetXboxController(0);
	UNUSED( deltaSeconds );
	UNUSED( controller );

// 	const KeyButtonState& leftArrow = g_theInput->GetKeyStates( 0x25 );
// 	const KeyButtonState& upArrow = g_theInput->GetKeyStates( 0x26 );
// 	const KeyButtonState& rightArrow = g_theInput->GetKeyStates( 0x27 );
// 	const KeyButtonState& downArrow = g_theInput->GetKeyStates( 0x28 );

// 	const KeyButtonState& wKey = g_theInput->GetKeyStates( 'W' );
// 	const KeyButtonState& aKey = g_theInput->GetKeyStates( 'A' );
// 	const KeyButtonState& sKey = g_theInput->GetKeyStates( 'S' );
// 	const KeyButtonState& dKey = g_theInput->GetKeyStates( 'D' );
// 	const KeyButtonState& cKey = g_theInput->GetKeyStates( 'C' );
// 	const KeyButtonState& spaceKey = g_theInput->GetKeyStates( SPACE_KEY );
// 	const KeyButtonState& shiftKey = g_theInput->GetKeyStates( SHIFT_KEY );
 	const KeyButtonState& f1Key = g_theInput->GetKeyStates( F1_KEY );
	const KeyButtonState& f2Key = g_theInput->GetKeyStates( F2_KEY );
	const KeyButtonState& f3Key = g_theInput->GetKeyStates( F3_KEY );
// 	const KeyButtonState& f5Key = g_theInput->GetKeyStates( F5_KEY );
// 	const KeyButtonState& f6Key = g_theInput->GetKeyStates( F6_KEY );
// 	const KeyButtonState& f7Key = g_theInput->GetKeyStates( F7_KEY );
// 	const KeyButtonState& f8Key = g_theInput->GetKeyStates( F8_KEY );
// 	const KeyButtonState& f11Key = g_theInput->GetKeyStates( F11_KEY );
	const KeyButtonState& num1Key = g_theInput->GetKeyStates( '1' );
	const KeyButtonState& num2Key = g_theInput->GetKeyStates( '2' );
	const KeyButtonState& num3Key = g_theInput->GetKeyStates( '3' );
 	const KeyButtonState& num4Key = g_theInput->GetKeyStates( '4' );
 	const KeyButtonState& num5Key = g_theInput->GetKeyStates( '5' );
 	const KeyButtonState& num6Key = g_theInput->GetKeyStates( '6' );
 	const KeyButtonState& num7Key = g_theInput->GetKeyStates( '7' );
 	const KeyButtonState& num8Key = g_theInput->GetKeyStates( '8' );
	const KeyButtonState& num9Key = g_theInput->GetKeyStates( '9' );
// 	const KeyButtonState& num0Key = g_theInput->GetKeyStates( '0' );
// 	const KeyButtonState& lBracketKey = g_theInput->GetKeyStates( LBRACKET_KEY );
// 	const KeyButtonState& rBracketKey = g_theInput->GetKeyStates( RBRACKET_KEY );
 	const KeyButtonState& rKey = g_theInput->GetKeyStates( 'R' );
// 	const KeyButtonState& fKey = g_theInput->GetKeyStates( 'F' );
// 	const KeyButtonState& tKey = g_theInput->GetKeyStates( 'T' );
// 	const KeyButtonState& gKey = g_theInput->GetKeyStates( 'G' );
// 	const KeyButtonState& hKey = g_theInput->GetKeyStates( 'H' );
// 	//const KeyButtonState& yKey = g_theInput->GetKeyStates( 'Y' );
// 	const KeyButtonState& qKey = g_theInput->GetKeyStates( 'Q' );
// 	const KeyButtonState& vKey = g_theInput->GetKeyStates( 'V' );
// 	const KeyButtonState& bKey = g_theInput->GetKeyStates( 'B' );
// 	const KeyButtonState& nKey = g_theInput->GetKeyStates( 'N' );
// 	const KeyButtonState& mKey = g_theInput->GetKeyStates( 'M' );
// 	const KeyButtonState& uKey = g_theInput->GetKeyStates( 'U' );
// 	const KeyButtonState& iKey = g_theInput->GetKeyStates( 'I' );
// 	const KeyButtonState& jKey = g_theInput->GetKeyStates( 'J' );
// 	const KeyButtonState& kKey = g_theInput->GetKeyStates( 'K' );
// 	const KeyButtonState& zKey = g_theInput->GetKeyStates( 'Z' );
// 	const KeyButtonState& xKey = g_theInput->GetKeyStates( 'X' );
// 	const KeyButtonState& plusKey = g_theInput->GetKeyStates( PLUS_KEY );
// 	const KeyButtonState& minusKey = g_theInput->GetKeyStates( MINUS_KEY );
// 	const KeyButtonState& semiColonKey = g_theInput->GetKeyStates( SEMICOLON_KEY );
// 	const KeyButtonState& singleQuoteKey = g_theInput->GetKeyStates( SINGLEQUOTE_KEY );
// 	const KeyButtonState& commaKey = g_theInput->GetKeyStates( COMMA_KEY );
// 	const KeyButtonState& periodKey = g_theInput->GetKeyStates( PERIOD_KEY );
	const KeyButtonState& enterKey = g_theInput->GetKeyStates( ENTER_KEY );

	if( f1Key.WasJustPressed() )
	{
		m_mcts->RunSimulations( 1000 );

	}
	if( f2Key.WasJustPressed() )
	{
		inputMove_t move = m_mcts->GetBestMove();

		PlayMoveIfValid( move.m_move );
	}
	if( f3Key.WasJustPressed() )
	{
		m_mcts->RunSimulations( 1 );
	}
	if( enterKey.WasJustPressed() )
	{
		
	}

	if( num1Key.WasJustPressed() )
	{
		//PlayMoveIfValid( TOPLEFT );
	}
	if( num2Key.WasJustPressed() )
	{
		//PlayMoveIfValid( TOPMIDDLE );
	}
	if( num3Key.WasJustPressed() )
	{
		//PlayMoveIfValid( TOPRIGHT );
	}
	if( num4Key.WasJustPressed() )
	{
		//PlayMoveIfValid( MIDDLELEFT );
	}
	if( num5Key.WasJustPressed() )
	{
		//PlayMoveIfValid( MIDDLEMIDDLE );
	}
	if( num6Key.WasJustPressed() )
	{
		//PlayMoveIfValid( MIDDLERIGHT );
	}
	if( num7Key.WasJustPressed() )
	{		
		//PlayMoveIfValid( BOTTOMLEFT );
	}
	if( num8Key.WasJustPressed() )
	{
		//PlayMoveIfValid( BOTTOMMIDDLE );
	}
	if( num9Key.WasJustPressed() )
	{
		//PlayMoveIfValid( BOTTOMRIGHT );
	}
	if( rKey.WasJustPressed() )
	{
		InitializeGameState();
		//reset ai
	}
}



void Game::PlayMoveIfValid( int moveToPlay )
{
	if( IsGameOver() != 0 )
	{
		return;
	}
	if( IsMoveValid( moveToPlay ) )
	{
		int whoseMoveIsIt = m_currentGameState.whoseMoveIsIt;
		m_currentGameState.gameArray[moveToPlay] = whoseMoveIsIt;
		if( whoseMoveIsIt == PLAYER_1 )
		{
			m_currentGameState.whoseMoveIsIt = PLAYER_2;
		}
		else
		{
			m_currentGameState.whoseMoveIsIt = PLAYER_1;
		}
	}
	inputMove_t move = inputMove_t(moveToPlay);

	m_mcts->UpdateGame( move, m_currentGameState );
}

bool Game::IsMoveValid( int moveToPlay )
{
	return IsMoveValidForGameState( moveToPlay, m_currentGameState );
}

bool Game::IsMoveValidForGameState( int moveToPlay, gamestate_t const& gameState )
{
	if( moveToPlay > -1 && moveToPlay < 9 )
	{
		int gamespot = gameState.gameArray[moveToPlay];

		if( gamespot == 0 )
		{
			return true;
		}
	}

	return false;
}

int Game::IsGameOverForGameState( gamestate_t const& gameState )
{
	int const* gameArray = gameState.gameArray;
	if( (gameArray[0] == PLAYER_1 && gameArray[0] == gameArray[1] && gameArray[1] == gameArray[2]) || //top row
		(gameArray[3] == PLAYER_1 && gameArray[3] == gameArray[4] && gameArray[4] == gameArray[5]) || //middle row
		(gameArray[6] == PLAYER_1 && gameArray[6] == gameArray[7] && gameArray[7] == gameArray[8]) || //bottom row
		(gameArray[0] == PLAYER_1 && gameArray[0] == gameArray[3] && gameArray[3] == gameArray[6]) || //left column
		(gameArray[1] == PLAYER_1 && gameArray[1] == gameArray[4] && gameArray[4] == gameArray[7]) || //middle column
		(gameArray[2] == PLAYER_1 && gameArray[2] == gameArray[5] && gameArray[5] == gameArray[8]) || //right column
		(gameArray[0] == PLAYER_1 && gameArray[0] == gameArray[4] && gameArray[4] == gameArray[8]) || //topleft Cross
		(gameArray[6] == PLAYER_1 && gameArray[6] == gameArray[4] && gameArray[4] == gameArray[2]) )  //bottomLeft Cross
	{
		return PLAYER_1;
	}

	if( (gameArray[0] == PLAYER_2 && gameArray[0] == gameArray[1] && gameArray[1] == gameArray[2]) || //top row
		(gameArray[3] == PLAYER_2 && gameArray[3] == gameArray[4] && gameArray[4] == gameArray[5]) || //middle row
		(gameArray[6] == PLAYER_2 && gameArray[6] == gameArray[7] && gameArray[7] == gameArray[8]) || //bottom row
		(gameArray[0] == PLAYER_2 && gameArray[0] == gameArray[3] && gameArray[3] == gameArray[6]) || //left column
		(gameArray[1] == PLAYER_2 && gameArray[1] == gameArray[4] && gameArray[4] == gameArray[7]) || //middle column
		(gameArray[2] == PLAYER_2 && gameArray[2] == gameArray[5] && gameArray[5] == gameArray[8]) || //right column
		(gameArray[0] == PLAYER_2 && gameArray[0] == gameArray[4] && gameArray[4] == gameArray[8]) || //topleft Cross
		(gameArray[6] == PLAYER_2 && gameArray[6] == gameArray[4] && gameArray[4] == gameArray[2]) )  //bottomLeft Cross
	{
		return PLAYER_2;
	}

	if( GetValidMovesAtGameState( gameState ).size() == 0 )
	{
		return TIE;
	}

	return 0;
}

int Game::IsGameOver()
{
	return IsGameOverForGameState( m_currentGameState );
}



std::vector<int> Game::GetValidMovesAtGameState( gamestate_t const& gameState )
{
	std::vector<int> validMoves;

	int const* gameArray = gameState.gameArray;
	int stateIndex = 0;
	while ( stateIndex < 9 )
	{
		if( gameArray[stateIndex] == 0 )
		{
			validMoves.push_back( stateIndex );
		}
		stateIndex++;
	}

	return validMoves;
}

int Game::GetNumberOfValidMovesAtGameState( gamestate_t const& gameState )
{
	int numberOfValidMoves = 0;

	int const* gameArray = gameState.gameArray;
	int stateIndex = 0;
	while( stateIndex < 9 )
	{
		if( gameArray[stateIndex] == 0 )
		{
			numberOfValidMoves++;
		}
		stateIndex++;
	}

	return numberOfValidMoves;
}

gamestate_t Game::GetGameStateAfterMove( gamestate_t const& currentGameState, inputMove_t const& move )
{
	gamestate_t newGameState = currentGameState;
	int whoseMoveIsIt = newGameState.whoseMoveIsIt;
	int moveToMake = move.m_move;
	if( IsMoveValidForGameState( move.m_move, newGameState ) )
	{
		newGameState.gameArray[moveToMake] = whoseMoveIsIt;

		if( whoseMoveIsIt == PLAYER_1 )
		{
			newGameState.whoseMoveIsIt = PLAYER_2;
		}
		else
		{
			newGameState.whoseMoveIsIt = PLAYER_1;
		}
	}

	return newGameState;
}

inputMove_t Game::GetRandomMoveAtGameState( gamestate_t const& currentGameState )
{
	std::vector<int> validMoves = GetValidMovesAtGameState( currentGameState );

	int randIndex = m_rand.RollRandomIntInRange( 0, (int)validMoves.size() - 1 );
	int randMove = validMoves[randIndex];
	inputMove_t randMoveStruct = inputMove_t(randMove);

	return randMoveStruct;
}

void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}
