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
	//m_camera.SetProjectionPerspective( 60.f, -0.09f, -100.f );
	m_camera.SetProjectionOrthographic( m_camera.m_outputSize, 0.f, 100.f );


	m_frontCubeModelMatrix = Mat44::CreateTranslation3D( Vec3( 2.5f, 0.5f, 0.5f ) );
	m_leftCubeModelMatrix = Mat44::CreateTranslation3D( Vec3( 0.5f, 2.5f, 0.5f ) );
	m_frontleftCubeModelMatrix = Mat44::CreateTranslation3D( Vec3( 2.5f, 2.5f, 0.5f ) );

	m_cubeMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCUTBN> cubeVerts;
	std::vector<uint> cubeIndices;
	AppendIndexedVertsTestCube( cubeVerts, cubeIndices );

	m_cubeMesh->UpdateVertices( cubeVerts );
	m_cubeMesh->UpdateIndices( cubeIndices );


	m_gameClock = new Clock();
	m_gameClock->SetParent( Clock::GetMaster() );

	g_theRenderer->Setup( m_gameClock );

	m_screenTexture = g_theRenderer->CreateTextureFromColor( Rgba8::BLACK, IntVec2(1920,1080) );

	m_world = new World( this );
	m_world->Startup();
}

void Game::Shutdown()
{
	delete m_cubeMesh;
	m_cubeMesh = nullptr;

	m_world->Shutdown();
	delete m_world;
	m_world = nullptr;
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

	m_world->Update( dt );
	UpdateCamera( dt );
}

void Game::Render()
{
	Texture* backbuffer = g_theRenderer->GetBackBuffer();
	Texture* colorTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	m_camera.SetColorTarget( 0, colorTarget );

	g_theRenderer->BeginCamera(m_camera);
	m_world->Render();
	g_theRenderer->EndCamera(m_camera);

	RenderUI();


	g_theRenderer->CopyTexture( backbuffer, colorTarget );
	m_camera.SetColorTarget( nullptr );
	g_theRenderer->ReleaseRenderTarget( colorTarget );
	GUARANTEE_OR_DIE( g_theRenderer->GetTotalRenderTargetPoolSize() < 8, "Created too many render targets" );

	DebugRenderBeginFrame();
	DebugRenderWorldToCamera( &m_camera );
	DebugRenderScreenTo( g_theRenderer->GetBackBuffer() );
	DebugRenderEndFrame();
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

	Vec2 playerCenter = m_world->GetPlayer()->GetPosition();
	Vec2 cameraOutputSize = m_camera.GetOutputSize();
	Vec2 cameraHalfSize = 0.5f * cameraOutputSize;
	IntVec2 currentMapBounds = GetCurrentMapBounds();
	Vec2 currentMapBoundsVec2 = (Vec2)currentMapBounds;
	Vec2 mapBoundsMax = currentMapBoundsVec2 - cameraHalfSize;
	Vec2 mapBoundsMin = cameraHalfSize;
	AABB2 cameraBoundingBox = AABB2( mapBoundsMin, mapBoundsMax );
	Vec2 newCameraCenter = cameraBoundingBox.GetNearestPoint( playerCenter );
	m_camera.SetPosition( newCameraCenter );
}

void Game::RenderGame()
{
	//m_world->Render();
}

void Game::RenderUI()
{
	Player* player = m_world->GetPlayer();
	int playerHealth = player->GetCurrentHealth();
	std::string playerHealthStr = Stringf( "Player Health: %i", playerHealth );
	DebugAddScreenText( Vec4( 0.01f, 0.95f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, playerHealthStr.c_str() );
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	const XboxController& controller = g_theInput->GetXboxController(0);
	UNUSED( deltaSeconds );
	UNUSED( controller );

	const KeyButtonState& wKey = g_theInput->GetKeyStates( 'W' );
	const KeyButtonState& aKey = g_theInput->GetKeyStates( 'A' );
	const KeyButtonState& sKey = g_theInput->GetKeyStates( 'S' );
	const KeyButtonState& dKey = g_theInput->GetKeyStates( 'D' );
	//const KeyButtonState& spaceKey = g_theInput->GetKeyStates( SPACE_KEY );
	const KeyButtonState& shiftKey = g_theInput->GetKeyStates( SHIFT_KEY );
	const KeyButtonState& f1Key = g_theInput->GetKeyStates( F1_KEY );
	const KeyButtonState& f5Key = g_theInput->GetKeyStates( F5_KEY );
	const KeyButtonState& f6Key = g_theInput->GetKeyStates( F6_KEY );
	const KeyButtonState& f7Key = g_theInput->GetKeyStates( F7_KEY );
	const KeyButtonState& f8Key = g_theInput->GetKeyStates( F8_KEY );
	const KeyButtonState& f11Key = g_theInput->GetKeyStates( F11_KEY );
	const KeyButtonState& num1Key = g_theInput->GetKeyStates( '1' );
	const KeyButtonState& num2Key = g_theInput->GetKeyStates( '2' );
	const KeyButtonState& num3Key = g_theInput->GetKeyStates( '3' );
	const KeyButtonState& num4Key = g_theInput->GetKeyStates( '4' );
	const KeyButtonState& num5Key = g_theInput->GetKeyStates( '5' );
	const KeyButtonState& num6Key = g_theInput->GetKeyStates( '6' );
	const KeyButtonState& num7Key = g_theInput->GetKeyStates( '7' );
	const KeyButtonState& num8Key = g_theInput->GetKeyStates( '8' );
	const KeyButtonState& num9Key = g_theInput->GetKeyStates( '9' );
	const KeyButtonState& num0Key = g_theInput->GetKeyStates( '0' );
	const KeyButtonState& lBracketKey = g_theInput->GetKeyStates( LBRACKET_KEY );
	const KeyButtonState& rBracketKey = g_theInput->GetKeyStates( RBRACKET_KEY );
	const KeyButtonState& rKey = g_theInput->GetKeyStates( 'R' );
	const KeyButtonState& fKey = g_theInput->GetKeyStates( 'F' );
	const KeyButtonState& tKey = g_theInput->GetKeyStates( 'T' );
	const KeyButtonState& gKey = g_theInput->GetKeyStates( 'G' );
	const KeyButtonState& hKey = g_theInput->GetKeyStates( 'H' );
	//const KeyButtonState& yKey = g_theInput->GetKeyStates( 'Y' );
	const KeyButtonState& qKey = g_theInput->GetKeyStates( 'Q' );
	const KeyButtonState& vKey = g_theInput->GetKeyStates( 'V' );
	const KeyButtonState& bKey = g_theInput->GetKeyStates( 'B' );
	const KeyButtonState& nKey = g_theInput->GetKeyStates( 'N' );
	const KeyButtonState& mKey = g_theInput->GetKeyStates( 'M' );
	const KeyButtonState& uKey = g_theInput->GetKeyStates( 'U' );
	const KeyButtonState& iKey = g_theInput->GetKeyStates( 'I' );
	const KeyButtonState& jKey = g_theInput->GetKeyStates( 'J' );
	const KeyButtonState& kKey = g_theInput->GetKeyStates( 'K' );
	const KeyButtonState& zKey = g_theInput->GetKeyStates( 'Z' );
	const KeyButtonState& xKey = g_theInput->GetKeyStates( 'X' );
	const KeyButtonState& plusKey = g_theInput->GetKeyStates( PLUS_KEY );
	const KeyButtonState& minusKey = g_theInput->GetKeyStates( MINUS_KEY );
	const KeyButtonState& semiColonKey = g_theInput->GetKeyStates( SEMICOLON_KEY );
	const KeyButtonState& singleQuoteKey = g_theInput->GetKeyStates( SINGLEQUOTE_KEY );
	const KeyButtonState& commaKey = g_theInput->GetKeyStates( COMMA_KEY );
	const KeyButtonState& periodKey = g_theInput->GetKeyStates( PERIOD_KEY );

	if( jKey.WasJustPressed() )
	{

	}
	if( kKey.WasJustPressed() )
	{

	}
	if( zKey.WasJustPressed() )
	{

	}
	if( xKey.WasJustPressed() )
	{

	}
	if( uKey.IsPressed() )
	{

	}
	if( iKey.IsPressed() )
	{

	}
	if( qKey.WasJustPressed() )
	{

	}

	if( f1Key.WasJustPressed() )
	{
		SoundID soundID = g_theAudio->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
		float volume = m_rand.RollRandomFloatInRange( 0.5f, 1.f );
		float balance = m_rand.RollRandomFloatInRange( -1.f, 1.f );
		float speed = m_rand.RollRandomFloatInRange( 0.5f, 2.f );

		g_theAudio->PlaySound( soundID, false, volume, balance, speed );
	}

	if( f11Key.WasJustPressed() )
	{
		g_theWindow->ToggleBorder();
	}

	if( num1Key.IsPressed() )
	{

	}
	if( num2Key.IsPressed() )
	{

	}
	if( num3Key.IsPressed() )
	{

	}
	if( num4Key.IsPressed() )
	{

	}
	if( num5Key.IsPressed() )
	{

	}
	if( num6Key.IsPressed() )
	{

	}
	if( num7Key.IsPressed() )
	{

	}
	if( num8Key.IsPressed() )
	{

	}
	if( num9Key.IsPressed() )
	{

	}
	if( num0Key.IsPressed() )
	{

	}
	if( lBracketKey.IsPressed() )
	{

	}
	if( rBracketKey.IsPressed() )
	{

	}
	if( semiColonKey.IsPressed() )
	{

	}
	if( singleQuoteKey.IsPressed() )
	{

	}
	if( commaKey.WasJustPressed() )
	{

	}
	if( periodKey.WasJustPressed() )
	{

	}
	if( vKey.WasJustPressed() )
	{

	}
	if( bKey.WasJustPressed() )
	{

	}
	if( nKey.WasJustPressed() )
	{

	}
	if( mKey.WasJustPressed() )
	{

	}
	if( gKey.IsPressed() )
	{
		float currentGamma = g_theRenderer->GetGamma();
		float newGamma = currentGamma - 2.f * deltaSeconds;
		g_theRenderer->SetGamma( newGamma );
	}
	if( hKey.IsPressed() )
	{
		float currentGamma = g_theRenderer->GetGamma();
		float newGamma = currentGamma + 2.f * deltaSeconds;
		g_theRenderer->SetGamma( newGamma );
	}
	if( f5Key.WasJustPressed() )
	{

	}
	if( f6Key.WasJustPressed() )
	{

	}
	if( f7Key.WasJustPressed() )
	{

	}
	if( f8Key.WasJustPressed() )
	{

	}

	if( rKey.WasJustPressed() )
	{
		Vec3 startPos = m_camera.GetPosition();
		Vec3 endPos = Vec3( 0.f, 0.f, -5.f );
		Mat44 cameraModel = m_camera.GetModelRotationMatrix();

		endPos = cameraModel.TransformPosition3D( endPos );
		endPos += startPos;
		LineSegment3 line = LineSegment3( startPos, endPos );
		DebugAddWorldLine( line, Rgba8::GREEN, Rgba8::BLUE, 10.f, DEBUG_RENDER_USE_DEPTH );
	}
	if( fKey.WasJustPressed() )
	{
		Vec3 startPos = m_camera.GetPosition();
		Vec3 endPos = Vec3( 0.f, 0.f, -5.f );
		Mat44 cameraModel = m_camera.GetModelRotationMatrix();

		endPos = cameraModel.TransformPosition3D( endPos );
		endPos += startPos;
		LineSegment3 line = LineSegment3( startPos, endPos );
		DebugAddWorldArrow( line, Rgba8::GREEN, Rgba8::BLUE, 10.f, DEBUG_RENDER_USE_DEPTH );
	}
	if( tKey.WasJustPressed() )
	{

	}
	if( plusKey.IsPressed() )
	{

	}
	if( minusKey.IsPressed() )
	{

	}

	Vec3 translator;

	if( wKey.IsPressed() )
	{
		//translator.y +=  1.f * deltaSeconds;
	}
	if( sKey.IsPressed() )
	{
		//translator.y -=  1.f * deltaSeconds;
	}
	if( aKey.IsPressed() )
	{
		//translator.x -=  1.f * deltaSeconds;
	}
	if( dKey.IsPressed() )
	{
		//translator.x +=  1.f * deltaSeconds;
	}

	if( shiftKey.IsPressed() )
	{
		//translator *= 2.f;
	}

	//m_camera.TranslateRelativeToViewOnlyYaw( translator );
}

IntVec2 Game::GetCurrentMapBounds() const
{
	return m_world->getCurrentMapBounds();
}
