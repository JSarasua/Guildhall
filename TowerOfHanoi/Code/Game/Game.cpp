#pragma once
#include "Game/Game.hpp"
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

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;


int constexpr maxDepth = 15;

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

	InitializeGameState();

}

void Game::Shutdown()
{

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

	float baseHeight = 0.7f;
	float baseHeightIncr = 0.02f;
	float col0X = 0.01f;
	float col1X = 0.1f;
	float col2X = 0.3f;
	float victoryHeight = 0.9f;

	float currentCol0Height = baseHeight;
	float currentCol1Height = baseHeight;
	float currentCol2Height = baseHeight;

	for( int rowIndex = 0; rowIndex < m_currentGameState.columns[0].size(); rowIndex++ )
	{
		int rowValue = m_currentGameState.columns[0][rowIndex];

		currentCol0Height = baseHeight + baseHeightIncr * (float)rowIndex;
		DebugAddScreenText( Vec4( col0X, currentCol0Height, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::RED, Rgba8::RED, 0.f, Stringf("%i", rowValue).c_str() );
	}

	for( int rowIndex = 0; rowIndex < m_currentGameState.columns[1].size(); rowIndex++ )
	{
		int rowValue = m_currentGameState.columns[1][rowIndex];

		currentCol1Height = baseHeight + baseHeightIncr * (float)rowIndex;
		DebugAddScreenText( Vec4( col1X, currentCol1Height, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, Stringf("%i", rowValue).c_str() );
	}

	Rgba8 cyan( 0, 168, 255 );
	for( int rowIndex = 0; rowIndex < m_currentGameState.columns[2].size(); rowIndex++ )
	{
		int rowValue = m_currentGameState.columns[2][rowIndex];

		currentCol2Height = baseHeight + baseHeightIncr * (float)rowIndex;
		DebugAddScreenText( Vec4( col2X, currentCol2Height, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, cyan, cyan, 0.f, Stringf("%i", rowValue).c_str() );
	}

	if( IsGameStateWon( m_currentGameState ) )
	{
		DebugAddScreenText( Vec4( col0X, victoryHeight, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, "YOU WON!" );
	}
	else
	{
		DebugAddScreenText( Vec4( col0X, victoryHeight, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, Stringf( "Col to pop: %i, Col to push: %i", m_currentInputMove.m_columnToPop+1, m_currentInputMove.m_columnToPush+1 ).c_str() );
		DebugAddScreenText( Vec4( col0X, 0.85f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, Stringf( "Number Of Simulations at Head: %i", m_mcAI.GetNumberOfSimulationsAtHead() ).c_str() );
		DebugAddScreenText( Vec4( col0X, 0.8f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, Stringf( "Number Of Wins at Head: %i", m_mcAI.GetNumberOfWinsAtHead() ).c_str() );
	}




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
	m_currentGameState.columns[0].clear();
	m_currentGameState.columns[1].clear();
	m_currentGameState.columns[2].clear();

//  	m_currentGameState.columns[0].push_back( 5 );
//  	m_currentGameState.columns[0].push_back( 4 );
  	m_currentGameState.columns[0].push_back( 3 );
	m_currentGameState.columns[0].push_back( 2 );
	m_currentGameState.columns[0].push_back( 1 );

	m_mcAI.SetGameState( m_currentGameState );
	m_mcAI.SetMaxDepth( maxDepth );
}

std::vector<inputMove_t> Game::GetValidMovesAtGameState( gameState_t const& gameState )
{
	bool isCol0NotEmpty = !gameState.columns[0].empty();
	bool isCol1NotEmpty = !gameState.columns[1].empty();
	bool isCol2NotEmpty = !gameState.columns[2].empty();

	int topValueCol0 = 9999999;
	int topValueCol1 = 9999999;
	int topValueCol2 = 9999999;

	if( isCol0NotEmpty )
	{
		topValueCol0 = gameState.columns[0].back();
	}
	if( isCol1NotEmpty )
	{
		topValueCol1 = gameState.columns[1].back();
	}
	if( isCol2NotEmpty )
	{
		topValueCol2 = gameState.columns[2].back();
	}

	std::vector<inputMove_t> moves;

	if( isCol0NotEmpty )
	{
		inputMove_t move1;
		inputMove_t move2;
		
		move1.m_columnToPop = 0;
		move2.m_columnToPop = 0;

		move1.m_columnToPush = 1;
		move2.m_columnToPush = 2;
		
		if( topValueCol0 < topValueCol1 )
		{
			moves.push_back( move1 );
		}

		if( topValueCol0 < topValueCol2 )
		{
			moves.push_back( move2 );
		}
	}

	if( isCol1NotEmpty )
	{
		inputMove_t move1;
		inputMove_t move2;

		move1.m_columnToPop = 1;
		move2.m_columnToPop = 1;

		move1.m_columnToPush = 0;
		move2.m_columnToPush = 2;
		
		if( topValueCol1 < topValueCol0 )
		{
			moves.push_back( move1 );
		}

		if( topValueCol1 < topValueCol2 )
		{
			moves.push_back( move2 );
		}
	}

	if( isCol2NotEmpty )
	{
		inputMove_t move1;
		inputMove_t move2;

		move1.m_columnToPop = 2;
		move2.m_columnToPop = 2;

		move1.m_columnToPush = 0;
		move2.m_columnToPush = 1;
		
		if( topValueCol2 < topValueCol0 )
		{
			moves.push_back( move1 );
		}

		if( topValueCol2 < topValueCol1 )
		{
			moves.push_back( move2 );
		}
	}

	return moves;
}

int Game::GetNumberOfValidMovesAtGameState( gameState_t const& gameState )
{
	bool isCol0NotEmpty = !gameState.columns[0].empty();
	bool isCol1NotEmpty = !gameState.columns[1].empty();
	bool isCol2NotEmpty = !gameState.columns[2].empty();

	int topValueCol0 = 9999999;
	int topValueCol1 = 9999999;
	int topValueCol2 = 9999999;

	if( isCol0NotEmpty )
	{
		topValueCol0 = gameState.columns[0].back();
	}
	if( isCol1NotEmpty )
	{
		topValueCol1 = gameState.columns[1].back();
	}
	if( isCol2NotEmpty )
	{
		topValueCol2 = gameState.columns[2].back();
	}


	int numberOfValidMoves = 0;

	if( isCol0NotEmpty )
	{
		if( topValueCol0 < topValueCol1 )
		{
			numberOfValidMoves++;
		}

		if( topValueCol0 < topValueCol2 )
		{
			numberOfValidMoves++;
		}
	}

	if( isCol1NotEmpty )
	{
		if( topValueCol1 < topValueCol0 )
		{
			numberOfValidMoves++;
		}

		if( topValueCol1 < topValueCol2 )
		{
			numberOfValidMoves++;
		}
	}

	if( isCol2NotEmpty )
	{
		if( topValueCol2 < topValueCol0 )
		{
			numberOfValidMoves++;
		}

		if( topValueCol2 < topValueCol1 )
		{
			numberOfValidMoves++;
		}
	}

	return numberOfValidMoves;
}

bool Game::IsGameStateWon( gameState_t const& gameState )
{
	std::deque<int> const& col0 = gameState.columns[0];
	std::deque<int> const& col1 = gameState.columns[1];
	std::deque<int> const& col2 = gameState.columns[2];

	if( col0.size() != 0 || col1.size() != 0 )
	{
		return false;
	}

	int maxVal = 999999;
	for( size_t col2Index = 0; col2Index < col2.size(); col2Index++ )
	{
		int currentValue = col2[col2Index];
		if( currentValue < maxVal )
		{
			maxVal = currentValue;
		}
		else
		{
			return false;
		}
	}

	return true;
}

void Game::UpdateGameStateIfValid( inputMove_t const& inputMove )
{
	if( IsMoveValidForGameState( inputMove, m_currentGameState ) )
	{
		int valueToPop = m_currentGameState.columns[inputMove.m_columnToPop].back();
		m_currentGameState.columns[inputMove.m_columnToPop].pop_back();

		m_currentGameState.columns[inputMove.m_columnToPush].push_back( valueToPop );
	
		//m_mcAI.SetGameState( m_currentGameState );
	}
}

void Game::UpdateGameStateIfValid( inputMove_t const& inputMove, gameState_t& gameState )
{
	if( IsMoveValidForGameState( inputMove, gameState ) )
	{
		int valueToPop = gameState.columns[inputMove.m_columnToPop].back();
		gameState.columns[inputMove.m_columnToPop].pop_back();

		gameState.columns[inputMove.m_columnToPush].push_back( valueToPop );

		//m_mcAI.SetGameState( m_currentGameState );
	}
}

bool Game::IsMoveValidForGameState( inputMove_t const& inputMove, gameState_t const& gameState )
{
	int columnToPop = inputMove.m_columnToPop;
	int columnToPush = inputMove.m_columnToPush;

	if( gameState.columns[columnToPop].empty() )
	{
		return false;
	}
	
	int topValueFromColumnToPop = gameState.columns[columnToPop].back();
	int topValueFromColumnToPush = 999999; //if empty, make it a high value
	
	if( !gameState.columns[columnToPush].empty() )
	{
		topValueFromColumnToPush = gameState.columns[columnToPush].back();
	}

	//Can only push onto a column that has a larger value than the column you are popping
	if( topValueFromColumnToPop < topValueFromColumnToPush )
	{
		return true;
	}
	else
	{
		return false;
	}
}

gameState_t Game::GetGameStateFromInput( inputMove_t const& inputMove, gameState_t const& currentGameState )
{
	if( IsMoveValidForGameState( inputMove, currentGameState ) )
	{

		gameState_t newGameState = currentGameState;
		int valueToPop = newGameState.columns[inputMove.m_columnToPop].back();
		newGameState.columns[inputMove.m_columnToPop].pop_back();

		newGameState.columns[inputMove.m_columnToPush].push_back( valueToPop );


		return newGameState;
	}

	ERROR_AND_DIE( "Invalid Move" );
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
// 	const KeyButtonState& f1Key = g_theInput->GetKeyStates( F1_KEY );
// 	const KeyButtonState& f5Key = g_theInput->GetKeyStates( F5_KEY );
// 	const KeyButtonState& f6Key = g_theInput->GetKeyStates( F6_KEY );
// 	const KeyButtonState& f7Key = g_theInput->GetKeyStates( F7_KEY );
// 	const KeyButtonState& f8Key = g_theInput->GetKeyStates( F8_KEY );
// 	const KeyButtonState& f11Key = g_theInput->GetKeyStates( F11_KEY );
	const KeyButtonState& num1Key = g_theInput->GetKeyStates( '1' );
	const KeyButtonState& num2Key = g_theInput->GetKeyStates( '2' );
	const KeyButtonState& num3Key = g_theInput->GetKeyStates( '3' );
 	const KeyButtonState& num4Key = g_theInput->GetKeyStates( '4' );
// 	const KeyButtonState& num5Key = g_theInput->GetKeyStates( '5' );
// 	const KeyButtonState& num6Key = g_theInput->GetKeyStates( '6' );
// 	const KeyButtonState& num7Key = g_theInput->GetKeyStates( '7' );
// 	const KeyButtonState& num8Key = g_theInput->GetKeyStates( '8' );
// 	const KeyButtonState& num9Key = g_theInput->GetKeyStates( '9' );
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

	if( enterKey.WasJustPressed() )
	{
		if( IsMoveValidForGameState( m_currentInputMove, m_currentGameState ) )
		{
			UpdateGameStateIfValid( m_currentInputMove );
			m_mcAI.UpdateHeadNode( m_currentInputMove );

		}

		m_currentInputMove.m_columnToPop = 0;
		m_currentInputMove.m_columnToPush = 0;
		m_isInputPop = true;
	}

	if( num1Key.WasJustPressed() )
	{
		if( m_isInputPop )
		{
			m_currentInputMove.m_columnToPop = 0;
			m_isInputPop = !m_isInputPop;
		}
		else
		{
			m_currentInputMove.m_columnToPush = 0;
			m_isInputPop = !m_isInputPop;
		}
	}
	if( num2Key.WasJustPressed() )
	{
		if( m_isInputPop )
		{
			m_currentInputMove.m_columnToPop = 1;
			m_isInputPop = !m_isInputPop;
		}
		else
		{
			m_currentInputMove.m_columnToPush = 1;
			m_isInputPop = !m_isInputPop;
		}
	}
	if( num3Key.WasJustPressed() )
	{
		if( m_isInputPop )
		{
			m_currentInputMove.m_columnToPop = 2;
			m_isInputPop = !m_isInputPop;
		}
		else
		{
			m_currentInputMove.m_columnToPush = 2;
			m_isInputPop = !m_isInputPop;
		}
	}
	if( num4Key.WasJustPressed() )
	{
		//run sims
		if( !IsGameStateWon( m_currentGameState ) )
		{
			m_mcAI.RunSimulations( 2000 );

			m_mcAI.UpdateBestMove();
			inputMove_t const& bestInput = m_mcAI.GetBestMove();
			UpdateGameStateIfValid( bestInput );

			m_mcAI.UpdateHeadNode( bestInput );
			
		}

	}
	if( rKey.WasJustPressed() )
	{
		InitializeGameState();
		//m_mcAI.SetGameState( m_currentGameState );
		m_mcAI.SetMaxDepth( maxDepth );
	}
}



void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}
