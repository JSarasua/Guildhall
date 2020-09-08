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

	m_screenTexture = g_theRenderer->CreateTextureFromColor( Rgba8::BLACK, IntVec2(1920,1080) );




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

	for( int rowIndex = 0; rowIndex < m_currentGameState.columns[2].size(); rowIndex++ )
	{
		int rowValue = m_currentGameState.columns[2][rowIndex];

		currentCol2Height = baseHeight + baseHeightIncr * (float)rowIndex;
		DebugAddScreenText( Vec4( col2X, currentCol2Height, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::BLUE, Rgba8::BLUE, 0.f, Stringf("%i", rowValue).c_str() );
	}

	if( IsGameStateWon( m_currentGameState ) )
	{
		DebugAddScreenText( Vec4( col0X, victoryHeight, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, "YOU WON!" );
	}



}

void Game::Render()
{
	Texture* backbuffer = g_theRenderer->GetBackBuffer();
	Texture* colorTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	
	m_camera.SetColorTarget( 0, colorTarget );

	g_theRenderer->BeginCamera(m_camera);

	g_theRenderer->DisableLight( 0 );

	Texture* testTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	g_theRenderer->BindTexture( testTexture );
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

 	m_currentGameState.columns[0].push_back( 5 );
 	m_currentGameState.columns[0].push_back( 4 );
 	m_currentGameState.columns[0].push_back( 3 );
	m_currentGameState.columns[0].push_back( 2 );
	m_currentGameState.columns[0].push_back( 1 );

	m_mcAI.SetGameState( m_currentGameState );
	m_mcAI.SetMaxDepth( 15 );
}

std::vector<inputMove_t> Game::GetValidMovesAtGameState( gameState_t const& gameState )
{
	bool isCol0NotEmpty = !gameState.columns[0].empty();
	bool isCol1NotEmpty = !gameState.columns[1].empty();
	bool isCol2NotEmpty = !gameState.columns[2].empty();

	std::vector<inputMove_t> moves;

	if( isCol0NotEmpty )
	{
		inputMove_t move1;
		inputMove_t move2;
		
		move1.m_columnToPop = 0;
		move2.m_columnToPop = 0;

		move1.m_columnToPush = 1;
		move2.m_columnToPush = 2;
		
		moves.push_back( move1 );
		moves.push_back( move2 );
	}

	if( isCol1NotEmpty )
	{
		inputMove_t move1;
		inputMove_t move2;

		move1.m_columnToPop = 1;
		move2.m_columnToPop = 1;

		move1.m_columnToPush = 0;
		move2.m_columnToPush = 2;
		
		moves.push_back( move1 );
		moves.push_back( move2 );
	}

	if( isCol2NotEmpty )
	{
		inputMove_t move1;
		inputMove_t move2;

		move1.m_columnToPop = 2;
		move2.m_columnToPop = 2;

		move1.m_columnToPush = 0;
		move2.m_columnToPush = 1;
		
		moves.push_back( move1 );
		moves.push_back( move2 );
	}

	return moves;
}

int Game::GetNumberOfValidMovesAtGameState( gameState_t const& gameState )
{
	bool isCol0NotEmpty = !gameState.columns[0].empty();
	bool isCol1NotEmpty = !gameState.columns[1].empty();
	bool isCol2NotEmpty = !gameState.columns[2].empty();

	int numberOfValidMoves = 0;

	if( isCol0NotEmpty )
	{
		numberOfValidMoves += 2;
	}

	if( isCol1NotEmpty )
	{
		numberOfValidMoves += 2;
	}

	if( isCol2NotEmpty )
	{
		numberOfValidMoves += 2;
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

// 	if( col2[0] == 5 && col2[1] == 4 && col2[2] == 3 && col2[3] == 2 && col2[4] == 1 )
// 	{
// 		return true;
// 	}

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
	if( gameState.columns[columnToPop].empty() )
	{
		return false;
	}

	return true;
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
		//m_mcAI.SetGameState( m_currentGameState );
	}

	ERROR_AND_DIE( "Invalid Move" );
}

void Game::AppendIndexedVertsTestCube( std::vector<Vertex_PCUTBN>& masterVertexList, std::vector<uint>& masterIndexList )
{
	Vec3 nonUniformScale = Vec3( 1.f, 1.f, 1.f );
	float cubeHalfHeight = 0.5f;
	float c = cubeHalfHeight;

	Vec2 bLeft( 0.f, 0.f );
	Vec2 tRight( 1.f, 1.f );
	Vec2 tLeft( 0.f, 1.f );
	Vec2 bRight( 1.f, 0.f );

	Vec3 forward	= Vec3( 0.f, 0.f, 1.f );
	Vec3 right	= Vec3( 1.f, 0.f, 0.f );
	Vec3 back		= Vec3( 0.f, 0.f, -1.f );
	Vec3 left		= Vec3( -1.f, 0.f, 0.f );
	Vec3 up		= Vec3( 0.f, 1.f, 0.f );
	Vec3 down		= Vec3( 0.f, -1.f, 0.f );

	Vertex_PCUTBN cubeVerts[24] =
	{
		//Front Quad
		//Now top
		Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE,		bLeft,  forward, right, up ),	//0
		Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE,		bRight, forward, right, up ),		//1
		Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE,		tRight, forward, right, up ),		//2


		//Vertex_PCUTBN( Vec3( -c, -c, c ), Rgba8::WHITE,		bLeft ),	//0
		//Vertex_PCUTBN( Vec3( c, c, c ), Rgba8::WHITE,		tRight ),	//2
		Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft, forward, right, up ),		//3

		//Right Quad
		//Now back
		Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft,	right, back, up ),		//4
		Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE,		bLeft, right, back, up ),		//5
		Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bRight, right, back, up ),		//6


		//Vertex_PCUTBN( Vec3( c, -c, c ), Rgba8::WHITE,		bLeft ),	//4
		//Vertex_PCUTBN( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),	//6
		Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE,		tRight, right, back, up ),		//7

		//Back Quad
		//Now bottom
		Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE,		tRight,	back, left, up ),		//8
		Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE,	tLeft, back, left, up ),		//9
		Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bLeft, back, left, up ),		//10

		//Vertex_PCUTBN( Vec3( c, -c, -c ), Rgba8::WHITE,	bLeft ),		//8
		//Vertex_PCUTBN( Vec3( -c, c, -c ), Rgba8::WHITE,	tRight ),		//10
		Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bRight, back, left, up ),		//11

		//Left Quad
		//Now Front
		Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE,	bRight,	left, forward, up ),		//12	//bLeft
		Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE,		tRight, left, forward, up ),		//13
		Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft, left, forward, up ),			//14

		//Vertex_PCUTBN( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),		//12
		//Vertex_PCUTBN( Vec3( -c, c, c ), Rgba8::WHITE,		tRight ),	//14
		Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bLeft, left, forward, up ),		//15

		//Top Quad
		//Now Left
		Vertex_PCUTBN( Vec3( -c, c, c ) * nonUniformScale, Rgba8::WHITE,		tRight,	up, right, back ),		//16
		Vertex_PCUTBN( Vec3( c, c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft, up, right, back ),		//17
		Vertex_PCUTBN( Vec3( c, c, -c ) * nonUniformScale, Rgba8::WHITE,		bLeft, up, right, back ),		//18

		//Vertex_PCUTBN( Vec3( -c, c, c ), Rgba8::WHITE,		bLeft ),	//16
		//Vertex_PCUTBN( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),	//18
		Vertex_PCUTBN( Vec3( -c, c, -c ) * nonUniformScale, Rgba8::WHITE,			bRight, up, right, back ),		//19

		//Bottom Quad
		//Now Right
		Vertex_PCUTBN( Vec3( -c, -c, -c ) * nonUniformScale, Rgba8::WHITE,	bLeft,	down, right, forward ),		//20
		Vertex_PCUTBN( Vec3( c, -c, -c ) * nonUniformScale, Rgba8::WHITE,		bRight, down, right, forward ),		//21
		Vertex_PCUTBN( Vec3( c, -c, c ) * nonUniformScale, Rgba8::WHITE,		tRight, down, right, forward ),		//22

		//Vertex_PCUTBN( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),		//20
		//Vertex_PCUTBN( Vec3( c, -c, c ), Rgba8::WHITE,		tRight ),	//22
		Vertex_PCUTBN( Vec3( -c, -c, c ) * nonUniformScale, Rgba8::WHITE,		tLeft, down, right, forward )			//23
	};

	for( int vertexIndex = 0; vertexIndex < 24; vertexIndex++ )
	{
		masterVertexList.push_back( cubeVerts[vertexIndex] );
	}

	uint cubeIndices[36] =
	{
		//Front Quad
		0,
		1,
		2,

		0,
		2,
		3,

		//Right Quad
		4,
		5,
		6,

		4,
		6,
		7,

		//Back Quad
		8,
		9,
		10,

		8 ,
		10,
		11,

		//Left Quad
		12,
		13,
		14,

		12,
		14,
		15,

		//Top Quad
		16,
		17,
		18,

		16,
		18,
		19,

		//Bottom Quad
		20,
		21,
		22,

		20,
		22,
		23
	};

	for( int indicesIndex = 0; indicesIndex < 36; indicesIndex++ )
	{
		masterIndexList.push_back( cubeIndices[indicesIndex] );
	}
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
			
			m_currentInputMove.m_columnToPop = 0;
			m_currentInputMove.m_columnToPush = 0;
			m_isInputPop = true;

			//m_mcAI.SetGameState( m_currentGameState );
		}
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
			m_mcAI.RunSimulations( 1000 );

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
		m_mcAI.SetMaxDepth( 50 );
	}
}

IntVec2 Game::GetCurrentMapBounds() const
{
	return m_world->getCurrentMapBounds();
}

void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}
