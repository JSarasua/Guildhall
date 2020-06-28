#pragma once
#include "Game/Game.hpp"
#include "App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/World.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Actor.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Game/BulletDefinition.hpp"



Game::Game() : m_imageTest(IMAGETESTPATH)
{
	m_camera = Camera();
	for( int cameraIndex = 0; cameraIndex < 4; cameraIndex++ )
	{
		m_cameras.push_back(Camera());
	}
	m_UICamera = Camera();

	m_world = new World(this);
}

Game::~Game(){}

void Game::Startup()
{
	//LoadAssets();
	//InitializeDefinitions();
	//m_world->Startup();
	//m_player = m_world->GetPlayer();
	g_theRenderer->CreateOrGetBitmapFont("Fonts/SquirrelFixedFont.png");
	m_numTilesInViewVertically = GAME_CAMERA_Y;
	m_numTilesInViewHorizontally = GAME_CAMERA_Y * CLIENT_ASPECT;

	EnableDebugRendering();
	m_camera = Camera();
	m_camera.SetColorTarget( nullptr ); // we use this
	m_camera.CreateMatchingDepthStencilTarget( g_theRenderer );
	m_camera.SetOutputSize( Vec2(m_numTilesInViewHorizontally, m_numTilesInViewVertically) );
	m_camera.SetProjectionOrthographic( m_camera.m_outputSize, 0.f, 100.f );

	m_UICamera = Camera();
	m_UICamera.SetColorTarget( nullptr ); // we use this
	m_UICamera.SetOutputSize( Vec2(UI_CAMERA_X, UI_CAMERA_Y) );
	m_UICamera.SetProjectionOrthographic( m_UICamera.m_outputSize, 0.f, 100.f );
	m_UICamera.SetPosition( Vec2(UI_CAMERA_X, UI_CAMERA_Y) * 0.5f );


	Rgba8 clearColor = Rgba8::BLACK;
	m_camera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT, clearColor, 0.f, 0 );

	g_theInput->HideSystemCursor();
}

void Game::Shutdown(){}

void Game::RunFrame(){}

void Game::Update( float deltaSeconds )
{
	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses( deltaSeconds );
	}

	switch( m_gameState )
	{
	case LOADING: UpdateLoading( deltaSeconds );
		break;
	case ATTRACT: UpdateAttract( deltaSeconds );
		break;
	case PLAYING: UpdatePlaying( deltaSeconds );
		break;
	default: ERROR_AND_DIE( "Invalid Game State" );
		break;
	}



	//UpdateCameras();
// 	UpdateConsoleTest( deltaSeconds );
// 	UpdateAlignedTextTest( deltaSeconds );
// 	UpdateImageTest(deltaSeconds);
// 	UpdateBlackboardTest(deltaSeconds);

}

void Game::Render()
{
	switch( m_gameState )
	{
	case LOADING: RenderLoading();
		break;
	case ATTRACT: RenderAttract();
		break;
// 	case DEATH: RenderDeath();
// 		break;
// 	case VICTORY: RenderVictory();
// 		break;
// 	case PAUSED: RenderPaused();
// 		break;
	case PLAYING: RenderPlaying();
		break;
	default: ERROR_AND_DIE( "Invalid Game State" );
		break;
	}



}


Vec2 Game::GetMousePositionOnMainCamera()
{
	return m_mousePositionOnMainCamera;
}

void Game::LoadAssets()
{
	//g_theRenderer->CreateOrGetBitmapFont("Fonts/SquirrelFixedFont.png");
	g_theRenderer->CreateOrGetTextureFromFile(IMAGETESTPATH);

	Texture* tileSpriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	Texture* actorSpriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/bender_12x11.png" );
	Texture* portraitSpriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/KushnariovaPortraits_8x8.png" );
	Texture* weaponSpriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/weapons.png" );
	Texture* bulletsSpriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Extras_4x4.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/reticle.png" );
	g_tileSpriteSheet = new SpriteSheet(*tileSpriteSheetTexture,IntVec2(8,8));
	g_actorSpriteSheet = new SpriteSheet(*actorSpriteSheetTexture, IntVec2(12,11));
	g_portraitSpriteSheet = new SpriteSheet(*portraitSpriteSheetTexture, IntVec2(8,8));
	g_weaponSpriteSheet = new SpriteSheet(*weaponSpriteSheetTexture, IntVec2(12, 12) );
	g_bulletsSpriteSheet = new SpriteSheet( *bulletsSpriteSheetTexture, IntVec2( 4, 4 ) );
}

void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	IntVec2 currentMapBounds = GetCurrentMapBounds();

	if( g_theApp->GetDebugCameraMode() )
	{
		UNIMPLEMENTED();
// 		if( currentMapBounds.x > currentMapBounds.y * CLIENT_ASPECT )
// 		{
// 			m_camera.SetOrthoView( Vec2( 0.f, 0.f ), Vec2((float)currentMapBounds.x, (float)currentMapBounds.x/CLIENT_ASPECT ) );
// 		}
// 		else
// 		{
// 			m_camera.SetOrthoView( Vec2( 0.f, 0.f ), Vec2((float)currentMapBounds.y * CLIENT_ASPECT, (float)currentMapBounds.y ) );
// 		}
	}
	else
	{
		//Using the map order, clamp to an imaginary smaller border including half the game camera size
		Vec2 cameraPosition = m_player->GetPosition();
		cameraPosition.x = Clampf( cameraPosition.x, m_numTilesInViewHorizontally/2.f, currentMapBounds.x - m_numTilesInViewHorizontally/2.f );
		cameraPosition.y = Clampf( cameraPosition.y, m_numTilesInViewVertically/2.f, currentMapBounds.y - m_numTilesInViewVertically/2.f );

		m_camera.SetPosition( cameraPosition );
	}

	m_camera.UpdateScreenShake( m_rand );
	float currentScreenShake = m_camera.GetCurrentScreenShakeIntensity();
	currentScreenShake -= currentScreenShake * deltaSeconds + deltaSeconds;
	currentScreenShake = Max( currentScreenShake, 0.f );
	m_camera.SetScreenShakeIntensity( currentScreenShake );
}

void Game::UpdateCamera( const Vec2& centerPosition, Camera& camera )
{
	IntVec2 currentMapBounds = GetCurrentMapBounds();

	//Using the map order, clamp to an imaginary smaller border including half the game camera size
	Vec2 cameraPosition = centerPosition;
	cameraPosition.x = Clampf( cameraPosition.x, m_numTilesInViewHorizontally/2.f, currentMapBounds.x - m_numTilesInViewHorizontally/2.f );
	cameraPosition.y = Clampf( cameraPosition.y, m_numTilesInViewVertically/2.f, currentMapBounds.y - m_numTilesInViewVertically/2.f );

	camera.SetPosition( cameraPosition );
}

void Game::UpdateCameras()
{
	std::vector<Actor*> players;
	m_world->GetPlayers(players);
	for( int cameraIndex = 0; cameraIndex < m_cameras.size(); cameraIndex++ )
	{
// 		if( cameraIndex == 4 )
// 		{
// 			IntVec2 currentMapBounds = GetCurrentMapBounds();
// 			if( currentMapBounds.x > currentMapBounds.y* CLIENT_ASPECT )
// 			{
// 				m_camera.SetOrthoView( Vec2( 0.f, 0.f ), Vec2( (float)currentMapBounds.x, (float)currentMapBounds.x/CLIENT_ASPECT ) );
// 			}
// 			else
// 			{
// 				m_camera.SetOrthoView( Vec2( 0.f, 0.f ), Vec2( (float)currentMapBounds.y * CLIENT_ASPECT, (float)currentMapBounds.y ) );
// 			}
// 		}
		if( players[cameraIndex] )
		{
		UpdateCamera(players[cameraIndex]->GetPosition(), m_cameras[cameraIndex]);
		}
	}
}

void Game::RenderGame()
{
	m_world->Render();

	//RenderDebugMouse();
}

void Game::RenderMouse()
{
	AABB2 mouseAABB;
	mouseAABB.SetCenter( m_mousePositionOnMainCamera );
	mouseAABB.SetDimensions( Vec2( 0.5f, 0.5f ) );
	Texture* reticle = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/reticle.png" );
	g_theRenderer->BindTexture( reticle );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->DrawAABB2Filled( mouseAABB, Rgba8::WHITE );
}

void Game::RenderUI()
{
// 	RenderAlignedTextTest();
// 	RenderImageTest();
// 	RenderBlackboardTest();
// 	RenderMouseTest();
//	m_world->RenderDebug();
	RenderConsole();
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	UNUSED( deltaSeconds );

	XboxController const& controller = g_theInput->GetXboxController(0);
	
	KeyButtonState const& leftMouseButton = g_theInput->GetMouseButton( LeftMouseButton );
	KeyButtonState const& spaceKey = g_theInput->GetKeyStates( SPACE_KEY );

	if( m_gameState == ATTRACT )
	{
		if( leftMouseButton.WasJustPressed() || spaceKey.WasJustPressed() )
		{
			m_gameState = PLAYING;
			g_theApp->UnPauseGame();
		}
	}
	if( m_gameState == PLAYING )
	{
		if( controller.GetButtonState( XBOX_BUTTON_ID_Y ).WasJustPressed() )
		{
			g_theConsole->SetIsOpen( !g_theConsole->IsOpen() );
		}

		const KeyButtonState& f5Key = g_theInput->GetKeyStates( 0x74 );
		if( f5Key.WasJustPressed() )
		{
			RebuildWorld();
		}
	}

	



}

IntVec2 Game::GetCurrentMapBounds() const
{
	return m_world->getCurrentMapBounds();
}



void Game::RebuildWorld()
{
	m_world->Shutdown();
	delete m_world;

	m_world = new World(this);
	m_world->Startup();
}

AABB2 Game::GetUICamera() const
{
	AABB2 uiCameraBox = AABB2(m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight());
	return uiCameraBox;
}

AABB2 Game::GetCamera() const
{
	AABB2 cameraBox = AABB2( m_camera.GetOrthoBottomLeft(), m_camera.GetOrthoTopRight() );
	return cameraBox;
}

void Game::RenderDebugMouse()
{
	AABB2 gameCamera = AABB2(m_camera.GetOrthoBottomLeft(), m_camera.GetOrthoTopRight());
	AABB2 topBox = gameCamera.GetBoxAtTop(0.1f);
	AABB2 topRightBox = topBox.GetBoxAtRight(0.2f);

	std::string mousePositionString = Stringf("Mouse Position: %f, %f, Camera Position: %f, %f", m_mousePositionOnMainCamera.x, m_mousePositionOnMainCamera.y, m_camera.GetOrthoBottomLeft().x, m_camera.GetOrthoBottomLeft().y);

	g_theRenderer->DrawAlignedTextAtPosition(mousePositionString.c_str(), topRightBox, 0.1f, ALIGN_CENTER_RIGHT);
}

void Game::RenderConsole() const
{
	g_theConsole->Render(*g_theRenderer,m_UICamera,TESTTEXTLINEHEIGHT);
}

void Game::AddAlignedTextTest()
{
	m_isAlignedTextTestActive = true;

	Vec2 topRight(155.f,40.f);
	Vec2 bottomLeft(90.f,20.f);
	m_alignedTextTestBox = AABB2(bottomLeft, topRight);
}

void Game::AddImageTest()
{
	m_isImageTestActive = true;

	Vec2 topRightText( 155.f, 55.f );
	Vec2 bottomLeftText( 90.f, 45.f );
	m_imageTextTestBox = AABB2( bottomLeftText, topRightText );

	Vec2 topRightImageBox(120.f, 85.f);
	Vec2 bottomLeftImageBox(90.f, 55.f);

	m_imageTestBox = AABB2(bottomLeftImageBox, topRightImageBox);

	g_theRenderer->CreateOrGetTextureFromFile(IMAGETESTPATH);
}

void Game::AddBlackboardTest()
{
	m_isBlackboardTestActive = true;


	Vec2 topRight( 150.f, 12.f );
	Vec2 bottomLeft( 30.f, 10.f );
	m_blackBoardTextBox = AABB2( bottomLeft, topRight );

	std::string startLevel = g_gameConfigBlackboard->GetValue("startLevel", "Default StartValue");
	float windowAspect = g_gameConfigBlackboard->GetValue("windowAspect", 0.f);
	bool isFullscreen = g_gameConfigBlackboard->GetValue("isFullscreen", true);
	g_gameConfigBlackboard->SetValue("NewSetValue","Are you still there?");
	std::string newSetValue = g_gameConfigBlackboard->GetValue("NewSetValue", "Set String fail");

	m_blackBoardText = Stringf("Start level: %s, WindowAspect: %f, isFullScreen: %d, newSetValue: %s", startLevel.c_str(), windowAspect, isFullscreen, newSetValue.c_str());
}

void Game::AddScreenShake( float screenShakeIncrement )
{
	screenShakeIncrement += m_camera.GetCurrentScreenShakeIntensity();
	m_camera.SetScreenShakeIntensity( screenShakeIncrement );
}

void Game::UpdateConsoleTest( float deltaSeconds )
{
	g_theConsole->Update();
// 	static float consoleTimer = 0.f;
// 
// 	consoleTimer += deltaSeconds;
// 
// 	if( consoleTimer > 0.2f )
// 	{
// 		g_theConsole->PringString( Rgba8::GREEN, "Are you still there?" );
// 		consoleTimer = 0.f;
// 	}
}

void Game::UpdateAlignedTextTest( float deltaSeconds )
{
	if( !m_isAlignedTextTestActive )
	{
		return;
	}
	static float alignment = 0.f;

	alignment += deltaSeconds;

	if( alignment < 1.f )
	{
		m_alignment.x = alignment;
		m_alignment.y = 0.f;
	}
	else if( alignment < 2.f )
	{
		m_alignment.x = 1.f;
		m_alignment.y = alignment - 1.f;
	}
	else if( alignment < 3.f )
	{
		m_alignment.x = 3.f - alignment;
		m_alignment.y = 1.f;
	}
	else if( alignment < 4.f )
	{
		m_alignment.x = 0.f;
		m_alignment.y = 4.f - alignment;
	}
	else if( alignment < 5.f )
	{
		m_alignment.x = alignment - 4.f;
		m_alignment.y = alignment - 4.f;
	}
	else
	{
		alignment = 2.f;
	}

	m_alignedTestText = Stringf("Alignment: (%f,%f)",m_alignment.x, m_alignment.y);
}

void Game::UpdateImageTest( float deltaSeconds )
{
	if( !m_isImageTestActive )
	{
		return;
	}

	static float imageTestTimer = 0.f;
	imageTestTimer += deltaSeconds;

	int xDimension = m_imageTest.GetDimensions().x - 1;
	int xIndex = ClampInt((int)imageTestTimer,0,xDimension);

	m_pixelColor = m_imageTest.GetTexelColor( IntVec2( xIndex, 0 ) );
	m_pixelToString = Stringf( "Pixel number: %i = R: %uc, G: %uc, B: %uc, A: %uc",xIndex, m_pixelColor.r, m_pixelColor.g, m_pixelColor.b, m_pixelColor.a );

	if( xIndex >= xDimension )
	{
		imageTestTimer = 0.f;
	}
}

void Game::UpdateBlackboardTest( float deltaSeconds )
{
	if( !m_isBlackboardTestActive )
	{
		return;
	}

	static float blackBoardTimer = 0.f;
	blackBoardTimer += deltaSeconds;

	if( blackBoardTimer > 1.f )
	{
		g_gameConfigBlackboard->SetValue( "stop", "Please stop" );
		std::string stopString = g_gameConfigBlackboard->GetValue( "stops", "But I won't stop" );
		m_blackBoardText = m_blackBoardText + " Stop?: " + stopString;
		blackBoardTimer = 0.f;
	}
}

void Game::RenderAlignedTextTest() const
{
	if( !m_isAlignedTextTestActive )
	{
		return;
	}

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawAABB2Filled(m_alignedTextTestBox,Rgba8(0,0,0,128));

	g_theRenderer->DrawAlignedTextAtPosition(m_alignedTestText.c_str(), m_alignedTextTestBox, TESTTEXTLINEHEIGHT, m_alignment);
}

void Game::RenderImageTest() const
{
	if( !m_isImageTestActive )
	{
		return;
	}
	g_theRenderer->DrawAlignedTextAtPosition(m_pixelToString.c_str(), m_imageTextTestBox, TESTTEXTLINEHEIGHT, Vec2(0.f,0.f));

	Texture* testImage = g_theRenderer->CreateOrGetTextureFromFile(IMAGETESTPATH);
	g_theRenderer->BindTexture(testImage);
	g_theRenderer->DrawAABB2Filled(m_imageTestBox, Rgba8::WHITE);
}

void Game::RenderBlackboardTest() const
{
	if( !m_isBlackboardTestActive )
	{
		return;
	}

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawAABB2Filled( m_blackBoardTextBox, Rgba8( 0, 0, 0, 128 ) );

	g_theRenderer->DrawAlignedTextAtPosition("Blackboard test", m_blackBoardTextBox, TESTTEXTLINEHEIGHT, Vec2(0.f, 1.f));
	g_theRenderer->DrawAlignedTextAtPosition(m_blackBoardText.c_str(), m_blackBoardTextBox, TESTTEXTLINEHEIGHT, Vec2(0.f, 0.f));
}

void Game::InitializeDefinitions()
{
	m_mapDefDoc		= new XmlDocument;
	m_tileDefDoc	= new XmlDocument;
	m_actorDefDoc	= new XmlDocument;
	XmlDocument bulletDefDoc;
	XmlDocument weaponDefDoc;

	const XmlElement& tileDef = GetRootElement(*m_tileDefDoc, "Data/Gameplay/TileDefs.xml");
	const XmlElement& mapDef = GetRootElement(*m_mapDefDoc, "Data/Gameplay/MapDefs.xml");
	const XmlElement& actorDef = GetRootElement(*m_actorDefDoc, "Data/Gameplay/Actors.xml");
	XmlElement const& bulletsDef = GetRootElement( bulletDefDoc, "Data/Gameplay/Bullets.xml");
	XmlElement const& weaponDef = GetRootElement( weaponDefDoc, "Data/Gameplay/Weapons.xml");

	TileDefinition::InitializeTileDefinitions(tileDef);
	MapDefinition::InitializeMapDefinitions(mapDef);
	ActorDefinition::InitializeActorDefinitions(actorDef);
	BulletDefinition::InitializeBulletDefinitions( bulletsDef );
	WeaponDefinition::InitializeWeaponDefinitions( weaponDef );
}

void Game::RenderMouseTest() const
{
	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedPos();

	AABB2 orthoBounds(m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight());
	//Should pass in camera
	//Use UV because our value is between 0 to 1
	Vec2 mouseDrawPos = orthoBounds.GetPointAtUV(mouseNormalizedPos);
	g_theRenderer->DrawRing(mouseDrawPos, 1.f, Rgba8::GREEN, 0.1f);
}

void Game::UpdateDebugMouse()
{

	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedPos();
	AABB2 orthoBounds( m_camera.GetOrthoBottomLeft(), m_camera.GetOrthoTopRight() );
	//Should pass in camera
	//Use UV because our value is between 0 to 1
	Vec2 mouseDrawPosOnCamera = orthoBounds.GetPointAtUV( mouseNormalizedPos );

	m_mousePositionOnMainCamera = mouseDrawPosOnCamera;
}

void Game::UpdateLoading( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	g_theApp->PauseGame();

	//CheckButtonPressesLoading( deltaSeconds );


	if( m_frameCounter > 1 )
	{
		m_world->Update( 0.f );
		m_gameState = ATTRACT;
	}
	else if( m_frameCounter == 1 )
	{
		LoadAssets();
		InitializeDefinitions();
		m_world->Startup();
		m_player = m_world->GetPlayer();
	}
	else if( m_frameCounter == 0 )
	{
		SoundID loadingSound = g_theAudio->CreateOrGetSound( "Data/Audio/Anticipation.mp3" );
		g_theAudio->PlaySound( loadingSound );
	}

	m_frameCounter++;
}

void Game::UpdateAttract( float deltaSeconds )
{
	m_world->Update( 0.f );
}

void Game::UpdatePlaying( float deltaSeconds )
{
	m_world->Update( deltaSeconds );
	UpdateCamera( deltaSeconds );
	UpdateDebugMouse();
}

void Game::RenderLoading()
{
	g_theRenderer->ClearScreen( Rgba8::BLACK );

	Texture* backbuffer = g_theRenderer->GetBackBuffer();
	Texture* colorTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	m_camera.SetColorTarget( 0, colorTarget );
	
	g_theRenderer->BeginCamera( m_camera );
	
	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindShader( (Shader*)nullptr );


	g_theRenderer->DrawTextAtPosition( "LOADING", Vec2( -3.f, -3.f ), 1.f );
	
	g_theRenderer->EndCamera( m_camera );

	g_theRenderer->CopyTexture( backbuffer, colorTarget );
	m_camera.SetColorTarget( nullptr );
	g_theRenderer->ReleaseRenderTarget( colorTarget );
}

void Game::RenderAttract()
{
	g_theRenderer->ClearScreen( Rgba8::BLACK );

	Texture* backbuffer = g_theRenderer->GetBackBuffer();
	Texture* colorTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	m_camera.SetColorTarget( 0, colorTarget );

	g_theRenderer->BeginCamera( m_camera );

	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindShader( (Shader*)nullptr );

	g_theRenderer->DrawTextAtPosition( "Bullet Hell Dungeon", Vec2( -4.75f, 1.f ), 0.5f );
	g_theRenderer->DrawTextAtPosition( "[SPACE] or [LMB] to Play", Vec2( -3.f, -2.f ), 0.25f );
	g_theRenderer->EndCamera( m_camera );

	g_theRenderer->CopyTexture( backbuffer, colorTarget );
	m_camera.SetColorTarget( nullptr );
	g_theRenderer->ReleaseRenderTarget( colorTarget );
}

void Game::RenderPlaying()
{
	g_theRenderer->ClearScreen( Rgba8::BLACK );

	Texture* backbuffer = g_theRenderer->GetBackBuffer();
	Texture* colorTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	m_camera.SetColorTarget( 0, colorTarget );
	m_UICamera.SetColorTarget( 0, colorTarget );

	g_theRenderer->BeginCamera( m_camera );
	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindShader( (Shader*)nullptr );
	RenderGame();
	RenderMouse();
	g_theRenderer->EndCamera( m_camera );

	g_theRenderer->BeginCamera( m_UICamera );
	RenderUI();
	g_theRenderer->EndCamera( m_UICamera );

	g_theRenderer->CopyTexture( backbuffer, colorTarget );
	m_camera.SetColorTarget( nullptr );
	g_theRenderer->ReleaseRenderTarget( colorTarget );
	GUARANTEE_OR_DIE( g_theRenderer->GetTotalRenderTargetPoolSize() < 8, "Created too many render targets" );

	DebugRenderBeginFrame();
	DebugRenderWorldToCamera( &m_camera );
	DebugRenderScreenTo( g_theRenderer->GetBackBuffer() );
	DebugRenderEndFrame();
}

void Game::AddDevConsoleTest()
{
	m_isDevConsoleTestActive = true;
	g_theConsole->PrintString( Rgba8::RED, "Hello World!" );
	g_theConsole->PrintString( Rgba8::GREEN, "Are you still there?" );
	g_theConsole->PrintString( Rgba8::BLUE, "Why?" );

	g_theConsole->PrintString( Rgba8::RED, "Hello World!" );
	g_theConsole->PrintString( Rgba8::GREEN, "Are you still there?" );
	g_theConsole->PrintString( Rgba8::BLUE, "Why?" );

	g_theConsole->PrintString( Rgba8::RED, "Hello World!" );
	g_theConsole->PrintString( Rgba8::GREEN, "Are you still there?" );
	g_theConsole->PrintString( Rgba8::BLUE, "Why?" );

	g_theConsole->PrintString( Rgba8::RED, "Hello World!" );
	g_theConsole->PrintString( Rgba8::GREEN, "Are you still there?" );
	g_theConsole->PrintString( Rgba8::BLUE, "Why?" );
}
