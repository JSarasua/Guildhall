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
#include "Game/Map3D.hpp"
#include "Game/Player3D.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;


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
	m_camera.SetOutputSize( Vec2( 160.f, 90.f ) );
	m_camera.SetProjectionPerspective( 60.f, -0.1f, -1000.f );
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


	m_sphereModelMatrix = Mat44::CreateTranslation3D( Vec3( 4.f, 0.f, -10.f ) );




	m_quadMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCUTBN> quadVerts;
	std::vector<uint> quadIndices;
	Vertex_PCUTBN::AppendVerts4Points( quadVerts, 
		quadIndices, 
		Vec3(-100.f, -1.f, 100.f), 
		Vec3( 100.f, -1.f, 100.f), 
		Vec3( 10.f, -1.f, -100 ), 
		Vec3( -10.f, -1.f, -100.f ), 
		Rgba8::WHITE, 
		AABB2(0.f, 0.f, 1.f, 1.f ) );

	m_quadMesh->UpdateVertices( quadVerts );
	m_quadMesh->UpdateIndices( quadIndices );



	m_gameClock = new Clock();
	m_gameClock->SetParent( Clock::GetMaster() );
	m_gameTimer.SetSeconds( m_gameClock, 60.f );

	g_theRenderer->Setup( m_gameClock );

	m_screenTexture = g_theRenderer->CreateTextureFromColor( Rgba8::BLACK, IntVec2(1920,1080) );


	Vec3 cameraDirection = m_camera.GetDirection();
	for( uint lightIndex = 0; lightIndex < MAX_LIGHTS; lightIndex++ )
	{
		light_t lightData;
		lightData.color = Vec3( 1.f, 1.f, 1.f );
		lightData.intensity = 0.f;
		lightData.position = m_camera.GetPosition();
		lightData.cosInnerAngle = -1.f;
		lightData.cosOuterAngle = -1.f;
		lightData.direction = cameraDirection;
		m_lights.push_back( lightData );
	}



	g_theRenderer->SetSpecularFactorAndPower( 0.5f, 2.f );
	g_theRenderer->SetAmbientLight( Rgba8::WHITE, 1.f );

	m_map = new Map3D( IntVec2(10,10), this );
	m_map->Startup();
}

void Game::Shutdown()
{
	delete m_quadMesh;
	m_quadMesh = nullptr;

	m_map->Shutdown();
	delete m_map;
	m_map = nullptr;
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

	if( m_isPaused )
	{
		dt = 0.f;
	}

	//float currentElapsedTime = (float)m_gameClock->GetTotalElapsedSeconds();
	if( m_gameTimer.HasElapsed() && !m_map->m_player->m_isDead )
	{
		DebugAddScreenText( Vec4( 0.5f, 0.8f, 0.f, 0.f ), Vec2( 0.5f, 0.5f ), 100.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, "You Win!" );
	}
	else
	{
		if( m_map->m_player->m_isDead )
		{
			dt = 0.f;
			DebugAddScreenText( Vec4( 0.5f, 0.8f, 0.f, 0.f ), Vec2( 0.5f, 0.5f ), 100.f, Rgba8::RED, Rgba8::RED, 0.f, "You're Dead" );
		}
		else
		{
			DebugAddScreenTextf( Vec4( 0.99f, 0.95f, 0.f, 0.f ), Vec2( 1.f, 1.f ), 15.f, Rgba8::BLUE, Rgba8::BLUE, 0.f, "Last Until: %.2f", m_gameTimer.GetSecondsRemaining() );
			DebugAddScreenTextf( Vec4( 0.99f, 0.93f, 0.f, 0.f ), Vec2( 1.f, 1.f ), 15.f, Rgba8::RED, Rgba8::RED, 0.f, "Size: %.2f", m_map->m_player->m_golemScale );
		}
	}


	Rgba8 clearColor;

	clearColor.g = 0;
	clearColor.r = 10;
	clearColor.b = 10;
	m_camera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT, clearColor, 0.f, 0 );

	//m_camera.SetScreenShakeIntensity( 0.1f );
	UpdateScreenShake( dt );
	
	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses( dt );
	}

	UpdateLightPosition( dt );

	m_map->Update( dt );
	UpdateCamera( dt );
}

void Game::Render()
{
	g_theRenderer->BeginCamera(m_camera);
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL );

	//EnableLights();

	Texture* lavaTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TileableLava.png");
	g_theRenderer->BindTexture( lavaTexture );
	g_theRenderer->SetBlendMode( BlendMode::SOLID );
	g_theRenderer->BindShader( m_shaders[m_currentShaderIndex] );

	g_theRenderer->SetModelMatrix( m_quadModelMatrix );
	g_theRenderer->DrawMesh( m_quadMesh );

	m_map->Render();

	RenderDevConsole();

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->EndCamera(m_camera);

	DebugRenderBeginFrame();
	DebugRenderWorldToCamera( &m_camera );

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
	g_theApp->m_game->SetAttenuation( attenuation );

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

void Game::Jump( float jumpIntensity )
{
	Vec3 cameraPosition = m_camera.GetPosition();
	
	if( AlmostEqualsFloat( cameraPosition.y, 0.f ) )
	{
		AddYVelocity( jumpIntensity * 15.f );
	}
}

void Game::AddYVelocity( float yVelocityToAdd )
{
	m_cameraYVelocity += yVelocityToAdd;
}

void Game::AddScreenShake( float screenShakeIntensityToAdd )
{
	float screenShake = m_camera.GetCurrentScreenShakeIntensity();
	screenShake += screenShakeIntensityToAdd;
	m_camera.SetScreenShakeIntensity( screenShake );
}

void Game::UpdateScreenShake( float deltaSeconds )
{
	m_camera.UpdateScreenShake( m_rand );
	float screenShake = m_camera.GetCurrentScreenShakeIntensity();
	screenShake *= 1.f - 2.f * deltaSeconds;//Min(1.f, deltaSeconds);
	if( AlmostEqualsFloat( screenShake, 0.f, 0.05f ) )
	{
		screenShake = 0.f;
	}
	m_camera.SetScreenShakeIntensity( screenShake );
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
	float golemScale = m_map->m_player->m_golemScale;
	Vec3 cameraOffset = Vec3(0.f, 0.f, 15.f * golemScale );
	m_camera.m_cameraOffset = cameraOffset;

	Transform headTransform = m_map->m_player->GetPlayerHeadTransform();
	//Transform chestTransform = m_map->m_player->GetPlayerChestTransform();
	//Mat44 chestMatrix = chestTransform.ToMatrix();
	//Vec3 headPosition = chestMatrix.TransformPosition3D( 0.5f * headTransform.m_position );
	//Vec3 chestPosition = chestMatrix.TransformPosition3D( Vec3() );

	Vec3 headTranslation = m_map->m_player->GetPlayerHeadMatrix().GetTranslation3D();
	m_camera.m_transform = headTransform;
	m_camera.SetPosition( headTranslation );

// 	m_cameraYVelocity -= 10.f * deltaSeconds;
// 	m_cameraYVelocity = Max( -10.f, m_cameraYVelocity );
// 	Vec3 currentCameraPosition = m_camera.GetPosition();
// 	currentCameraPosition.y += m_cameraYVelocity * deltaSeconds;
// 	currentCameraPosition.y = Max( 0.f, currentCameraPosition.y );
// 	m_camera.SetPosition( currentCameraPosition );
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

	//const KeyButtonState& leftArrow = g_theInput->GetKeyStates( 0x25 );
	//const KeyButtonState& upArrow = g_theInput->GetKeyStates( 0x26 );
	//const KeyButtonState& rightArrow = g_theInput->GetKeyStates( 0x27 );
	//const KeyButtonState& downArrow = g_theInput->GetKeyStates( 0x28 );

	//const KeyButtonState& wKey = g_theInput->GetKeyStates( 'W' );
	//const KeyButtonState& aKey = g_theInput->GetKeyStates( 'A' );
	//const KeyButtonState& sKey = g_theInput->GetKeyStates( 'S' );
	//const KeyButtonState& dKey = g_theInput->GetKeyStates( 'D' );
	//const KeyButtonState& cKey = g_theInput->GetKeyStates( 'C' );
	//const KeyButtonState& spaceKey = g_theInput->GetKeyStates( SPACE_KEY );
	//const KeyButtonState& shiftKey = g_theInput->GetKeyStates( SHIFT_KEY );
	const KeyButtonState& f1Key = g_theInput->GetKeyStates( F1_KEY );
	//const KeyButtonState& f2Key = g_theInput->GetKeyStates( F2_KEY );
	//const KeyButtonState& f3Key = g_theInput->GetKeyStates( F3_KEY );
	//const KeyButtonState& f4Key = g_theInput->GetKeyStates( F4_KEY );
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
	const KeyButtonState& uKey = g_theInput->GetKeyStates( 'U' );
	const KeyButtonState& iKey = g_theInput->GetKeyStates( 'I' );
	const KeyButtonState& jKey = g_theInput->GetKeyStates( 'J' );
	const KeyButtonState& kKey = g_theInput->GetKeyStates( 'K' );
	const KeyButtonState& zKey = g_theInput->GetKeyStates( 'Z' );
	const KeyButtonState& xKey = g_theInput->GetKeyStates( 'X' );
	const KeyButtonState& pKey = g_theInput->GetKeyStates( 'P' );
	const KeyButtonState& plusKey = g_theInput->GetKeyStates( PLUS_KEY );
	const KeyButtonState& minusKey = g_theInput->GetKeyStates( MINUS_KEY );
	const KeyButtonState& semiColonKey = g_theInput->GetKeyStates( SEMICOLON_KEY );
	const KeyButtonState& singleQuoteKey = g_theInput->GetKeyStates( SINGLEQUOTE_KEY );
	const KeyButtonState& commaKey = g_theInput->GetKeyStates( COMMA_KEY );
	const KeyButtonState& periodKey = g_theInput->GetKeyStates( PERIOD_KEY );

	if( pKey.WasJustPressed() )
	{
		m_isPaused = !m_isPaused;
	}
	if( f1Key.WasJustPressed() )
	{
		AddScreenShake( 0.5f );
	}
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
		float newFogColorLerp = m_fogColorLerp - 0.5f * deltaSeconds;
		newFogColorLerp = Clampf( newFogColorLerp, 0.f, 1.f );
		m_fogColorLerp = newFogColorLerp;

		float fogMin = m_fogRange * 0.5f;
		float nearFog = m_fogDistance - fogMin;
		float farFog = m_fogDistance + fogMin;

		Rgba8 lerpColor = Rgba8::LerpColorAsHSL( m_fogRed, m_fogBlue, m_fogColorLerp );
		g_theRenderer->EnableFog( nearFog, farFog, lerpColor );
	}
	if( num6Key.IsPressed() )
	{
		float newFogColorLerp = m_fogColorLerp + 0.5f * deltaSeconds;
		newFogColorLerp = Clampf( newFogColorLerp, 0.f, 1.f );
		m_fogColorLerp = newFogColorLerp;

		float fogMin = m_fogRange * 0.5f;
		float nearFog = m_fogDistance - fogMin;
		float farFog = m_fogDistance + fogMin;

		Rgba8 lerpColor = Rgba8::LerpColorAsHSL( m_fogRed, m_fogBlue, m_fogColorLerp );
		g_theRenderer->EnableFog( nearFog, farFog, lerpColor );
	}
	if( num7Key.WasJustPressed() )
	{
		Texture* tex = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
		AABB2 aabb(1.f, 0.f, 0.f, 1.f);
		DebugAddScreenTexturedQuad( AABB2( 500.f, 300.f, 700.f, 500.f ), tex, aabb, Rgba8::WHITE, Rgba8::BLACK, 5.f );
	}
	if( num8Key.WasJustPressed() )
	{
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
		delete m_map;
		m_map = new Map3D( IntVec2( 10, 10 ), this );
		m_map->Startup();

		m_gameTimer.Reset();
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
		ToggleAttenuation();
	}
	if( yKey.WasJustPressed() )
	{
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

// 	Vec3 translator;
// 
// 	if( wKey.IsPressed() )
// 	{
// 		translator.z -=  10.f * deltaSeconds;
// 	}
// 	if( sKey.IsPressed() )
// 	{
// 		translator.z +=  10.f * deltaSeconds;
// 	}
// 	if( aKey.IsPressed() )
// 	{
// 		translator.x -=  10.f * deltaSeconds;
// 	}
// 	if( dKey.IsPressed() )
// 	{
// 		translator.x +=  10.f * deltaSeconds;
// 	}
// 	if( cKey.IsPressed() )
// 	{
// 		translator.y +=  10.f * deltaSeconds;
// 	}
// 	if( spaceKey.WasJustPressed() )
// 	{
// 		Jump( 1.f );
// 	}
// 
// 	if( shiftKey.IsPressed() )
// 	{
// 		translator *= 2.f;
// 	}
// 
// 	m_camera.TranslateRelativeToView( translator );
// 
// 	Vec3 rotator;
// 	if( upArrow.IsPressed() )
// 	{
// 		rotator.x += 10.f * deltaSeconds;
// 	}
// 	if( downArrow.IsPressed() )
// 	{
// 		rotator.x -= 10.f * deltaSeconds;
// 	}
// 	if( leftArrow.IsPressed() )
// 	{
// 		rotator.y += 10.f * deltaSeconds;
// 	}
// 	if( rightArrow.IsPressed() )
// 	{
// 		rotator.y -= 10.f * deltaSeconds;
// 	}
// 
// 	Vec2 mouseChange = g_theInput->GetMouseDeltaPos();
// 
// 	rotator.x -= mouseChange.y * 0.1f;
// 	rotator.y -= mouseChange.x * 0.1f;
// 	m_camera.RotatePitchRollYawDegrees( rotator );

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
