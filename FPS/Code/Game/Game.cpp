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
	m_camera.SetProjectionPerspective( 60.f, -0.1f, -100.f );

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


	m_loadedMeshModelMatrix = Mat44::CreateTranslation3D( Vec3( 0.f, 0.f, -10.f ) );
	m_sphereModelMatrix = Mat44::CreateTranslation3D( Vec3( 4.f, 0.f, -10.f ) );
	m_triPlanarSphereModelMatrix = Mat44::CreateTranslation3D( Vec3( 8.f, 0.f, -10.f ) );
	m_quadModelMatrix = Mat44::CreateTranslation3D( Vec3( -4.f, 0.f, -10.f ) );
	m_circleOfSpheresModelMatrix = Mat44::CreateTranslation3D( Vec3( 0.f, 0.f, -20.f ) );
	m_numberOfCirclingCubes = 18;

	m_loadedMesh = new GPUMesh( g_theRenderer );

	std::vector<Vertex_PCUTBN> loadedVerts;
	std::vector<uint> loadedIndices;
	MeshImportOptions_t options;
	options.m_transform.SetUniformScale( 0.2f );
	options.m_invertV = true;
	options.m_invertWindingOrder = false;
	options.m_generateNormals = false;
	options.m_generateTangents = true;
	LoadOBJToVertexArray( loadedVerts, loadedIndices, "Data/Meshes/SciFi_Fighter.obj", options );
	//Vertex_PCUTBN::AppendIndexedVertsCube( cubeVerts, cubeIndices, 1.f );
	m_loadedMesh->UpdateVertices( loadedVerts );
	m_loadedMesh->UpdateIndices( loadedIndices );


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

	m_lights[1].intensity = 0.5;
	m_lights[1].position = m_quadModelMatrix.GetTranslation3D();
	m_lights[1].position.z += 0.5f;
	m_lights[1].cosInnerAngle = 0.95f;
	m_lights[1].cosOuterAngle = 0.93f;

	m_lights[2].intensity = 0.3f;
	m_lights[2].position = Vec3 ( 0.f, 4.f, -5.f );
	m_lights[2].direction = Vec3(0.f, -1.f, 0.f );
	m_lights[2].isDirectional = 1.f;
	m_lights[2].attenuation = Vec3( 1.f, 0.f, 0.f );
	m_lights[2].color = Vec3( 1.f, 1.f, 0.f );


	g_theRenderer->SetSpecularFactorAndPower( 0.5f, 2.f );

	g_theRenderer->SetAmbientLight( Rgba8::WHITE, 0.f );

	g_theEventSystem->SubscribeToEvent( "light_set_ambient_color", CONSOLECOMMAND, SetAmbientColor );
	g_theEventSystem->SubscribeToEvent( "light_set_attenuation", CONSOLECOMMAND, SetAttenuation );
	g_theEventSystem->SubscribeToEvent( "light_set_color", CONSOLECOMMAND, SetLightColor );
}

void Game::Shutdown()
{

	delete m_loadedMesh;
	m_loadedMesh = nullptr;

	delete m_sphereMesh;
	m_sphereMesh = nullptr;

	delete m_quadMesh;
	m_quadMesh = nullptr;

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
	clearColor.r = 10;
	clearColor.b = 10;

	m_camera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT, clearColor, 0.f, 0 );

	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses( dt );
	}

	UpdateLightPosition( dt );


// 	m_cubeModelMatrix.RotateYDegrees( 45.f * dt );
// 	m_cubeModelMatrix.RotateXDegrees( 30.f * dt );
	m_sphereModelMatrix.RotateXDegrees( 30.f * dt );
	m_sphereModelMatrix.RotateYDegrees( -15.f * dt );
	m_triPlanarSphereModelMatrix.RotateXDegrees( 30.f * dt );
	//m_quadModelMatrix.RotateZDegrees( 35.f * dt ); //if you want the quad to rotate

	Mat44 newCircleOfSpheresRotation;
	newCircleOfSpheresRotation.RotateYDegrees( 5.f * dt );
	m_circleOfSpheresModelMatrix.RotateYDegrees( 45.f * dt );
	//newCircleOfSpheresRotation.TransformBy( m_circleOfSpheresModelMatrix );
	//m_circleOfSpheresModelMatrix = newCircleOfSpheresRotation;


	for( size_t lightIndex = 0; lightIndex < m_lights.size(); lightIndex++ )
	{
		Vec3 lightColor = m_lights[lightIndex].color;
		lightColor *= 255.f;
		Rgba8 lightRGBA8Color = Rgba8( (unsigned char)lightColor.x, (unsigned char)lightColor.y, (unsigned char)lightColor.z );

		if( !m_isLightFollowingCamera && m_lights[lightIndex].intensity >= 0.0001f )
		{
			if( m_lights[lightIndex].isDirectional == 1.f )
			{
				Vec3 startPosition = m_lights[lightIndex].position;
				Vec3 direction = m_lights[lightIndex].direction;
				Vec3 endPosition = startPosition + direction;
				LineSegment3 lightDir( startPosition, endPosition );
				DebugAddWorldArrow( lightDir, lightRGBA8Color, 0.f );
			}
			else
			{
				DebugAddWorldPoint( m_lights[lightIndex].position, lightRGBA8Color, 0.f );
			}
		}
	}



	Vec4 ambientLight = g_theRenderer->GetAmbientLight();
	float ambientIntensity = ambientLight.w;
	Vec3 attenuation = m_lights[m_currentLightIndex].attenuation;
	float lightIntensity = m_lights[m_currentLightIndex].intensity;
	float specularFactor = g_theRenderer->GetSpecularFactor();
	float specularPower = g_theRenderer->GetSpecularPower();
	float gamma = g_theRenderer->GetGamma();
	float nearFog = g_theRenderer->m_fogNear;
	float farFog = g_theRenderer->m_fogFar;
	Vec3 fogColor = g_theRenderer->m_fogColor;

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

	light_t const& currentLight = m_lights[m_currentLightIndex];

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
	std::string currentCosAnglesStr = Stringf( "1,2 Adjust Spotlight Cos Angles: Inner %.2f, Outer %.2f", currentLight.cosInnerAngle, currentLight.cosOuterAngle );
	std::string toggleDirectionalStr = Stringf( "Z Toggle Directional Light: %.0f", currentLight.isDirectional );
	std::string nearFarFogStr = Stringf( "3,4 Adjust Fog: Near %.2f, Far %.2f", nearFog, farFog );
	std::string fogColorStr = Stringf( "5,6 Adjust Fog color: %.2f, %.2f, %.2f", fogColor.x, fogColor.y, fogColor.z );
	std::string dissolveStr = Stringf( "U,I Adjust dissolve height: %.2f", m_dissolveAmount );
	std::string lightIndexStr = Stringf( "J,K Cycle current light to adjust: %i", m_currentLightIndex );
	std::string projectionNoteStr = Stringf( "Note: Projection follows first light, so use F7 to make it follow camera");

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
	DebugAddScreenText( Vec4( 0.01f, 0.69f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, currentCosAnglesStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.67f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, toggleDirectionalStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.65f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, nearFarFogStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.63f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, fogColorStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.61f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, dissolveStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.59f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, lightIndexStr.c_str() );
	DebugAddScreenText( Vec4( 0.01f, 0.57f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 15.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, projectionNoteStr.c_str() );
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
	EnableLights();

	dissolve_t* dissolveData = m_testMaterial->GetDataAs<dissolve_t>();
	dissolveData->amount = m_dissolveAmount;
	m_testMaterial->SetData( *dissolveData );

	g_theRenderer->SetModelMatrix( m_loadedMeshModelMatrix );
	g_theRenderer->BindMaterial( m_testMaterial );
	g_theRenderer->DrawMesh( m_loadedMesh );

	g_theRenderer->SetBlendMode( eBlendMode::SOLID );
	g_theRenderer->SetModelMatrix( m_quadModelMatrix );
	g_theRenderer->DrawMesh( m_quadMesh );

	g_theRenderer->SetModelMatrix( m_sphereModelMatrix );
	g_theRenderer->DrawMesh( m_sphereMesh );

	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL );
	fresnel_t fresnel;
	fresnel.color = Vec3( 0.f, 1.f, 0.f );
	fresnel.power = 16.f;
	fresnel.factor = 1.f;

	g_theRenderer->SetMaterialData( &fresnel, sizeof(fresnel) );
	Shader* fresnelShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/Fresnel.hlsl" );
	g_theRenderer->BindShader( fresnelShader );
	g_theRenderer->SetModelMatrix( m_sphereModelMatrix );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_EQUAL, eDepthWriteMode::WRITE_NONE );
	g_theRenderer->DrawMesh( m_sphereMesh );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_LESS_THAN_OR_EQUAL );

	Texture* tex0_d = g_theRenderer->CreateOrGetTextureFromFile( /*"Data/Images/test.png"*/"Data/Images/forrest_ground_01_diff_1k.png" );
	Texture* tex0_n = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/forrest_ground_01_nor_1k.png" );
	Texture* tex1_d = g_theRenderer->CreateOrGetTextureFromFile( /*"Data/Images/test.png"*/"Data/Images/sand_01_diff_1k.png" );
	Texture* tex1_n = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/sand_01_nor_1k.png" );
	Texture* tex2_d = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/rock_06_diff_1k.png" );
	Texture* tex2_n = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/rock_06_nor_1k.png" );

	g_theRenderer->BindDataTexture( 8,  tex0_d );
	g_theRenderer->BindDataTexture( 9,  tex0_n );
	g_theRenderer->BindDataTexture( 10, tex1_d );
	g_theRenderer->BindDataTexture( 11, tex1_n );
	g_theRenderer->BindDataTexture( 12, tex2_d );
	g_theRenderer->BindDataTexture( 13, tex2_n );

	Shader* triPlanarShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/TriPlanar.hlsl" );
	g_theRenderer->BindShader( triPlanarShader );
	g_theRenderer->SetModelMatrix( m_triPlanarSphereModelMatrix );
	g_theRenderer->DrawMesh( m_sphereMesh );

	g_theRenderer->SetBlendMode( eBlendMode::SOLID );
	g_theRenderer->BindShader( m_shaders[m_currentShaderIndex] );
	RenderCircleOfSpheres();

	//RenderProjection();

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->EndCamera(m_camera);

	DebugRenderBeginFrame();
	DebugRenderWorldToCamera( &m_camera );

	//Setup camera that is identity
	// bind a shader for the effect
	// bind "src" as an input
	//bind other inputs
	//Texture* backbuffer = g_theRenderer->GetBackBuffer();

	Shader* shaderEffect = g_theRenderer->GetOrCreateShader( "Data/Shaders/ImageEffect.hlsl" );
	g_theRenderer->StartEffect( backbuffer, colorTarget, shaderEffect);
	g_theRenderer->EndEffect();

	//g_theRenderer->CopyTexture( backbuffer, frameTarget );
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


void Game::RenderCircleOfSpheres()
{
	Texture* couchDiffuse = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/example_colour.png");
	Texture* couchNormal = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/example_normal.png");
	//Texture* couchSpec = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/example_spec.png");

	g_theRenderer->BindTexture( couchDiffuse );
	g_theRenderer->BindNormal( couchNormal );

	float degreeIncrements = 360.f / (float)m_numberOfCirclingCubes;
	Mat44 circleOfSpheres = m_circleOfSpheresModelMatrix;
	for( float currentDegreeIncrement = 0.f; currentDegreeIncrement < 360.f; currentDegreeIncrement += degreeIncrements )
	{
		g_theRenderer->SetModelMatrix( circleOfSpheres );
		g_theRenderer->DrawMesh( m_sphereMesh );

		Mat44 nextModelMatrix;
		nextModelMatrix.RotateYDegrees( degreeIncrements );
		nextModelMatrix.TransformBy( circleOfSpheres );
		circleOfSpheres = nextModelMatrix;
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
}

void Game::RenderGame()
{
	//m_world->Render();
}

void Game::RenderProjection()
{
	//Projection
	Texture* projectionTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/chi.png" );
	g_theRenderer->BindDataTexture( 14, projectionTexture );
	g_theRenderer->SetBlendMode( eBlendMode::ADDITIVE );
	Mat44 lightView0 = LookAtAndMoveToWorld( m_lights[0].position, m_lights[0].position + m_lights[0].direction, Vec3( 0.f, 1.f, 0.f) );
	MatrixInvertOrthoNormal( lightView0 );
	
	Mat44 newProjectionMatrix = MakePerspectiveProjectMatrixD3D( 30.f, 1.f, -0.1f, -1000.f );
	Mat44 viewMatrix = m_camera.GetViewMatrix();
	newProjectionMatrix.TransformBy( lightView0 );
	
	projection_t projectionData;
	projectionData.projection = newProjectionMatrix;
	projectionData.position = m_lights[0].position;
	projectionData.strength = 1.f;
	
	g_theRenderer->SetMaterialData( &projectionData, sizeof(projectionData) );
	Shader* projectionShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/Projection.hlsl");
	g_theRenderer->BindShader( projectionShader );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_EQUAL, eDepthWriteMode::WRITE_NONE );
	g_theRenderer->SetModelMatrix( m_loadedMeshModelMatrix );
	g_theRenderer->DrawMesh( m_loadedMesh );
	g_theRenderer->SetModelMatrix( m_quadModelMatrix );
	g_theRenderer->DrawMesh( m_quadMesh );
	g_theRenderer->SetModelMatrix( m_sphereModelMatrix );
	g_theRenderer->DrawMesh( m_sphereMesh );
	g_theRenderer->SetModelMatrix( m_triPlanarSphereModelMatrix );
	g_theRenderer->DrawMesh( m_sphereMesh );
	RenderCircleOfSpheres();
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
		m_testMaterial->m_specularFactor = newSpecularFactor;
	}
	if( rBracketKey.IsPressed() )
	{
		float currentSpecularFactor = g_theRenderer->GetSpecularFactor();

		float newSpecularFactor = currentSpecularFactor + 0.5f * deltaSeconds;
		newSpecularFactor = Clampf( newSpecularFactor, 0.f, 1.f );
		g_theRenderer->SetSpecularFactor( newSpecularFactor );
		m_testMaterial->m_specularFactor = newSpecularFactor;
	}
	if( semiColonKey.IsPressed() )
	{
		float currentSpecularPower = g_theRenderer->GetSpecularPower();

		float newSpecularPower = currentSpecularPower - 20.f * deltaSeconds;
		newSpecularPower = Max( newSpecularPower, 1.f );
		g_theRenderer->SetSpecularPower( newSpecularPower );
		m_testMaterial->m_specularPower = newSpecularPower;
	}
	if( singleQuoteKey.IsPressed() )
	{
		float currentSpecularPower = g_theRenderer->GetSpecularPower();

		float newSpecularPower = currentSpecularPower + 20.f * deltaSeconds;
		newSpecularPower = Max( newSpecularPower, 1.f );
		g_theRenderer->SetSpecularPower( newSpecularPower );
		m_testMaterial->m_specularPower = newSpecularPower;
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
		ToggleAttenuation();
	}
	if( yKey.WasJustPressed() )
	{
		Mat44 cameraModel = m_camera.GetCameraModelMatrix();
		DebugAddWireMeshToWorld( cameraModel, m_sphereMesh, Rgba8::RED, Rgba8::BLUE, 45.f, DEBUG_RENDER_USE_DEPTH );
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
	m_lights[m_currentLightIndex].position = pos;
}

void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}
