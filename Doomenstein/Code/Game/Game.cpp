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
#include "Game/MapRegionType.hpp"
#include "Game/MapMaterialType.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;


//light_t Game::m_pointLight;
std::vector<light_t> Game::m_lights;
uint Game::m_currentLightIndex = 0;

Game::Game()
{
	//m_world = new World(this);
}

Game::~Game(){}

void Game::Startup()
{
	LoadAssets();
	InitializeDefinitions();

	EnableDebugRendering();
	m_camera = Camera();
	m_camera.SetColorTarget(nullptr); // we use this
	m_camera.CreateMatchingDepthStencilTarget( g_theRenderer );
	m_camera.SetOutputSize( Vec2( 16.f, 9.f ) );
	m_camera.SetProjectionPerspective( 60.f, -0.09f, -100.f );
	m_camera.Translate( Vec3( -2.f, 0.f, 1.5f ) );


	Vec2 UIdimension = Vec2( 16.f, 9.f );
	m_UICamera = Camera();
	m_UICamera.SetColorTarget( g_theRenderer->GetBackBuffer() );
	m_UICamera.SetClearMode( NO_CLEAR, Rgba8::BLACK );
	m_UICamera.SetProjectionOrthographic( UIdimension, 0.f, 100.f );
	m_UICamera.SetPosition( Vec3( 0.5f * UIdimension ) );
	m_UICamera.m_cameraType = SCREENCAMERA;


	m_frontCubeModelMatrix = Mat44::CreateTranslation3D( Vec3( 2.5f, 0.5f, 0.5f ) );
	m_leftCubeModelMatrix = Mat44::CreateTranslation3D( Vec3( 0.5f, 2.5f, 0.5f ) );
	m_frontleftCubeModelMatrix = Mat44::CreateTranslation3D( Vec3( 2.5f, 2.5f, 0.5f ) );

	m_cubeMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCUTBN> cubeVerts;
	std::vector<uint> cubeIndices;
	//Vertex_PCUTBN::AppendIndexedVertsCube( cubeVerts, cubeIndices, 0.5f );
	AppendIndexedVertsTestCube( cubeVerts, cubeIndices );

	m_cubeMesh->UpdateVertices( cubeVerts );
	m_cubeMesh->UpdateIndices( cubeIndices );


	m_gameClock = new Clock();
	m_gameClock->SetParent( Clock::GetMaster() );

	g_theRenderer->Setup( m_gameClock );

	m_screenTexture = g_theRenderer->CreateTextureFromColor( Rgba8::BLACK, IntVec2(1920,1080) );



	Vec3 cameraDirection = m_camera.GetDirection();
	for( uint lightIndex = 0; lightIndex < MAX_LIGHTS; lightIndex++ )
	{
		light_t lightData;
		lightData.color = Vec3( 1.f, 1.f, 1.f );
		lightData.intensity = 0.f;
		lightData.position = m_camera.GetPosition();
		lightData.cosInnerAngle = -0.98f;
		lightData.cosOuterAngle = -1.f;
		lightData.direction = cameraDirection;
		m_lights.push_back( lightData );
	}
	m_lights[0].intensity = 0.5f;
	m_lights[0].position.z -= 2.f;
	m_lights[0].color = Vec3( 1.f, 1.f, 1.f );



	g_theRenderer->SetSpecularFactorAndPower( 0.5f, 2.f );

	g_theRenderer->SetAmbientLight( Rgba8::WHITE, 0.f );

	g_theEventSystem->SubscribeToEvent( "light_set_ambient_color", CONSOLECOMMAND, SetAmbientColor );
	g_theEventSystem->SubscribeToEvent( "light_set_attenuation", CONSOLECOMMAND, SetAttenuation );
	g_theEventSystem->SubscribeToEvent( "light_set_color", CONSOLECOMMAND, SetLightColor );

	SoundID soundID = g_theAudio->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
	float volume = m_rand.RollRandomFloatInRange( 0.5f, 1.f );
	float balance = m_rand.RollRandomFloatInRange( -1.f, 1.f );
	float speed = m_rand.RollRandomFloatInRange( 0.5f, 2.f );
	g_theAudio->PlaySound( soundID, false, volume, balance, speed );

	FPSStartup();

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

	delete m_viewModelsSpriteSheet;
	m_viewModelsSpriteSheet = nullptr;
}

void Game::RunFrame(){}

void Game::InitializeDefinitions()
{
	XmlDocument mapMaterialDoc;
	XmlDocument mapRegionDoc;

	XmlElement const& mapMaterialTypeRoot = GetRootElement( mapMaterialDoc, "Data/Definitions/MapMaterialTypes.xml" );
	XmlElement const& mapRegionTypeRoot = GetRootElement( mapRegionDoc, "Data/Definitions/MapRegionTypes.xml" );

	MapMaterialType::InitializeMapMaterialDefinitions( mapMaterialTypeRoot );
	MapRegionType::InitializeMapRegionDefinitions( mapRegionTypeRoot );
}

void Game::Update()
{
	FPSCounterUpdate();
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
	
	m_world->Update( dt );

	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses( dt );
	}

	DebugAddWorldBasis( Mat44(), 0.f, DEBUG_RENDER_ALWAYS );
	Mat44 cameraModelMatrix = Mat44();
	cameraModelMatrix.ScaleUniform3D( 0.01f );
	Vec3 compassPosition = m_camera.GetPosition();
	compassPosition += m_camera.GetDirection() * 0.1f;

	cameraModelMatrix.SetTranslation3D( compassPosition );
	DebugAddWorldBasis( cameraModelMatrix, 0.f, DEBUG_RENDER_ALWAYS );
	//DebugAddScreenBasis( m_camera.GetModelRotationMatrix(), Rgba8::WHITE, Rgba8::WHITE, 0.f );

	//GREY: Playing (UI)
	//YELLOW: Camera Yaw=X.X	Pitch=X.X	Roll=X.X	xyz=(X.XX, Y.YY, Z.ZZ)
	//RED: iBasis (forward,	+x world-east when identity): (X.XX, Y.YY, Z.ZZ)
	//GREEN: jBasis (left,	+y world-north when identity):	(X.XX, Y.YY, Z.ZZ)
	//BLUE: kBasis (up, +z world-up when identity):		(X.XX, Y.YY, Z.ZZ)

	Vec3 cameraPitchRollYaw = m_camera.GetRotation();
	Vec3 cameraPosition = m_camera.GetPosition();
	float yaw = cameraPitchRollYaw.z;
	float pitch = cameraPitchRollYaw.x;
	float roll = cameraPitchRollYaw.y;
	Mat44 cameraBases = m_camera.GetCameraScreenRotationMatrix();
	Vec3 cameraIBasis = cameraBases.GetIBasis3D();
	Vec3 cameraJBasis = cameraBases.GetJBasis3D();
	Vec3 cameraKBasis = cameraBases.GetKBasis3D();

 	std::string playingUIStr = Stringf( "Playing (UI)");
	std::string cameraRotationTranslationStr = Stringf( "Camera Yaw = %.1f	Pitch=%.1f	Roll=%.1f	xyz=(%.2f, %.2f, %.2f)", yaw, pitch, roll, cameraPosition.x, cameraPosition.y, cameraPosition.z );;
	std::string cameraIBasisStr = Stringf( "iBasis (forward, +x world-east when identity): (%.2f, %.2f, %.2f)", cameraIBasis.x, cameraIBasis.y, cameraIBasis.z );
	std::string cameraJBasisStr = Stringf( "jBasis (left,	+y world-north when identity):	(%.2f, %.2f, %.2f)", cameraJBasis.x, cameraJBasis.y, cameraJBasis.z );
	std::string cameraKBasisStr = Stringf( "kBasis (up, +z world-up when identity):		(%.2f, %.2f, %.2f)", cameraKBasis.x, cameraKBasis.y, cameraKBasis.z );

 	DebugAddScreenText( Vec4( 0.01f, 0.95f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, playingUIStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.93f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::YELLOW, Rgba8::YELLOW, 0.f, cameraRotationTranslationStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.91f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::RED, Rgba8::RED, 0.f, cameraIBasisStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.89f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, cameraJBasisStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.87f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::BLUE, Rgba8::BLUE, 0.f, cameraKBasisStr.c_str() );
}

void Game::Render()
{
	Texture* backbuffer = g_theRenderer->GetBackBuffer();
	Texture* colorTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	Texture* bloomTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	Texture* albedoTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	Texture* normalTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	Texture* tangentTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	
	m_camera.SetColorTarget( 0, colorTarget );
	m_camera.SetColorTarget( 1, bloomTarget );
	m_camera.SetColorTarget( 2, normalTarget );
	m_camera.SetColorTarget( 3, albedoTarget );
	m_camera.SetColorTarget( 4, tangentTarget );

	g_theRenderer->BeginCamera(m_camera);

// 	Texture* testTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
// 	g_theRenderer->BindTexture( testTexture );
// 	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
// 	g_theRenderer->SetModelMatrix( m_frontCubeModelMatrix );
// 
// 	Shader* shader = g_theRenderer->GetOrCreateShader(  "Data/Shaders/WorldOpaque.hlsl" );
// 	g_theRenderer->BindShader( shader );
// 	g_theRenderer->DrawMesh( m_cubeMesh );
// 
// 	g_theRenderer->SetModelMatrix( m_leftCubeModelMatrix );
// 	g_theRenderer->DrawMesh( m_cubeMesh );
// 
// 	g_theRenderer->SetModelMatrix( m_frontleftCubeModelMatrix );
// 	g_theRenderer->DrawMesh( m_cubeMesh );

	m_world->Render();

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->EndCamera(m_camera);


	//g_theRenderer->BeginCamera( m_UICamera );
	//g_theRenderer->EndCamera( m_UICamera );

	DebugRenderBeginFrame();
	DebugRenderWorldToCamera( &m_camera );
	
	g_theRenderer->CopyTexture( backbuffer, colorTarget );

	m_camera.SetColorTarget( nullptr );
	g_theRenderer->ReleaseRenderTarget( colorTarget );
	g_theRenderer->ReleaseRenderTarget( bloomTarget );
	g_theRenderer->ReleaseRenderTarget( albedoTarget );
	g_theRenderer->ReleaseRenderTarget( normalTarget );
	g_theRenderer->ReleaseRenderTarget( tangentTarget );


	GUARANTEE_OR_DIE( g_theRenderer->GetTotalRenderTargetPoolSize() < 8, "Created too many render targets" );

	g_theRenderer->BeginCamera( m_UICamera );
	FPSRender();
	UIRender();
	g_theRenderer->EndCamera( m_UICamera );

	DebugRenderScreenTo( g_theRenderer->GetBackBuffer() );
	DebugRenderEndFrame();
}




bool Game::SetAmbientColor( const EventArgs* args )
{
	Rgba8 ambientColor = args->GetValue( "color", Rgba8::WHITE );
	g_theRenderer->SetAmbientColor( ambientColor );

	return true;
}

bool Game::SetAttenuation( const EventArgs* args )
{
	Vec3 attenuation = args->GetValue( "attenuation", Vec3( 0.f, 1.f, 0.f ) );
	g_theGame->SetAttenuation( attenuation );

	return true;
}

void Game::SetAttenuation( Vec3 const& newAttenuation )
{
	Vec3& attenuation = m_lights[m_currentLightIndex].attenuation;
	attenuation = newAttenuation;
}

bool Game::SetLightColor( const EventArgs* args )
{
	Rgba8 lightColor = args->GetValue( "color", Rgba8::WHITE );
	uint lightIndex = args->GetValue( "index", 0 );

	float lightColorAsFloats[4];
	lightColor.ToFloatArray( lightColorAsFloats );

	Vec3 lightColorAsVec3 = Vec3( lightColorAsFloats[0], lightColorAsFloats[1], lightColorAsFloats[2]);
	lightColorAsVec3 /= 255.f;
	Game::m_lights[lightIndex].color = lightColorAsVec3;

	return true;
}

bool Game::TestEventSystem( EventArgs const& args )
{
	UNUSED( args );
	DebugAddScreenTextf( Vec4( 0.5f, 0.5f, 0.f, 0.f), Vec2(), 10.f, Rgba8::GREEN, Rgba8::GREEN, 5.f, "Test Event System" );

	return true;
}

bool Game::TestEventSystem2( EventArgs const& args )
{
	UNUSED( args );
	DebugAddScreenTextf( Vec4( 0.5f, 0.3f, 0.f, 0.f ), Vec2(), 10.f, Rgba8::GREEN, Rgba8::GREEN, 5.f, "Test Event System 2" );

	return true;
}

void Game::IncrementCurrentLight()
{
	m_currentLightIndex++;
	if( m_currentLightIndex >= MAX_LIGHTS )
	{
		m_currentLightIndex = 0;
	}
}

void Game::DecrementCurrentLight()
{
	if( m_currentLightIndex == 0 )
	{
		m_currentLightIndex = MAX_LIGHTS - 1;
	}
	else
	{
		m_currentLightIndex--;
	}
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

void Game::ToggleAttenuation()
{
	Vec3& attenuation = m_lights[m_currentLightIndex].attenuation;
	if( attenuation.x == 1.f )
	{
		attenuation.x = 0.f;
		attenuation.y = 1.f;
		attenuation.z = 0.f;
	}
	else if( attenuation.y == 1.f )
	{
		attenuation.x = 0.f;
		attenuation.y = 0.f;
		attenuation.z = 1.f;
	}
	else if( attenuation.z == 1.f )
	{
		attenuation.x = 1.f;
		attenuation.y = 0.f;
		attenuation.z = 0.f;
	}
	else
	{
		attenuation.x = 1.f;
		attenuation.y = 0.f;
		attenuation.z = 0.f;
	}
}

void Game::ToggleBloom()
{
	m_isBloomActive = !m_isBloomActive;
}



void Game::UpdateLightPosition( float deltaSeconds )
{
	m_lightAnimatedTheta += deltaSeconds * 45.f;
	m_lightAnimatedPhi += deltaSeconds * 90.f;

	Vec2 xzLightPath = Vec2::MakeFromPolarDegrees( m_lightAnimatedTheta, 6.f );
	float yLight = SinDegrees( m_lightAnimatedPhi );

	m_lightAnimatedPosition = Vec3( xzLightPath.x, yLight, xzLightPath.y );
	m_lightAnimatedPosition.z *= 0.5f;
	m_lightAnimatedPosition.z += -10.f;
	m_lightAnimatedPosition.x *= 2.f;


	if( m_isLightFollowingCamera )
	{
		m_lights[m_currentLightIndex].position = m_camera.GetPosition();
		m_lights[m_currentLightIndex].direction = m_camera.GetDirection();
	}
	else if( m_isLightAnimated )
	{
		m_lights[m_currentLightIndex].position = m_lightAnimatedPosition;
	}
}

void Game::EnableLights()
{
	for( size_t lightIndex = 0; lightIndex < MAX_LIGHTS; lightIndex++ )
	{
		g_theRenderer->EnableLight( (uint)lightIndex, m_lights[lightIndex] );
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
		DecrementCurrentLight();
	}
	if( kKey.WasJustPressed() )
	{
		IncrementCurrentLight();
	}
	if( zKey.WasJustPressed() )
	{
		m_lights[m_currentLightIndex].direction = m_camera.GetDirection();
		if( m_lights[m_currentLightIndex].isDirectional == 1.f )
		{
			m_lights[m_currentLightIndex].isDirectional = 0.f;
		}
		else
		{
			m_lights[m_currentLightIndex].isDirectional = 1.f;
		}

	}
	if( xKey.WasJustPressed() )
	{
		if( m_lights[m_currentLightIndex].cosOuterAngle == -1.f )
		{
			m_lights[m_currentLightIndex].cosInnerAngle = 0.98f;
			m_lights[m_currentLightIndex].cosOuterAngle = 0.96f;
		}
		else
		{
			m_lights[m_currentLightIndex].cosInnerAngle = -0.98f;
			m_lights[m_currentLightIndex].cosOuterAngle = -1.f;
		}
	}
	if( uKey.IsPressed() )
	{
		m_dissolveAmount -= 0.5f * deltaSeconds;
		m_dissolveAmount = Clampf( m_dissolveAmount, 0.f, 1.f );
	}
	if( iKey.IsPressed() )
	{
		m_dissolveAmount += 0.5f * deltaSeconds;
		m_dissolveAmount = Clampf( m_dissolveAmount, 0.f, 1.f );
	}
	if( qKey.WasJustPressed() )
	{
		SetLightPosition( m_camera.GetPosition() );
	}

	if( f1Key.WasJustPressed() )
	{
		m_isDebugRenderingEnabled = !m_isDebugRenderingEnabled;

		if( m_isDebugRenderingEnabled )
		{
			EnableDebugRendering();
		}
		else
		{
			DisableDebugRendering();
		}
	}

	if( f11Key.WasJustPressed() )
	{
		g_theWindow->ToggleBorder();
	}

	if( num1Key.IsPressed() )
	{
		float currentInnerCosTheta = m_lights[m_currentLightIndex].cosInnerAngle;
		float currentOuterCosTheta = m_lights[m_currentLightIndex].cosOuterAngle;
		float range = currentInnerCosTheta - currentOuterCosTheta;

		float max = 1.f;
		float min = -1.f + range;

		float newInnerCosTheta = currentInnerCosTheta - 0.5f * deltaSeconds;
		newInnerCosTheta = Clampf( newInnerCosTheta, min, max );
		float newOuterCosTheta = newInnerCosTheta - range;

		m_lights[m_currentLightIndex].cosInnerAngle = newInnerCosTheta;
		m_lights[m_currentLightIndex].cosOuterAngle = newOuterCosTheta;
	}
	if( num2Key.IsPressed() )
	{
		float currentInnerCosTheta = m_lights[m_currentLightIndex].cosInnerAngle;
		float currentOuterCosTheta = m_lights[m_currentLightIndex].cosOuterAngle;
		float range = currentInnerCosTheta - currentOuterCosTheta;

		float max = 1.f;
		float min = -1.f + range;

		float newInnerCosTheta = currentInnerCosTheta + 0.5f * deltaSeconds;
		newInnerCosTheta = Clampf( newInnerCosTheta, min, max );
		float newOuterCosTheta = newInnerCosTheta - range;

		m_lights[m_currentLightIndex].cosInnerAngle = newInnerCosTheta;
		m_lights[m_currentLightIndex].cosOuterAngle = newOuterCosTheta;
	}
	if( num3Key.IsPressed() )
	{
		float fogMin = m_fogRange * 0.5f;
		float newFogDistance = m_fogDistance - 10.f * deltaSeconds;

		newFogDistance = Max( newFogDistance, fogMin );
		m_fogDistance = newFogDistance;

		float newNearFog = newFogDistance - fogMin;
		float newFarFog = newFogDistance + fogMin;
		Rgba8 lerpColor = Rgba8::LerpColorAsHSL( m_fogRed, m_fogBlue, m_fogColorLerp );
		g_theRenderer->EnableFog( newNearFog, newFarFog, lerpColor );
	}
	if( num4Key.IsPressed() )
	{
		float fogMin = m_fogRange * 0.5f;
		float newFogDistance = m_fogDistance + 10.f * deltaSeconds;

		newFogDistance = Max( newFogDistance, fogMin );
		m_fogDistance = newFogDistance;

		float newNearFog = newFogDistance - fogMin;
		float newFarFog = newFogDistance + fogMin;
		Rgba8 lerpColor = Rgba8::LerpColorAsHSL( m_fogRed, m_fogBlue, m_fogColorLerp );
		g_theRenderer->EnableFog( newNearFog, newFarFog, lerpColor );
	}
	if( num5Key.IsPressed() )
	{
		m_greyscaleAmount -= deltaSeconds;
		m_greyscaleAmount = Clampf( m_greyscaleAmount, 0.f, 1.f );
	}
	if( num6Key.IsPressed() )
	{
		m_greyscaleAmount += deltaSeconds;
		m_greyscaleAmount = Clampf( m_greyscaleAmount, 0.f, 1.f );
	}
	if( num7Key.IsPressed() )
	{
		m_tintAmount -= deltaSeconds;
		m_tintAmount = Clampf( m_tintAmount, 0.f, 1.f );
	}
	if( num8Key.IsPressed() )
	{
		m_tintAmount += deltaSeconds;
		m_tintAmount = Clampf( m_tintAmount, 0.f, 1.f );
	}
	if( num9Key.IsPressed() )
	{
		Vec4 currentAmbientLight = g_theRenderer->GetAmbientLight();
		float currentAmbientIntensity = currentAmbientLight.w;

		float newAmbientIntensity = currentAmbientIntensity - ( 0.5f*deltaSeconds );
		newAmbientIntensity = Clampf( newAmbientIntensity, 0.f, 1.f );
		g_theRenderer->SetAmbientIntensity( newAmbientIntensity );
	}
	if( num0Key.IsPressed() )
	{
		Vec4 currentAmbientLight = g_theRenderer->GetAmbientLight();
		float currentAmbientIntensity = currentAmbientLight.w;

		float newAmbientIntensity = currentAmbientIntensity + (0.5f*deltaSeconds);
		newAmbientIntensity = Clampf( newAmbientIntensity, 0.f, 1.f );
		g_theRenderer->SetAmbientIntensity( newAmbientIntensity );
	}
	if( lBracketKey.IsPressed() )
	{
		float currentSpecularFactor = g_theRenderer->GetSpecularFactor();

		float newSpecularFactor = currentSpecularFactor - 0.5f * deltaSeconds;
		newSpecularFactor = Clampf( newSpecularFactor, 0.f, 1.f );
		g_theRenderer->SetSpecularFactor( newSpecularFactor );
	}
	if( rBracketKey.IsPressed() )
	{
		float currentSpecularFactor = g_theRenderer->GetSpecularFactor();

		float newSpecularFactor = currentSpecularFactor + 0.5f * deltaSeconds;
		newSpecularFactor = Clampf( newSpecularFactor, 0.f, 1.f );
		g_theRenderer->SetSpecularFactor( newSpecularFactor );
	}
	if( semiColonKey.IsPressed() )
	{
		float currentSpecularPower = g_theRenderer->GetSpecularPower();

		float newSpecularPower = currentSpecularPower - 20.f * deltaSeconds;
		newSpecularPower = Max( newSpecularPower, 1.f );
		g_theRenderer->SetSpecularPower( newSpecularPower );
	}
	if( singleQuoteKey.IsPressed() )
	{
		float currentSpecularPower = g_theRenderer->GetSpecularPower();

		float newSpecularPower = currentSpecularPower + 20.f * deltaSeconds;
		newSpecularPower = Max( newSpecularPower, 1.f );
		g_theRenderer->SetSpecularPower( newSpecularPower );
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
		m_lights[m_currentLightIndex].position = Vec3( 0.f, 0.f, 0.f );
		m_isLightFollowingCamera = false;
		m_isLightAnimated = false;
	}
	if( f6Key.WasJustPressed() )
	{
		m_lights[m_currentLightIndex].position = m_camera.GetPosition();
		m_isLightFollowingCamera = false;
		m_isLightAnimated = false;
	}
	if( f7Key.WasJustPressed() )
	{
		m_lights[m_currentLightIndex].position = m_camera.GetPosition();
		m_isLightFollowingCamera = true;
		m_isLightAnimated = false;
	}
	if( f8Key.WasJustPressed() )
	{
		m_lights[m_currentLightIndex].position = m_lightAnimatedPosition;
		m_isLightFollowingCamera = false;
		m_isLightAnimated = true;
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
		//ToggleAttenuation();
		ToggleBloom();
	}
	if( plusKey.IsPressed() )
	{
		float currentLightIntensity = m_lights[m_currentLightIndex].intensity;

		float newLightIntensity = currentLightIntensity + 2.f * deltaSeconds;
		newLightIntensity = Max( newLightIntensity, 0.f );
		m_lights[m_currentLightIndex].intensity = newLightIntensity;
	}
	if( minusKey.IsPressed() )
	{
		float currentLightIntensity = m_lights[m_currentLightIndex].intensity;

		float newLightIntensity = currentLightIntensity - 2.f * deltaSeconds;
		newLightIntensity = Max( newLightIntensity, 0.f );
		m_lights[m_currentLightIndex].intensity = newLightIntensity;
	}

	Vec3 translator;

	if( wKey.IsPressed() )
	{
		translator.x +=  1.f * deltaSeconds;
	}
	if( sKey.IsPressed() )
	{
		translator.x -=  1.f * deltaSeconds;
	}
	if( aKey.IsPressed() )
	{
		translator.y +=  1.f * deltaSeconds;
	}
	if( dKey.IsPressed() )
	{
		translator.y -=  1.f * deltaSeconds;
	}
	if( cKey.IsPressed() )
	{
		translator.z +=  1.f * deltaSeconds;
	}
	if( spaceKey.IsPressed() )
	{
		translator.z -=  1.f * deltaSeconds;
	}

	if( shiftKey.IsPressed() )
	{
		translator *= 2.f;
	}

	m_camera.TranslateRelativeToViewOnlyYaw( translator );

	Vec3 rotator;
	if( upArrow.IsPressed() )
	{
		rotator.x += 1.f * deltaSeconds;
	}
	if( downArrow.IsPressed() )
	{
		rotator.x -= 1.f * deltaSeconds;
	}
	if( leftArrow.IsPressed() )
	{
		rotator.z += 1.f * deltaSeconds;
	}
	if( rightArrow.IsPressed() )
	{
		rotator.z -= 1.f * deltaSeconds;
	}

	Vec2 mouseChange = g_theInput->GetMouseDeltaPos();

	rotator.x += mouseChange.y * 0.1f;
	rotator.z -= mouseChange.x * 0.1f;
	m_camera.RotatePitchRollYawDegrees( rotator );

}


void Game::SetLightPosition( Vec3 const& pos )
{
	m_lights[m_currentLightIndex].position = pos;
}

void Game::FPSStartup()
{
	for( int i = 0; i < 10; i++ )
	{
		m_deltaSecondsFromLastFrame[i] = 0.0;
	}
}

void Game::FPSCounterUpdate()
{
	m_deltaSecondsFromLastFrame[m_fpsCounterIndex] = m_gameClock->GetLastDeltaSeconds();
	m_fpsCounterIndex++;
	if( m_fpsCounterIndex > 9 )
	{
		m_fpsCounterIndex = 0;
	}

	m_fps = 0.f;
	for( int frameIndex = 0; frameIndex < 10; frameIndex++ )
	{
		m_fps += (float)m_deltaSecondsFromLastFrame[m_fpsCounterIndex];
	}

	m_fps /= 10.f;
	m_msPerFrame = m_fps;
	m_fps = 1.f / m_fps;
	
	m_msPerFrame *= 1000.f;
}

void Game::FPSRender()
{
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	std::string renderString = Stringf( "FPS: %.2f (%.2f ms/frame)", m_fps, m_msPerFrame );
	g_theRenderer->DrawTextAtPosition( renderString.c_str(), Vec2( 13.f, 8.75f ), 0.1f );
}

void Game::UIRender()
{
	Texture* baseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/HUD_Base.png" );
	float baseAspect = baseTexture->GetAspectRatio();
	float heightOverWidth = 1.f / baseAspect;
	AABB2 screenDimensions = AABB2( m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight() );
	float screenWidth = screenDimensions.maxs.x - screenDimensions.mins.x;
	float screenHeight = screenWidth * heightOverWidth;
	AABB2 uiBounds = screenDimensions.CarveBoxOffBottom( 0.f, screenHeight );
	//uiBounds.maxs.y = screenHeight;
	g_theRenderer->BindTexture( baseTexture );
	g_theRenderer->DrawAABB2Filled( uiBounds, Rgba8::WHITE );

	AABB2 gunBounds = screenDimensions.CarveBoxOffBottom( 0.f, screenHeight );
	AABB2 gunSpriteUvs;
	SpriteDefinition const& gunSpriteDef = m_viewModelsSpriteSheet->GetSpriteDefinition( 0 );
	Texture const& gunSpriteTexture = gunSpriteDef.GetTexture();
	float gunSpriteTextureAspect = gunSpriteTexture.GetAspectRatio();
	float gunSpriteHeightOverWidth = 1.f / gunSpriteTextureAspect;
	gunBounds.maxs.y = gunSpriteHeightOverWidth * (gunBounds.GetDimensions().x) - gunBounds.mins.y;
	gunSpriteDef.GetUVs( gunSpriteUvs.mins, gunSpriteUvs.maxs );
	g_theRenderer->BindTexture( &gunSpriteTexture );
	g_theRenderer->DrawAABB2Filled( gunBounds, Rgba8::WHITE, gunSpriteUvs.mins, gunSpriteUvs.maxs );
}

void Game::LoadAssets()
{
	Texture* viewModelSpriteTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/ViewModelsSpriteSheet_8x8.png" );
	m_viewModelsSpriteSheet = new SpriteSheet( *viewModelSpriteTex, IntVec2( 8, 8 ) );
}

void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}
