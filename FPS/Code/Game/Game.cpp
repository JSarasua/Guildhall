#pragma once
#include "Game/Game.hpp"
#include "App.hpp"
#include "Game/World.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/MatrixUtils.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;




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
	m_camera.SetOutputSize( Vec2( 160.f, 90.f ) );
	m_camera.SetProjectionPerspective( 60.f, -0.1f, -100.f );
	//m_camera.SetOrthoView(Vec2(0.f, 0.f), Vec2(GAME_CAMERA_Y* CLIENT_ASPECT, GAME_CAMERA_Y));
	m_invertShader = g_theRenderer->GetOrCreateShader("Data/Shaders/InvertColor.hlsl");
	m_litShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/Phong.hlsl" );
	m_cubeModelMatrix = Mat44::CreateTranslation3D( Vec3( 1.f, 0.5f, -12.f ) );
	m_circleOfSpheresModelMatrix = Mat44::CreateTranslation3D( Vec3( 0.f, 0.f, -20.f ) );
	m_numberOfCirclingCubes = 18;

	m_cubeMesh = new GPUMesh( g_theRenderer );
// 	std::vector<Vertex_PCU> cubeVerts;
// 	std::vector<uint> cubeIndices;
// 	AppendIndexedVertsCube( cubeVerts, cubeIndices, 1.f );
// 	m_cubeMesh->UpdateVertices( cubeVerts );
// 	m_cubeMesh->UpdateIndices( cubeIndices );

	std::vector<Vertex_PCUTBN> cubeVerts;
	std::vector<uint> cubeIndices;
	Vertex_PCUTBN::AppendIndexedVertsCube( cubeVerts, cubeIndices, 1.f );
	m_cubeMesh->UpdateVertices( cubeVerts );
	m_cubeMesh->UpdateIndices( cubeIndices );




	m_sphereMesh = new GPUMesh( g_theRenderer );
// 	std::vector<Vertex_PCU> sphereVerts;
// 	std::vector<uint> sphereIndices;
// 	AppendIndexedVertsSphere( sphereVerts, sphereIndices, Vec3(0.f, 0.f, 0.f), 1.f, 64, 64, Rgba8::WHITE );
// 	m_sphereMesh->UpdateVertices( sphereVerts );
// 	m_sphereMesh->UpdateIndices( sphereIndices );

	std::vector<Vertex_PCUTBN> sphereVerts;
	std::vector<uint> sphereIndices;
	Vertex_PCUTBN::AppendIndexedVertsSphere( sphereVerts, sphereIndices, 1.f );
	m_sphereMesh->UpdateVertices( sphereVerts );
	m_sphereMesh->UpdateIndices( sphereIndices );

	m_gameClock = new Clock();
	m_gameClock->SetParent( Clock::GetMaster() );

	g_theRenderer->Setup( m_gameClock );

	m_screenTexture = g_theRenderer->CreateTextureFromColor( Rgba8::BLACK, IntVec2(1920,1080) );

	m_light = new light_t();
	m_light->color = Vec3(1.f, 1.f, 1.f);
	m_light->intensity = 1.f;
	m_light->position = m_camera.GetPosition();
}

void Game::Shutdown()
{
	delete m_cubeMesh;
	m_cubeMesh = nullptr;

	delete m_sphereMesh;
	m_sphereMesh = nullptr;
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
	clearColor.r = 10;
	clearColor.b = 10;

	m_camera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT, clearColor, 0.f, 0 );

	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses( dt );
	}


	m_cubeModelMatrix.RotateYDegrees( 45.f * dt );

	Mat44 newCircleOfSpheresRotation;
	newCircleOfSpheresRotation.RotateYDegrees( 5.f * dt );
	m_circleOfSpheresModelMatrix.RotateYDegrees( 45.f * dt );
	newCircleOfSpheresRotation.TransformBy( m_circleOfSpheresModelMatrix );
	m_circleOfSpheresModelMatrix = newCircleOfSpheresRotation;
}

void Game::Render()
{
	g_theRenderer->BeginCamera(m_camera);
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL );
	g_theRenderer->SetBlendMode(BlendMode::ADDITIVE);

	g_theRenderer->DrawAABB2(AABB2(Vec2(2.5f,0.25f), Vec2( 9.5f, 6.5f )), Rgba8::GREEN, 0.25f);

	Texture* tex = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
	g_theRenderer->SetBlendMode(BlendMode::SOLID);
	g_theRenderer->BindTexture(tex);
	g_theRenderer->BindShader(m_invertShader);
	g_theRenderer->DrawAABB2Filled(AABB2(Vec2(3.25f,1.f), Vec2( 6.f, 3.75f )), Rgba8(255, 255, 255, 128 ));

	g_theRenderer->SetBlendMode( BlendMode::ALPHA );
	g_theRenderer->BindShader( (Shader*)nullptr );
	g_theRenderer->DrawAABB2Filled( AABB2( Vec2( 6.f, 1.f ), Vec2( 8.75f, 3.75f ) ), Rgba8( 255, 255, 255 ) );


	tex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankBase.png" );
	g_theRenderer->SetBlendMode( BlendMode::SOLID );
	g_theRenderer->BindTexture( tex );
	g_theRenderer->BindShader( m_invertShader );
	g_theRenderer->DrawAABB2Filled( AABB2( Vec2( -0.5f, -0.5f ), Vec2( 0.5f, 0.5f ) ), Rgba8( 255, 255, 255, 128 ), -10.f );


	g_theRenderer->DisableLight( 0 );
	g_theRenderer->SetAmbientLight( Rgba8::WHITE, 0.2f );
	g_theRenderer->EnableLight( 0, *m_light );
	g_theRenderer->BindShader( m_litShader );
	g_theRenderer->SetModelMatrix( m_cubeModelMatrix );
	g_theRenderer->DrawMesh( m_cubeMesh );

	RenderCircleOfSpheres();

	RenderDevConsole();

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->EndCamera(m_camera);

	DebugRenderBeginFrame();
	DebugRenderWorldToCamera( &m_camera );

	DebugRenderScreenTo( g_theRenderer->GetBackBuffer() );
	DebugRenderEndFrame();

}


void Game::RenderCircleOfSpheres()
{
	float degreeIncrements = 360.f / (float)m_numberOfCirclingCubes;

	for( float currentDegreeIncrement = 0.f; currentDegreeIncrement < 360.f; currentDegreeIncrement += degreeIncrements )
	{
		g_theRenderer->SetModelMatrix( m_circleOfSpheresModelMatrix );
		g_theRenderer->DrawMesh( m_sphereMesh );

		Mat44 nextModelMatrix;
		nextModelMatrix.RotateYDegrees( degreeIncrements );
		nextModelMatrix.TransformBy( m_circleOfSpheresModelMatrix );
		m_circleOfSpheresModelMatrix = nextModelMatrix;
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

	const KeyButtonState& leftArrow = g_theInput->GetKeyStates( 0x25 );
	const KeyButtonState& upArrow = g_theInput->GetKeyStates( 0x26 );
	const KeyButtonState& rightArrow = g_theInput->GetKeyStates( 0x27 );
	const KeyButtonState& downArrow = g_theInput->GetKeyStates( 0x28 );

	const KeyButtonState& wKey = g_theInput->GetKeyStates( 'W' );
	const KeyButtonState& aKey = g_theInput->GetKeyStates( 'A' );
	const KeyButtonState& sKey = g_theInput->GetKeyStates( 'S' );
	const KeyButtonState& dKey = g_theInput->GetKeyStates( 'D' );
	const KeyButtonState& cKey = g_theInput->GetKeyStates( 'C' );
	const KeyButtonState& spaceKey = g_theInput->GetKeyStates( SPACE_KEY );
	const KeyButtonState& shiftKey = g_theInput->GetKeyStates( SHIFT_KEY );
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
	const KeyButtonState& yKey = g_theInput->GetKeyStates( 'Y' );
	const KeyButtonState& qKey = g_theInput->GetKeyStates( 'Q' );

	if( qKey.WasJustPressed() )
	{
		SetLightPosition( m_camera.GetPosition() );
	}

	if( f11Key.WasJustPressed() )
	{
		g_theWindow->ToggleBorder();
	}

	if( num1Key.WasJustPressed() )
	{
		DebugAddWorldPoint( m_camera.GetPosition(), 0.1f, Rgba8::RED, Rgba8::GREEN, 15.f, DEBUG_RENDER_ALWAYS );
	}
	if( num2Key.WasJustPressed() )
	{
		Vec3 cameraPos = m_camera.GetPosition();
		DebugAddWorldBillboardTextf( cameraPos, Vec2( 0.5f, 0.5f ), Rgba8::RED, 10.f, DEBUG_RENDER_USE_DEPTH, "Billboarded text: ( %.2f, %.2f, %.2f )", cameraPos.x, cameraPos.y, cameraPos.z );
	}
	if( num3Key.WasJustPressed() )
	{
		AABB2 aabb = DebugGetScreenBounds();
		Vec2 maxs = aabb.maxs;
		maxs -= Vec2( 60.f, 80.f );
		DebugAddScreenPoint( maxs, 50.f, Rgba8::RED, Rgba8::GREEN, 10.f );
	}
	if( num4Key.WasJustPressed() )
	{
		Mat44 cameraView = m_camera.GetViewMatrix();
		Vec3 cameraPos = m_camera.GetPosition();
		MatrixInvertOrthoNormal( cameraView );
		DebugAddWorldTextf( cameraView, Vec2( 0.5f, 0.5f ), Rgba8::RED, 10.f, DEBUG_RENDER_XRAY, "Non-billboarded text: ( %.2f, %.2f, %.2f )", cameraPos.x, cameraPos.y, cameraPos.z );
	}
	if( num5Key.WasJustPressed() )
	{
		DebugAddScreenLine( Vec2( 0.f, 0.f ), Vec2( 1920.f, 0.f ), Rgba8::GREEN, Rgba8::BLUE, 10.f );
	}
	if( num6Key.WasJustPressed() )
	{
		DebugAddScreenAABB2( AABB2( 10.f, 10.f, 200.f, 200.f ), Rgba8::RED, Rgba8::GREEN, 10.f );
	}
	if( num7Key.WasJustPressed() )
	{
		Texture* tex = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
		AABB2 aabb(1.f, 0.f, 0.f, 1.f);
		DebugAddScreenTexturedQuad( AABB2( 500.f, 300.f, 700.f, 500.f ), tex, aabb, Rgba8::WHITE, Rgba8::BLACK, 5.f );
	}
	if( num8Key.WasJustPressed() )
	{
		DebugAddWorldWireSphere( m_camera.GetPosition(), 1.f, Rgba8::RED, Rgba8::GREEN, 10.f, DEBUG_RENDER_XRAY );

	}
	if( num9Key.WasJustPressed() )
	{
		Transform transform = m_camera.GetTransform();
		transform.SetNonUniformScale( Vec3( 1.f, 1.f, 5.f ) );
		DebugAddWorldWireBounds( transform, Rgba8::BLUE, Rgba8::RED, 10.f, DEBUG_RENDER_ALWAYS );
	}
	if( num0Key.WasJustPressed() )
	{
		Vec4 pos( 1.f, 1.f, -20.f, -20.f );
		DebugAddScreenTextf( pos, Vec2( 1.f, 1.f ), 20.f, Rgba8::RED, Rgba8::GREEN, 10.f, "Screen Text Location: %.1f, %.1f, %.1f, %.1f", pos.x, pos.y, pos.z, pos.w );
	}
	if( lBracketKey.WasJustPressed() )
	{
		DebugAddScreenArrow( Vec2( 100.f, 100.f ), Vec2( 200.f, 250. ), Rgba8::RED, Rgba8::BLUE, 10.f );
	}
	if( rBracketKey.IsPressed() )
	{
		Mat44 cameraModel = m_camera.GetModelRotationMatrix();
		DebugAddScreenBasis( cameraModel, Rgba8::WHITE, Rgba8::WHITE, 0.f );
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
		//DebugAddWorldPoint( startPos, 0.1f, Rgba8::RED, Rgba8::GREEN, 15.f, DEBUG_RENDER_ALWAYS );
		//DebugAddWorldPoint( endPos, 0.1f, Rgba8::RED, Rgba8::GREEN, 15.f, DEBUG_RENDER_ALWAYS );
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
		Mat44 cameraModel = m_camera.GetCameraModelMatrix();
		DebugAddWorldBasis( cameraModel, Rgba8::WHITE, Rgba8::GREEN, 10.f, DEBUG_RENDER_USE_DEPTH );
	}
	if( gKey.WasJustPressed() )
	{
		Vec3 p0 = Vec3( -1.f,-1.f, -1.f );
		Vec3 p1 = Vec3( 1.f, -1.f, -1.f );
		Vec3 p2 = Vec3( 1.f, 3.f, -2.f );
		Vec3 p3 = Vec3( -1.f, 3.f, -2.f );

		Mat44 cameraModel = m_camera.GetCameraModelMatrix();
		p0 = cameraModel.TransformPosition3D( p0 );
		p1 = cameraModel.TransformPosition3D( p1 );
		p2 = cameraModel.TransformPosition3D( p2 );
		p3 = cameraModel.TransformPosition3D( p3 );

		DebugAddWorldQuad( p0, p1, p2, p3, Rgba8::GREEN, Rgba8::RED, 5.f, DEBUG_RENDER_XRAY );
	}
	if( yKey.WasJustPressed() )
	{
		Mat44 cameraModel = m_camera.GetCameraModelMatrix();
		DebugAddWireMeshToWorld( cameraModel, m_sphereMesh, Rgba8::RED, Rgba8::BLUE, 5.f, DEBUG_RENDER_USE_DEPTH );
	}

	Vec3 translator;

	if( wKey.IsPressed() )
	{
		translator.z -=  10.f * deltaSeconds;
	}
	if( sKey.IsPressed() )
	{
		translator.z +=  10.f * deltaSeconds;
	}
	if( aKey.IsPressed() )
	{
		translator.x -=  10.f * deltaSeconds;
	}
	if( dKey.IsPressed() )
	{
		translator.x +=  10.f * deltaSeconds;
	}
	if( cKey.IsPressed() )
	{
		translator.y +=  10.f * deltaSeconds;
	}
	if( spaceKey.IsPressed() )
	{
		translator.y -=  10.f * deltaSeconds;
	}

	if( shiftKey.IsPressed() )
	{
		translator *= 2.f;
	}

	m_camera.TranslateRelativeToView( translator );

	Vec3 rotator;
	if( upArrow.IsPressed() )
	{
		rotator.x += 10.f * deltaSeconds;
	}
	if( downArrow.IsPressed() )
	{
		rotator.x -= 10.f * deltaSeconds;
	}
	if( leftArrow.IsPressed() )
	{
		rotator.y += 10.f * deltaSeconds;
	}
	if( rightArrow.IsPressed() )
	{
		rotator.y -= 10.f * deltaSeconds;
	}

	Vec2 mouseChange = g_theInput->GetMouseDeltaPos();
// 	Vec2 mousePos = g_theInput->GetMouseNormalizedPos();
// 	mousePos.x -= 0.5f;
// 	mousePos.y -= 0.5f;

	rotator.x -= mouseChange.y * 0.1f;
	rotator.y -= mouseChange.x * 0.1f;
	m_camera.RotatePitchRollYawDegrees( rotator );

}

IntVec2 Game::GetCurrentMapBounds() const
{
	return m_world->getCurrentMapBounds();
}

void Game::SetLightPosition( Vec3 const& pos )
{
	m_light->position = pos;
}

void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}
