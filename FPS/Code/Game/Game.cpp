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
#include "Engine/Renderer/Shader.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;


light_t Game::m_light;

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


	m_cubeModelMatrix = Mat44::CreateTranslation3D( Vec3( 0.f, 0.f, -10.f ) );
	m_sphereModelMatrix = Mat44::CreateTranslation3D( Vec3( 4.f, 0.f, -10.f ) );
	m_quadModelMatrix = Mat44::CreateTranslation3D( Vec3( -4.f, 0.f, -10.f ) );
	m_circleOfSpheresModelMatrix = Mat44::CreateTranslation3D( Vec3( 0.f, 0.f, -20.f ) );
	m_numberOfCirclingCubes = 18;

	m_cubeMesh = new GPUMesh( g_theRenderer );

	std::vector<Vertex_PCUTBN> cubeVerts;
	std::vector<uint> cubeIndices;
	Vertex_PCUTBN::AppendIndexedVertsCube( cubeVerts, cubeIndices, 1.f );
	m_cubeMesh->UpdateVertices( cubeVerts );
	m_cubeMesh->UpdateIndices( cubeIndices );


	m_sphereMesh = new GPUMesh( g_theRenderer );

	std::vector<Vertex_PCUTBN> sphereVerts;
	std::vector<uint> sphereIndices;
	Vertex_PCUTBN::AppendIndexedVertsSphere( sphereVerts, sphereIndices, 1.f );
	m_sphereMesh->UpdateVertices( sphereVerts );
	m_sphereMesh->UpdateIndices( sphereIndices );

	m_quadMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCUTBN> quadVerts;
	std::vector<uint> quadIndices;
	Vertex_PCUTBN::AppendVerts4Points( quadVerts, 
		quadIndices, 
		Vec3(-1.f, -1.f, 0.f), 
		Vec3( 1.f, -1.f, 0.f), 
		Vec3( 1.f, 1.f, 0 ), 
		Vec3( -1.f, 1.f, 0.f ), 
		Rgba8::WHITE, 
		AABB2(0.f, 0.f, 1.f, 1.f ) );

	m_quadMesh->UpdateVertices( quadVerts );
	m_quadMesh->UpdateIndices( quadIndices );



	m_gameClock = new Clock();
	m_gameClock->SetParent( Clock::GetMaster() );

	g_theRenderer->Setup( m_gameClock );

	m_screenTexture = g_theRenderer->CreateTextureFromColor( Rgba8::BLACK, IntVec2(1920,1080) );

	m_light.color = Vec3(1.f, 1.f, 1.f);
	m_light.intensity = 0.5f;
	m_light.position = m_camera.GetPosition();

	g_theRenderer->SetSpecularFactorAndPower( 0.5f, 2.f );

	g_theRenderer->SetAmbientLight( Rgba8::WHITE, 0.5f );

	g_theEventSystem->SubscribeToEvent( "light_set_ambient_color", CONSOLECOMMAND, SetAmbientColor );
	g_theEventSystem->SubscribeToEvent( "light_set_attenuation", CONSOLECOMMAND, SetAttenuation );
	g_theEventSystem->SubscribeToEvent( "light_set_color", CONSOLECOMMAND, SetLightColor );
}

void Game::Shutdown()
{
	delete m_cubeMesh;
	m_cubeMesh = nullptr;

	delete m_sphereMesh;
	m_sphereMesh = nullptr;

	delete m_quadMesh;
	m_quadMesh = nullptr;
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

	UpdateLightPosition( dt );


	m_cubeModelMatrix.RotateYDegrees( 45.f * dt );
	m_cubeModelMatrix.RotateXDegrees( 30.f * dt );
	m_sphereModelMatrix.RotateXDegrees( 30.f * dt );
	m_sphereModelMatrix.RotateYDegrees( -15.f * dt );
	//m_quadModelMatrix.RotateZDegrees( 35.f * dt ); //if you want the quad to rotate

	Mat44 newCircleOfSpheresRotation;
	newCircleOfSpheresRotation.RotateYDegrees( 5.f * dt );
	m_circleOfSpheresModelMatrix.RotateYDegrees( 45.f * dt );
	newCircleOfSpheresRotation.TransformBy( m_circleOfSpheresModelMatrix );
	m_circleOfSpheresModelMatrix = newCircleOfSpheresRotation;

	Vec3 lightColor = m_light.color;
	lightColor *= 255.f;
	Rgba8 lightRGBA8Color = Rgba8( (unsigned char)lightColor.x, (unsigned char)lightColor.y, (unsigned char)lightColor.z );

	if( !m_isLightFollowingCamera )
	{
		DebugAddWorldPoint( m_light.position, lightRGBA8Color, 0.f );
	}



	Vec4 ambientLight = g_theRenderer->GetAmbientLight();
	float ambientIntensity = ambientLight.w;
	Vec3 attenuation = g_theRenderer->GetAttenuation();
	float lightIntensity = m_light.intensity;
	float specularFactor = g_theRenderer->GetSpecularFactor();
	float specularPower = g_theRenderer->GetSpecularPower();
	float gamma = g_theRenderer->GetGamma();


	std::string renderTextureFilePathStr;
	std::string normalTextureFilepathStr;

	if( nullptr != m_renderTextures[m_currentRenderTextureIndex] )
	{
		renderTextureFilePathStr = m_renderTextures[m_currentRenderTextureIndex]->GetFilePath();
	}
	if( nullptr != m_normalTextures[m_currentNormalTextureIndex] )
	{
		normalTextureFilepathStr = m_normalTextures[m_currentNormalTextureIndex]->GetFilePath();
	}

	std::string cycleShaderStr = Stringf("<,> Cycle Shader: %s", m_shaders[m_currentShaderIndex]->m_filename.c_str() );
	std::string cycleRenderTextureStr = Stringf("N,M Cycle Texture: %s", renderTextureFilePathStr.c_str() );
	std::string cycleNormalTextureStr = Stringf("V,B Cycle Normal Texture: %s", normalTextureFilepathStr.c_str() );
	std::string ambientIntensityStr = Stringf("9,0 Adjust Ambient Intensity: %.2f", ambientIntensity );
	std::string attenuationStr = Stringf("T Toggle Attenuation (Constant, Linear, Quadratic): (%.2f, %.2f, %.2f)", attenuation.x, attenuation.y, attenuation.z );
	std::string lightIntensityStr = Stringf("-,+ Adjust Light Intensity: %.2f", lightIntensity );
	std::string specularFactorStr = Stringf("[,] Adjust Specular Factor: %.2f", specularFactor );
	std::string specularPowerStr = Stringf(";,' Adjust Specular Power: %.2f", specularPower );
	std::string gammaStr = Stringf("G,H Adjust Gamma: %.2f", gamma );
	std::string lightAtOriginStr = Stringf( "F5 Set Light to Origin" );
	std::string lightToCameraStr = Stringf( "F6 Set Light to Camera Position" );
	std::string lightFollowCameraStr = Stringf( "F7 Set Light to Follow Camera" );
	std::string lightAnimatedStr = Stringf( "F8 Set Light on animated path" );

	DebugAddScreenText( Vec4( 0.01f, 0.95f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, cycleShaderStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.93f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, cycleRenderTextureStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.91f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, cycleNormalTextureStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.89f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, ambientIntensityStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.87f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, attenuationStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.85f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, lightIntensityStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.83f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, specularFactorStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.81f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, specularPowerStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.79f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, gammaStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.77f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, lightAtOriginStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.75f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, lightToCameraStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.73f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, lightFollowCameraStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.71f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, lightAnimatedStr.c_str() );
}

void Game::Render()
{
	g_theRenderer->BeginCamera(m_camera);
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL );
	g_theRenderer->SetBlendMode(BlendMode::ADDITIVE);

// 	Texture* tex = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/example_colour.png");
// 	Texture* normalTex = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/example_normal.png");


 	g_theRenderer->SetBlendMode( BlendMode::SOLID );
 	g_theRenderer->BindTexture( m_renderTextures[m_currentRenderTextureIndex] );
	g_theRenderer->BindNormal( m_normalTextures[m_currentNormalTextureIndex] );


	g_theRenderer->DisableLight( 0 );
	g_theRenderer->EnableLight( 0, m_light );
	g_theRenderer->BindShader( m_shaders[m_currentShaderIndex] );
	g_theRenderer->SetModelMatrix( m_cubeModelMatrix );
	g_theRenderer->DrawMesh( m_cubeMesh );

	g_theRenderer->SetModelMatrix( m_quadModelMatrix );
	g_theRenderer->DrawMesh( m_quadMesh );

	g_theRenderer->SetModelMatrix( m_sphereModelMatrix );
	g_theRenderer->DrawMesh( m_sphereMesh );



	g_theRenderer->SetBlendMode( BlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL );
	fresnel_t fresnel;
	fresnel.color = Vec3( 0.f, 1.f, 0.f );
	fresnel.power = 16.f;
	fresnel.factor = 1.f;

	g_theRenderer->SetMaterialData( &fresnel, sizeof(fresnel) );
	Shader* fresnelShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/Fresnel.hlsl" );
	g_theRenderer->BindShader( fresnelShader );
	g_theRenderer->SetModelMatrix( m_sphereModelMatrix );
	g_theRenderer->DrawMesh( m_sphereMesh );




	g_theRenderer->SetBlendMode( BlendMode::SOLID );
	g_theRenderer->BindShader( m_shaders[m_currentShaderIndex] );
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

bool Game::SetAmbientColor( const EventArgs* args )
{
	Rgba8 ambientColor = args->GetValue( "color", Rgba8::WHITE );
	g_theRenderer->SetAmbientColor( ambientColor );

	return true;
}

bool Game::SetAttenuation( const EventArgs* args )
{
	Vec3 attenuation = args->GetValue( "attenuation", Vec3( 0.f, 1.f, 0.f ) );
	g_theRenderer->SetAttenuation( attenuation );

	return true;
}

bool Game::SetLightColor( const EventArgs* args )
{
	Rgba8 lightColor = args->GetValue( "color", Rgba8::WHITE );

	float lightColorAsFloats[4];
	lightColor.ToFloatArray( lightColorAsFloats );

	Vec3 lightColorAsVec3 = Vec3( lightColorAsFloats[0], lightColorAsFloats[1], lightColorAsFloats[2]);
	lightColorAsVec3 /= 255.f;
	Game::m_light.color = lightColorAsVec3;

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
		m_light.position = m_camera.GetPosition();
	}
	else if( m_isLightAnimated )
	{
		m_light.position = m_lightAnimatedPosition;
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
	const KeyButtonState& yKey = g_theInput->GetKeyStates( 'Y' );
	const KeyButtonState& qKey = g_theInput->GetKeyStates( 'Q' );
	const KeyButtonState& vKey = g_theInput->GetKeyStates( 'V' );
	const KeyButtonState& bKey = g_theInput->GetKeyStates( 'B' );
	const KeyButtonState& nKey = g_theInput->GetKeyStates( 'N' );
	const KeyButtonState& mKey = g_theInput->GetKeyStates( 'M' );
	const KeyButtonState& plusKey = g_theInput->GetKeyStates( PLUS_KEY );
	const KeyButtonState& minusKey = g_theInput->GetKeyStates( MINUS_KEY );
	const KeyButtonState& semiColonKey = g_theInput->GetKeyStates( SEMICOLON_KEY );
	const KeyButtonState& singleQuoteKey = g_theInput->GetKeyStates( SINGLEQUOTE_KEY );
	const KeyButtonState& commaKey = g_theInput->GetKeyStates( COMMA_KEY );
	const KeyButtonState& periodKey = g_theInput->GetKeyStates( PERIOD_KEY );

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
		Transform transform = m_camera.GetTransform();

		DebugAddWireMeshToWorld( transform.ToMatrix(), m_sphereMesh, Rgba8::WHITE, Rgba8::WHITE, 15.f, DEBUG_RENDER_ALWAYS );
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
		DecrementShader();
	}
	if( periodKey.WasJustPressed() )
	{
		IncrementShader();
	}
	if( vKey.WasJustPressed() )
	{
		DecrementNormalTexture();
	}
	if( bKey.WasJustPressed() )
	{
		IncrementNormalTexture();
	}
	if( nKey.WasJustPressed() )
	{
		DecrementRenderTexture();
	}
	if( mKey.WasJustPressed() )
	{
		IncrementRenderTexture();
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
		m_light.position = Vec3( 0.f, 0.f, 0.f );
		m_isLightFollowingCamera = false;
		m_isLightAnimated = false;
	}
	if( f6Key.WasJustPressed() )
	{
		m_light.position = m_camera.GetPosition();
		m_isLightFollowingCamera = false;
		m_isLightAnimated = false;
	}
	if( f7Key.WasJustPressed() )
	{
		m_light.position = m_camera.GetPosition();
		m_isLightFollowingCamera = true;
		m_isLightAnimated = false;
	}
	if( f8Key.WasJustPressed() )
	{
		m_light.position = m_lightAnimatedPosition;
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
		g_theRenderer->ToggleAttenuation();
	}
	if( yKey.WasJustPressed() )
	{
		Mat44 cameraModel = m_camera.GetCameraModelMatrix();
		DebugAddWireMeshToWorld( cameraModel, m_sphereMesh, Rgba8::RED, Rgba8::BLUE, 45.f, DEBUG_RENDER_USE_DEPTH );
	}
	if( plusKey.IsPressed() )
	{
		float currentLightIntensity = m_light.intensity;

		float newLightIntensity = currentLightIntensity + 2.f * deltaSeconds;
		newLightIntensity = Max( newLightIntensity, 0.f );
		m_light.intensity = newLightIntensity;
	}
	if( minusKey.IsPressed() )
	{
		float currentLightIntensity = m_light.intensity;

		float newLightIntensity = currentLightIntensity - 2.f * deltaSeconds;
		newLightIntensity = Max( newLightIntensity, 0.f );
		m_light.intensity = newLightIntensity;
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
	m_light.position = pos;
}

void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}
