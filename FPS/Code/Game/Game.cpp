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
	//m_camera.SetOrthoView(Vec2(0.f, 0.f), Vec2(GAME_CAMERA_Y* CLIENT_ASPECT, GAME_CAMERA_Y));
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


	m_light.color = Vec3(1.f, 1.f, 1.f);
	m_light.intensity = 0.5f;
	m_light.position = m_camera.GetPosition();
	m_light.specularFactor = 0.5f;
	m_light.specularPower = 2.f;

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

	Mat44 newCircleOfSpheresRotation;
	newCircleOfSpheresRotation.RotateYDegrees( 5.f * dt );
	m_circleOfSpheresModelMatrix.RotateYDegrees( 45.f * dt );
	newCircleOfSpheresRotation.TransformBy( m_circleOfSpheresModelMatrix );
	m_circleOfSpheresModelMatrix = newCircleOfSpheresRotation;

	Vec3 lightColor = m_light.color;
	lightColor *= 255.f;
	Rgba8 lightRGBA8Color = Rgba8( (unsigned char)lightColor.x, (unsigned char)lightColor.y, (unsigned char)lightColor.z );

	DebugAddWorldPoint( m_light.position, lightRGBA8Color, 0.f );

	//<,> cycle shader output name of shader
	//9,0 be able to adjust global ambient light output ambient light intensity
	//T: toggle attenuation output attenuation
	//-,+: Adjust light intensity
	//[,]: Adjust specular factor
	//;,': Adjust specular power
	// F5: Set Light to origin
	// F6: Set Light to camera's location
	// F7: Toggle Light following camera
	// F8: Animated. Light follows a fixed path through environment

	Vec4 ambientLight = g_theRenderer->GetAmbientLight();
	float ambientIntensity = ambientLight.w;
	Vec3 attenuation = g_theRenderer->GetAttenuation();
	float lightIntensity = m_light.intensity;
	float specularFactor = m_light.specularFactor;
	float specularPower = m_light.specularPower;

	std::string cycleShaderStr = Stringf("<,> Cycle Shader: %s", m_shaders[m_currentShaderIndex]->m_filename.c_str() );
	std::string ambientIntensityStr = Stringf("9,0 Adjust Ambient Intensity: %.2f", ambientIntensity );
	std::string attenuationStr = Stringf("T Toggle Attenuation (Constant, Linear, Quadratic): (%.0f, %.0f, %.0f)", attenuation.x, attenuation.y, attenuation.z );
	std::string lightIntensityStr = Stringf("-,+ Adjust Light Intensity: %.2f", lightIntensity );
	std::string specularFactorStr = Stringf("[,] Adjust Specular Factor: %.2f", specularFactor );
	std::string specularPowerStr = Stringf(";,' Adjust Specular Power: %.2f", specularPower );
	std::string lightAtOriginStr = Stringf( "F5 Set Light to Origin" );
	std::string lightToCameraStr = Stringf( "F6 Set Light to Camera Position" );
	std::string lightFollowCameraStr = Stringf( "F7 Set Light to Follow Camera" );
	std::string lightAnimatedStr = Stringf( "F8 Set Light on animated path" );

	DebugAddScreenTextf( Vec4( 0.01f, 0.95f, 0.f, 0.f ), Vec2( 0.f, 0.f ), Rgba8::WHITE, cycleShaderStr.c_str() );
	DebugAddScreenTextf( Vec4( 0.01f, 0.93f, 0.f, 0.f ), Vec2( 0.f, 0.f ), Rgba8::WHITE, ambientIntensityStr.c_str() );
	DebugAddScreenTextf( Vec4( 0.01f, 0.91f, 0.f, 0.f ), Vec2( 0.f, 0.f ), Rgba8::WHITE, attenuationStr.c_str() );
	DebugAddScreenTextf( Vec4( 0.01f, 0.89f, 0.f, 0.f ), Vec2( 0.f, 0.f ), Rgba8::WHITE, lightIntensityStr.c_str() );
	DebugAddScreenTextf( Vec4( 0.01f, 0.87f, 0.f, 0.f ), Vec2( 0.f, 0.f ), Rgba8::WHITE, specularFactorStr.c_str() );
	DebugAddScreenTextf( Vec4( 0.01f, 0.85f, 0.f, 0.f ), Vec2( 0.f, 0.f ), Rgba8::WHITE, specularPowerStr.c_str() );
	DebugAddScreenTextf( Vec4( 0.01f, 0.83f, 0.f, 0.f ), Vec2( 0.f, 0.f ), Rgba8::WHITE, lightAtOriginStr.c_str() );
	DebugAddScreenTextf( Vec4( 0.01f, 0.81f, 0.f, 0.f ), Vec2( 0.f, 0.f ), Rgba8::WHITE, lightToCameraStr.c_str() );
	DebugAddScreenTextf( Vec4( 0.01f, 0.79f, 0.f, 0.f ), Vec2( 0.f, 0.f ), Rgba8::WHITE, lightFollowCameraStr.c_str() );
	DebugAddScreenTextf( Vec4( 0.01f, 0.77f, 0.f, 0.f ), Vec2( 0.f, 0.f ), Rgba8::WHITE, lightAnimatedStr.c_str() );
}

void Game::Render()
{
	g_theRenderer->BeginCamera(m_camera);
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL );
	g_theRenderer->SetBlendMode(BlendMode::ADDITIVE);

/*	g_theRenderer->DrawAABB2(AABB2(Vec2(2.5f,0.25f), Vec2( 9.5f, 6.5f )), Rgba8::GREEN, 0.25f);*/

	Texture* tex = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/example_colour.png");
	Texture* normalTex = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/example_normal.png");
// 	g_theRenderer->SetBlendMode(BlendMode::SOLID);
// 	g_theRenderer->BindTexture(tex);
// 	g_theRenderer->BindShader(m_invertShader);
// 	g_theRenderer->DrawAABB2Filled(AABB2(Vec2(3.25f,1.f), Vec2( 6.f, 3.75f )), Rgba8(255, 255, 255, 128 ));
// 
// 	g_theRenderer->SetBlendMode( BlendMode::ALPHA );
// 	g_theRenderer->BindShader( (Shader*)nullptr );
// 	g_theRenderer->DrawAABB2Filled( AABB2( Vec2( 6.f, 1.f ), Vec2( 8.75f, 3.75f ) ), Rgba8( 255, 255, 255 ) );
// 
// 
// 

 	g_theRenderer->SetBlendMode( BlendMode::SOLID );
 	g_theRenderer->BindTexture( tex );
	g_theRenderer->BindNormal( normalTex );
// 	g_theRenderer->BindShader( m_invertShader );
// 	g_theRenderer->DrawAABB2Filled( AABB2( Vec2( -0.5f, -0.5f ), Vec2( 0.5f, 0.5f ) ), Rgba8( 255, 255, 255, 128 ), -10.f );


	g_theRenderer->DisableLight( 0 );
	g_theRenderer->EnableLight( 0, m_light );
	//g_theRenderer->BindShader( m_litShader );
	g_theRenderer->BindShader( m_shaders[m_currentShaderIndex] );
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

void Game::UpdateLightPosition( float deltaSeconds )
{
	m_lightAnimatedTheta += deltaSeconds * 45.f;
	m_lightAnimatedPhi += deltaSeconds * 90.f;

	Vec2 xzLightPath = Vec2::MakeFromPolarDegrees( m_lightAnimatedTheta, 5.f );
	float yLight = SinDegrees( m_lightAnimatedPhi );

	m_lightAnimatedPosition = Vec3( xzLightPath.x, yLight, xzLightPath.y );
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
	const KeyButtonState& yKey = g_theInput->GetKeyStates( 'Y' );
	const KeyButtonState& qKey = g_theInput->GetKeyStates( 'Q' );
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
		transform.SetNonUniformScale( Vec3( 1.f, 1.f, 5.f ) );
		DebugAddWorldWireBounds( transform, Rgba8::BLUE, Rgba8::RED, 10.f, DEBUG_RENDER_ALWAYS );

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
		float currentSpecularFactor = m_light.specularFactor;

		float newSpecularFactor = currentSpecularFactor - 0.5f * deltaSeconds;
		newSpecularFactor = Clampf( newSpecularFactor, 0.f, 1.f );
		m_light.specularFactor = newSpecularFactor;
	}
	if( rBracketKey.IsPressed() )
	{
		float currentSpecularFactor = m_light.specularFactor;

		float newSpecularFactor = currentSpecularFactor + 0.5f * deltaSeconds;
		newSpecularFactor = Clampf( newSpecularFactor, 0.f, 1.f );
		m_light.specularFactor = newSpecularFactor;
	}
	if( semiColonKey.IsPressed() )
	{
		float currentSpecularPower = m_light.specularPower;

		float newSpecularPower = currentSpecularPower - 20.f * deltaSeconds;
		newSpecularPower = Max( newSpecularPower, 1.f );
		m_light.specularPower = newSpecularPower;
	}
	if( singleQuoteKey.IsPressed() )
	{
		float currentSpecularPower = m_light.specularPower;

		float newSpecularPower = currentSpecularPower + 20.f * deltaSeconds;
		newSpecularPower = Max( newSpecularPower, 1.f );
		m_light.specularPower = newSpecularPower;
	}
	if( commaKey.WasJustPressed() )
	{
		DecrementShader();
	}
	if( periodKey.WasJustPressed() )
	{
		IncrementShader();
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
		g_theRenderer->ToggleAttenuation();
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
		DebugAddWireMeshToWorld( cameraModel, m_sphereMesh, Rgba8::RED, Rgba8::BLUE, 45.f, DEBUG_RENDER_USE_DEPTH );
	}
	if( plusKey.IsPressed() )
	{
		float currentLightIntensity = m_light.intensity;

		float newLightIntensity = currentLightIntensity + 0.5f * deltaSeconds;
		newLightIntensity = Clampf( newLightIntensity, 0.f, 1.f );
		m_light.intensity = newLightIntensity;
	}
	if( minusKey.IsPressed() )
	{
		float currentLightIntensity = m_light.intensity;

		float newLightIntensity = currentLightIntensity - 0.5f * deltaSeconds;
		newLightIntensity = Clampf( newLightIntensity, 0.f, 1.f );
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
	m_light.position = pos;
}

void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}
