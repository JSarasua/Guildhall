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
	EnableDebugRendering();
	m_camera = Camera();
	m_camera.SetColorTarget(nullptr); // we use this
	m_camera.CreateMatchingDepthStencilTarget( g_theRenderer );
	m_camera.SetOutputSize( Vec2( 16.f, 9.f ) );
	m_camera.SetProjectionPerspective( 60.f, -0.09f, -100.f );
	m_camera.Translate( Vec3( -2.f, 0.f, 1.5f ) );
	//m_camera.SetProjectionOrthographic( m_camera.m_outputSize, -0.1f, -100.f );
	
	XmlDocument shaderStateDoc;
	XmlElement const& element = GetRootElement( shaderStateDoc, "Data/ShaderStates/BasicLit.xml" );
	m_testShaderState = new ShaderState(g_theRenderer, element );
	g_theRenderer->AcquireShaderState( m_testShaderState );
	m_testMaterial = new Material( g_theRenderer, "Data/Materials/TestMaterial.xml" );

	dissolve_t dissolveData;
	dissolveData.amount = m_dissolveAmount;
	dissolveData.edgeFarColor = Vec3( 0.5f, 0.f, 0.f );
	dissolveData.edgeNearColor = Vec3( 1.f, 1.f, 0.f );
	dissolveData.edgeWidth = 0.1f;
	
	m_testMaterial->SetData( dissolveData );
	//m_testMaterial->SetShaderByName( dissolveShader );
	
// 	m_testMaterial->m_texturePerSlot.push_back( noiseTexture );
// 	m_testMaterial->m_samplersPerSlot.push_back( new Sampler( g_theRenderer, SAMPLER_POINT, SAMPLER_WRAP ) );

	//g_theRenderer->SetMaterialData( &dissolveData, sizeof( dissolveData ) );

	m_invertShader = g_theRenderer->GetOrCreateShader("Data/Shaders/InvertColor.hlsl");
	Shader* litShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/Phong.hlsl" );
	Shader* normalShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/Normals.hlsl" );
	Shader* tangentShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/Tangents.hlsl" );
	Shader* bitangentShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/Bitangents.hlsl" );
	Shader* surfaceNormalShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/SurfaceNormals.hlsl" );
	m_shaders.push_back( litShader );
	m_shaders.push_back( normalShader );
	m_shaders.push_back( tangentShader );
	m_shaders.push_back( bitangentShader );
	m_shaders.push_back( surfaceNormalShader );


	Texture* couchTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/example_colour.png" );
	Texture* stoneTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/stone_diffuse.png" );
	Texture* testTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/test.png" );
	Texture* chiTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/chi.png" );
	Texture* tileDiffuseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/tile_diffuse.png" );
	Texture* tileGlossTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/tile_gloss.png" );
	Texture* woodCrateTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/woodcrate.png" );
	m_renderTextures.push_back( nullptr );
	m_renderTextures.push_back( couchTexture );
	m_renderTextures.push_back( stoneTexture );
	m_renderTextures.push_back( testTexture );
	m_renderTextures.push_back( chiTexture );
	m_renderTextures.push_back( tileDiffuseTexture );
	m_renderTextures.push_back( tileGlossTexture );
	m_renderTextures.push_back( woodCrateTexture );

	Texture* normalCouchTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/example_normal.png" );
	Texture* normalStoneTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/stone_normal.png" );
	Texture* normalTileTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/tile_normal.png" );
	Texture* normalTestSphereTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/test_normal.png" );
	m_normalTextures.push_back( nullptr );
	m_normalTextures.push_back( normalCouchTexture );
	m_normalTextures.push_back( normalStoneTexture );
	m_normalTextures.push_back( testTexture );
	m_normalTextures.push_back( chiTexture );
	m_normalTextures.push_back( normalTileTexture );
	m_normalTextures.push_back( normalTestSphereTexture );

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

	InitializeGameState();
	m_mcAI.SetGameState( m_currentGameState );
	m_mcAI.SetMaxDepth( 100 );
}

void Game::Shutdown()
{
	delete m_cubeMesh;
	m_cubeMesh = nullptr;

	delete m_testMaterial;
	m_testMaterial = nullptr;
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

	DebugAddWorldBasis( Mat44(), 0.f, DEBUG_RENDER_ALWAYS );
	Mat44 cameraModelMatrix = Mat44();
	cameraModelMatrix.ScaleUniform3D( 0.01f );
	Vec3 compassPosition = m_camera.GetPosition();
	compassPosition += m_camera.GetDirection() * 0.1f;

// 	cameraModelMatrix.SetTranslation3D( compassPosition );
// 	DebugAddWorldBasis( cameraModelMatrix, 0.f, DEBUG_RENDER_ALWAYS );
	//DebugAddScreenBasis( m_camera.GetModelRotationMatrix(), Rgba8::WHITE, Rgba8::WHITE, 0.f );

	//GREY: Playing (UI)
	//YELLOW: Camera Yaw=X.X	Pitch=X.X	Roll=X.X	xyz=(X.XX, Y.YY, Z.ZZ)
	//RED: iBasis (forward,	+x world-east when identity): (X.XX, Y.YY, Z.ZZ)
	//GREEN: jBasis (left,	+y world-north when identity):	(X.XX, Y.YY, Z.ZZ)
	//BLUE: kBasis (up, +z world-up when identity):		(X.XX, Y.YY, Z.ZZ)

// 	Vec3 cameraPitchRollYaw = m_camera.GetRotation();
// 	Vec3 cameraPosition = m_camera.GetPosition();
// 	float yaw = cameraPitchRollYaw.z;
// 	float pitch = cameraPitchRollYaw.x;
// 	float roll = cameraPitchRollYaw.y;
// 	Mat44 cameraBases = m_camera.GetCameraScreenRotationMatrix();
// 	Vec3 cameraIBasis = cameraBases.GetIBasis3D();
// 	Vec3 cameraJBasis = cameraBases.GetJBasis3D();
// 	Vec3 cameraKBasis = cameraBases.GetKBasis3D();
// 
//  	std::string playingUIStr = Stringf( "Playing (UI)");
// 	std::string cameraRotationTranslationStr = Stringf( "Camera Yaw = %.1f	Pitch=%.1f	Roll=%.1f	xyz=(%.2f, %.2f, %.2f)", yaw, pitch, roll, cameraPosition.x, cameraPosition.y, cameraPosition.z );;
// 	std::string cameraIBasisStr = Stringf( "iBasis (forward, +x world-east when identity): (%.2f, %.2f, %.2f)", cameraIBasis.x, cameraIBasis.y, cameraIBasis.z );
// 	std::string cameraJBasisStr = Stringf( "jBasis (left,	+y world-north when identity):	(%.2f, %.2f, %.2f)", cameraJBasis.x, cameraJBasis.y, cameraJBasis.z );
// 	std::string cameraKBasisStr = Stringf( "kBasis (up, +z world-up when identity):		(%.2f, %.2f, %.2f)", cameraKBasis.x, cameraKBasis.y, cameraKBasis.z );
// 
//  	DebugAddScreenText( Vec4( 0.01f, 0.95f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, playingUIStr.c_str() );
// 	DebugAddScreenText( Vec4( 0.01f, 0.93f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::YELLOW, Rgba8::YELLOW, 0.f, cameraRotationTranslationStr.c_str() );
// 	DebugAddScreenText( Vec4( 0.01f, 0.91f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::RED, Rgba8::RED, 0.f, cameraIBasisStr.c_str() );
// 	DebugAddScreenText( Vec4( 0.01f, 0.89f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, cameraJBasisStr.c_str() );
// 	DebugAddScreenText( Vec4( 0.01f, 0.87f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::BLUE, Rgba8::BLUE, 0.f, cameraKBasisStr.c_str() );

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

	g_theRenderer->DisableLight( 0 );
	//EnableLights();

	Texture* testTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	g_theRenderer->BindTexture( testTexture );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetModelMatrix( m_frontCubeModelMatrix );

// 	Shader* shader = g_theRenderer->GetOrCreateShader(  "Data/Shaders/WorldOpaque.hlsl" );
// 	g_theRenderer->BindShader( shader );
// 	g_theRenderer->DrawMesh( m_cubeMesh );
// 
// 	g_theRenderer->SetModelMatrix( m_leftCubeModelMatrix );
// 	g_theRenderer->DrawMesh( m_cubeMesh );
// 
// 	g_theRenderer->SetModelMatrix( m_frontleftCubeModelMatrix );
// 	g_theRenderer->DrawMesh( m_cubeMesh );

	//RenderProjection();

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->EndCamera(m_camera);

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
		move1.m_columnToPush = 2;
		
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
		move1.m_columnToPush = 2;
		
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
		move1.m_columnToPush = 1;
		
		moves.push_back( move1 );
		moves.push_back( move2 );
	}

	return moves;
}

bool Game::IsGameStateWon( gameState_t const& gameState )
{
	std::deque<int> const& col2 = gameState.columns[2];

	if( col2.size() != 5 )
	{
		return false;
	}

	if( col2[0] == 5 && col2[1] == 4 && col2[2] == 3 && col2[3] == 2 && col2[4] == 1 )
	{
		return true;
	}

	return false;
}

void Game::UpdateGameStateIfValid( inputMove_t const& inputMove )
{
	if( IsMoveValidForGameState( inputMove, m_currentGameState ) )
	{
		int valueToPop = m_currentGameState.columns[inputMove.m_columnToPop].back();
		m_currentGameState.columns[inputMove.m_columnToPop].pop_back();

		m_currentGameState.columns[inputMove.m_columnToPush].push_back( valueToPop );
	}
}

void Game::UpdateGameStateIfValid( inputMove_t const& inputMove, gameState_t& gameState )
{
	if( IsMoveValidForGameState( inputMove, gameState ) )
	{
		int valueToPop = gameState.columns[inputMove.m_columnToPop].back();
		gameState.columns[inputMove.m_columnToPop].pop_back();

		gameState.columns[inputMove.m_columnToPush].push_back( valueToPop );
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

void Game::IncrementShader()
{
	m_currentShaderIndex++;
	if( m_currentShaderIndex >= m_shaders.size() )
	{
		m_currentShaderIndex = 0;
	}
}

void Game::DecrementShader()
{
	if( m_currentShaderIndex == 0 )
	{
		if( !m_shaders.empty() )
		{
			m_currentShaderIndex = m_shaders.size() - 1;
		}
	}
	else
	{
		m_currentShaderIndex--;
	}
}

void Game::IncrementRenderTexture()
{
	m_currentRenderTextureIndex++;
	if( m_currentRenderTextureIndex >= m_renderTextures.size() )
	{
		m_currentRenderTextureIndex = 0;
	}
}

void Game::DecrementRenderTexture()
{
	if( m_currentRenderTextureIndex == 0 )
	{
		if( !m_renderTextures.empty() )
		{
			m_currentRenderTextureIndex = m_renderTextures.size() - 1;
		}
	}
	else
	{
		m_currentRenderTextureIndex--;
	}
}

void Game::IncrementNormalTexture()
{
	m_currentNormalTextureIndex++;
	if( m_currentNormalTextureIndex >= m_normalTextures.size() )
	{
		m_currentNormalTextureIndex = 0;
	}
}

void Game::DecrementNormalTexture()
{
	if( m_currentNormalTextureIndex == 0 )
	{
		if( !m_normalTextures.empty() )
		{
			m_currentNormalTextureIndex = m_normalTextures.size() - 1;
		}
	}
	else
	{
		m_currentNormalTextureIndex--;
	}
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
			m_currentInputMove.m_columnToPop = 0;
			m_currentInputMove.m_columnToPush = 0;
			m_isInputPop = true;
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
		m_mcAI.RunSimulations( 1000 );
		m_mcAI.UpdateBestMove();
		UpdateGameStateIfValid( m_mcAI.GetBestMove() );
	}
	if( rKey.WasJustPressed() )
	{
		InitializeGameState();
		m_mcAI.SetGameState( m_currentGameState );
		m_mcAI.SetMaxDepth( 100 );
	}
}

IntVec2 Game::GetCurrentMapBounds() const
{
	return m_world->getCurrentMapBounds();
}

void Game::SetLightPosition( Vec3 const& pos )
{
	m_lights[m_currentLightIndex].position = pos;
}

void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}
