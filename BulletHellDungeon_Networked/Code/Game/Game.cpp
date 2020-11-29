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
#include "Game/AudioDefinition.hpp"
#include "Game/UDPGameConnection.hpp"
#include "Game/Client.hpp"
#include "Game/RemoteServer.hpp"


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

Game::~Game()
{
	delete m_world;
	delete m_mapDefDoc;
	delete m_tileDefDoc;
	delete m_actorDefDoc;
}

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

	m_deathTimer.SetSeconds( 2.5 );
	m_deathTimer.Stop();

	m_victoryTimer.SetSeconds( 2.5 );
	m_victoryTimer.Stop();
}

void Game::Shutdown()
{
	m_world->Shutdown();
}

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
	case PAUSED: UpdatePaused( deltaSeconds );
		break;
	case DEATH: UpdateDeath( deltaSeconds );
		break;
	case VICTORY: UpdateVictory( deltaSeconds );
		break;
	default: ERROR_AND_DIE( "Invalid Game State" );
		break;
	}

	if( m_player )
	{
		if( !m_player->IsAlive() )
		{
			if( m_gameState == PLAYING || m_gameState == PAUSED )
			{
				m_gameState = DEATH;
				m_deathTimer.Reset();
				//AudioDefinition::StopAllSounds();
			}
		}
	}
}

void Game::UpdateRemote( UDPGameConnection* udpConnection, float deltaSeconds )
{
	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses( 0 );
	}

	switch( m_gameState )
	{
	case LOADING: UpdateLoading( 0 );
		break;
	case ATTRACT: UpdateAttract( 0 );
		break;
	case PLAYING: UpdatePlayingNetworked( deltaSeconds, udpConnection );
		break;
	case PAUSED: UpdatePaused( 0 );
		break;
	case DEATH: //UpdateDeath( 0 );
		break;
	case VICTORY: //UpdateVictory( 0 );
		break;
	default: ERROR_AND_DIE( "Invalid Game State" );
		break;
	}
}

void Game::Render()
{

}


void Game::SetPlayerToFollow( int playerID )
{
	std::vector<Actor*> players;
	m_world->GetPlayers( players );
	if( playerID < players.size() )
	{
		m_player = players[playerID];
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

	Texture* tileSpriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/noLightTileSpriteSheet_4x4.png" );
	Texture* actorSpriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/bender_14x14_New.png" );
	Texture* portraitSpriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/KushnariovaPortraits_8x8.png" );
	Texture* weaponSpriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/weapons.png" );
	Texture* bulletsSpriteSheetTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Extras_4x4.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/reticle.png" );
	g_tileSpriteSheet = new SpriteSheet(*tileSpriteSheetTexture,IntVec2(4,4));
	g_actorSpriteSheet = new SpriteSheet(*actorSpriteSheetTexture, IntVec2(14,14));
	g_portraitSpriteSheet = new SpriteSheet(*portraitSpriteSheetTexture, IntVec2(8,8));
	g_weaponSpriteSheet = new SpriteSheet(*weaponSpriteSheetTexture, IntVec2(12, 12) );
	g_bulletsSpriteSheet = new SpriteSheet( *bulletsSpriteSheetTexture, IntVec2( 4, 4 ) );

	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PauseMenu.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PauseMenuPlay.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PauseMenuRestart.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PauseMenuQuit.png" );

	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/MainMenu.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/MainMenuPlay.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/MainMenuQuit.png" );

	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/HealthBar.png" );

 	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/VictoryMenuContinue.png" );
 	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/VictoryMenu.png" );

	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/DeadMenu.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/DeadMenuContinue.png" );
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
	Texture* healthBarTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/HealthBar.png" );
	float playerHealth = (float)m_player->GetHealth();
	float playerHealthFraction = playerHealth / (float)PLAYER_HEALTH;

	AABB2 gameCamera = AABB2( m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight() );
	AABB2 topBox = gameCamera.GetBoxAtTop( 0.1f );
	AABB2 topLeftBox = topBox.GetBoxAtLeft( 0.3f );
	AABB2 underHealthBox = topLeftBox.GetBoxAtRight( 0.81f );
	Vec2 underHealthBoxDims = underHealthBox.GetDimensions();
	underHealthBoxDims.y *= 0.2f;
	underHealthBox.SetDimensions( underHealthBoxDims );
	underHealthBox.maxs.x -= 3.f;
	AABB2 playerHealthBox = underHealthBox.GetBoxAtLeft( playerHealthFraction );
	AABB2 topRightBox = topBox.GetBoxAtRight( 0.15f );
	topRightBox.SetDimensions( Vec2( 1.16f, 0.7f ) * 10.f );
	AABB2 weaponBorderBox = topRightBox;
	weaponBorderBox.SetDimensions( weaponBorderBox.GetDimensions() * 2.f );

	float bossHealth = (float)GetBossHealth();
	float bossHealthFraction = bossHealth/1000.f;

	AABB2 bossHealthBox = gameCamera.GetBoxAtBottom( 0.2f );
	bossHealthBox.SetDimensions( Vec2( 120.f, 3.f ) );
	AABB2 bossCurrentHealthBox = bossHealthBox.GetBoxAtLeft( bossHealthFraction );

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawAABB2Filled( underHealthBox, Rgba8( 235, 86, 82 ) );
	g_theRenderer->DrawAABB2Filled( playerHealthBox, Rgba8( 51, 189, 75 ) );

	if( bossHealthFraction >= 0.f )
	{
		g_theRenderer->DrawAABB2Filled( bossHealthBox, Rgba8( 235, 86, 82 ) );
		g_theRenderer->DrawAABB2Filled( bossCurrentHealthBox, Rgba8( 51, 189, 75 ) );
	}

	g_theRenderer->BindTexture( healthBarTex );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->DrawAABB2Filled( topLeftBox, Rgba8::WHITE );
	SpriteDefinition const* weaponSprite = m_player->GetCurrentWeapon()->GetWeaponSpriteDef();
	Texture const& weaponTexture = weaponSprite->GetTexture();
	AABB2 weaponUVs;
	weaponSprite->GetUVs( weaponUVs.mins, weaponUVs.maxs );

	Texture* weaponBorderTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/WeaponBorder.png" );
	g_theRenderer->BindTexture( weaponBorderTex );
	g_theRenderer->DrawAABB2Filled( weaponBorderBox, Rgba8::WHITE );
	g_theRenderer->BindTexture( &weaponTexture );
	g_theRenderer->DrawAABB2Filled( topRightBox, Rgba8::WHITE, weaponUVs.mins, weaponUVs.maxs );
	RenderConsole();
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	UNUSED( deltaSeconds );

	XboxController const& controller = g_theInput->GetXboxController(0);
	
	KeyButtonState const& leftMouseButton = g_theInput->GetMouseButton( LeftMouseButton );

// 	if( m_gameState == ATTRACT )
// 	{
// 		if( leftMouseButton.WasJustReleased() )
// 		{
// 			if( m_isMouseOverMainMenuPlay )
// 			{
// 				m_gameState = PLAYING;
// 				g_theApp->UnPauseGame();
// 
// 				AudioDefinition::StopAllSounds();
// 				AudioDefinition* gamePlayAudio = AudioDefinition::GetAudioDefinition( "GamePlayMusic" );
// 				gamePlayAudio->PlaySound();
// 			}
// 			else if( m_isMouseOverMainMenuQuit )
// 			{
// 				g_theApp->HandleQuitRequested();
// 			}
// 
// 		}
// 	}
	if( m_gameState == PLAYING )
	{
		if( controller.GetButtonState( XBOX_BUTTON_ID_Y ).WasJustPressed() )
		{
			g_theConsole->SetIsOpen( !g_theConsole->IsOpen() );
		}

		const KeyButtonState& f5Key = g_theInput->GetKeyStates( 0x74 );
		const KeyButtonState& f6Key = g_theInput->GetKeyStates( F6_KEY );
		if( f5Key.WasJustPressed() )
		{
			AudioDefinition* gamePlaySound = AudioDefinition::GetAudioDefinition( "GamePlayMusic" );
			gamePlaySound->StopSound();
			RebuildWorld();
			
			gamePlaySound->PlayGameSound();
		}
		if( f6Key.WasJustPressed() )
		{
			m_world->MoveToNextMap();
		}
	}

	if( m_gameState == PAUSED )
	{
		if( leftMouseButton.WasJustReleased() )
		{
			if( m_pausedRestartButton.IsPointInside( m_mousePositionOnUICamera ) )
			{
				RebuildWorld();
				g_theApp->UnPauseGame();
			}
			else if( m_pausedResumeButton.IsPointInside( m_mousePositionOnUICamera ) )
			{
				g_theApp->UnPauseGame();
			}
			else if( m_pausedQuitButton.IsPointInside( m_mousePositionOnUICamera ) )
			{
				g_theApp->HandleQuitRequested();
			}
		}

	}

	if( m_gameState == DEATH )
	{
		if( m_deathTimer.HasElapsed() )
		{
			if( leftMouseButton.WasJustReleased() )
			{
				if( m_deadContinueButton.IsPointInside( m_mousePositionOnUICamera ) )
				{
					RebuildWorld();
					g_theApp->UnPauseGame();
					m_gameState = ATTRACT;

					AudioDefinition::StopAllSounds();
					AudioDefinition* attractScreenSound = AudioDefinition::GetAudioDefinition( "AttractMusic" );
					attractScreenSound->PlayGameSound();
				}
			}
		}
	}

	if( m_gameState == VICTORY )
	{
		if( m_victoryTimer.HasElapsed() )
		{
			if( leftMouseButton.WasJustReleased() )
			{
				if( m_victoryContinueButton.IsPointInside( m_mousePositionOnUICamera ) )
				{
					RebuildWorld();
					g_theApp->UnPauseGame();
					m_gameState = ATTRACT;

					AudioDefinition::StopAllSounds();
					AudioDefinition* attractScreenSound = AudioDefinition::GetAudioDefinition( "AttractMusic" );
					attractScreenSound->PlayGameSound();
				}
			}
		}
	}
	

	const KeyButtonState& minusKey = g_theInput->GetKeyStates( MINUS_KEY );
	const KeyButtonState& plusKey = g_theInput->GetKeyStates( PLUS_KEY );
	const KeyButtonState& shiftKey = g_theInput->GetKeyStates( SHIFT_KEY );
	const KeyButtonState& ctrlKey = g_theInput->GetKeyStates( CTRL_KEY );

	float currentMasterVolume = AudioDefinition::s_masterVolume;
	float currentBGVolume = AudioDefinition::s_backgroundMusicVolume;
	float currentSFXVolume = AudioDefinition::s_SFXVolume;

	if( shiftKey.IsPressed() )
	{
		if( minusKey.WasJustPressed() )
		{
			AudioDefinition::ChangeBackgroundMusicVolume( currentBGVolume - 0.05f );
		}
		else if( plusKey.WasJustPressed() )
		{
			AudioDefinition::ChangeBackgroundMusicVolume( currentBGVolume + 0.05f );
		}
	}
	else if( ctrlKey.IsPressed() )
	{
		if( minusKey.WasJustPressed() )
		{
			AudioDefinition::ChangeSFXVolumme( currentSFXVolume - 0.05f );
		}
		else if( plusKey.WasJustPressed() )
		{
			AudioDefinition::ChangeSFXVolumme( currentSFXVolume + 0.05f );
		}
	}
	else
	{
		if( minusKey.WasJustPressed() )
		{
			AudioDefinition::ChangeMasterVolume( currentMasterVolume - 0.05f );
		}
		else if( plusKey.WasJustPressed() )
		{
			AudioDefinition::ChangeMasterVolume( currentMasterVolume + 0.05f );
		}
	}

}

IntVec2 Game::GetCurrentMapBounds() const
{
	return m_world->GetCurrentMapBounds();
}



void Game::RebuildWorld()
{
	m_world->Shutdown();
	delete m_world;

	m_world = new World(this);
	m_world->Startup();
	m_player = m_world->GetPlayer();
}

int Game::GetBossHealth() const
{
	return m_world->GetBossHealth();
}

void Game::AddPlayer( int playerSlot )
{
	m_world->AddNewPlayer( playerSlot );
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

void Game::TriggerVictoryState()
{
	m_gameState = VICTORY;
	m_victoryTimer.Reset();
	AudioDefinition::StopAllSounds();
}

std::vector<Vertex_PCU> const& Game::GetTileVertsToRender()
{
	return m_world->m_currentMap->GetTileVertsToRender();
}

std::vector<Entity*> const& Game::GetEntitiesToRender()
{
	return m_world->m_currentMap->GetEntitiesToRender();
}

int Game::GetPlayerHealth() const
{
	if( m_player )
	{
		return m_player->GetHealth();
	}
	else
	{
		return 0;
	}

}

void Game::UpdateConsoleTest( float deltaSeconds )
{
	UNUSED( deltaSeconds );
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
	XmlDocument audioDefDoc;

	XmlElement const& audioDef = GetRootElement( audioDefDoc, "Data/Gameplay/AudioFiles.xml" );
	XmlElement const& tileDef = GetRootElement( *m_tileDefDoc, "Data/Gameplay/TileDefs.xml" );
	XmlElement const& mapDef = GetRootElement( *m_mapDefDoc, "Data/Gameplay/MapDefs.xml" );
	XmlElement const& actorDef = GetRootElement( *m_actorDefDoc, "Data/Gameplay/Actors.xml" );
	XmlElement const& bulletsDef = GetRootElement( bulletDefDoc, "Data/Gameplay/Bullets.xml");
	XmlElement const& weaponDef = GetRootElement( weaponDefDoc, "Data/Gameplay/Weapons.xml");

	AudioDefinition::InitializeAudioDefinitions( audioDef );
	TileDefinition::InitializeTileDefinitions( tileDef );
	MapDefinition::InitializeMapDefinitions( mapDef );
	ActorDefinition::InitializeActorDefinitions( actorDef );
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

	AABB2 orthoBoundsUI( m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight() );
	Vec2 mouseDrawPosOnUICamera = orthoBoundsUI.GetPointAtUV( mouseNormalizedPos );
	m_mousePositionOnUICamera = mouseDrawPosOnUICamera;
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
// 		LoadAssets();
// 		InitializeDefinitions();
		m_world->Startup();
		m_player = m_world->GetPlayer();

// 		AudioDefinition* attractScreenSound = AudioDefinition::GetAudioDefinition( "AttractMusic" );
// 		attractScreenSound->PlaySound();
	}
	else if( m_frameCounter == 0 )
	{
// 		SoundID loadingSound = g_theAudio->CreateOrGetSound( "Data/Audio/Anticipation.mp3" );
// 		g_theAudio->PlaySound( loadingSound );
	}

	if( m_frameCounter == 2 )
	{

	}

	m_frameCounter++;
}

void Game::UpdateAttract( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	m_world->Update( 0.f );

// 	UpdateDebugMouse();
// 
// 	AABB2 mainMenuBounds = AABB2( m_camera.GetOrthoBottomLeft(), m_camera.GetOrthoTopRight() );
// 	AABB2 playButtonBounds = mainMenuBounds;
// 	AABB2 quitButtonBounds;
// 	Vec2 buttonDimensions = mainMenuBounds.GetDimensions();
// 	buttonDimensions *= 0.1f;
// 	playButtonBounds.SetDimensions( buttonDimensions );
// 	playButtonBounds.Translate( Vec2( 0.f, -1.f ) );
// 	quitButtonBounds = playButtonBounds;
// 	quitButtonBounds.Translate( Vec2( 0.f, -1.f ) );
// 
// 
// 	m_isMouseOverMainMenuPlay = false;
// 	m_isMouseOverMainMenuQuit = false;
// 
// 	if( playButtonBounds.IsPointInside( m_mousePositionOnMainCamera ) )
// 	{
// 		m_isMouseOverMainMenuPlay = true;
// 	}
// 	else if( quitButtonBounds.IsPointInside( m_mousePositionOnMainCamera ) )
// 	{
// 		m_isMouseOverMainMenuQuit = true;
// 	}

}

void Game::UpdateDeath( float deltaSeconds )
{
 	UNUSED( deltaSeconds );
// 
// 	AABB2 uiBounds = AABB2( m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight() );
// 	Vec2 uiDims = uiBounds.GetDimensions();
// 	m_deadMenu = uiBounds;
// 	m_deadMenu.SetDimensions( uiDims * 0.8f );
// 	m_deadContinueButton = m_deadMenu.GetBoxAtBottom( 0.5f );
// 	Vec2 deadContinueButtonDims = m_deadContinueButton.GetDimensions();
// 	deadContinueButtonDims = deadContinueButtonDims * Vec2( 0.2f, 0.2f );
// 	m_deadContinueButton.SetDimensions( deadContinueButtonDims );
// 	m_deadContinueButton.Translate( Vec2( 0.f, 7.f ) );
// 
// 	UpdateDebugMouse();
// 
// 	m_isMouseOverDeadContinue = false;
// 	if( m_deathTimer.HasElapsed() )
// 	{
// 		if( m_deadContinueButton.IsPointInside( m_mousePositionOnUICamera ) )
// 		{
// 			m_isMouseOverDeadContinue = true;
// 		}
// 	}
}

void Game::UpdateVictory( float deltaSeconds )
{
 	UNUSED( deltaSeconds );
// 
// 	AABB2 uiBounds = AABB2( m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight() );
// 	Vec2 uiDims = uiBounds.GetDimensions();
// 	m_victoryMenu = uiBounds;
// 	m_victoryMenu.SetDimensions( uiDims * 0.8f );
// 	m_victoryContinueButton = m_victoryMenu.GetBoxAtBottom( 0.5f );
// 	Vec2 victoryContinueButtonDims = m_victoryContinueButton.GetDimensions();
// 	victoryContinueButtonDims = victoryContinueButtonDims * Vec2( 0.2f, 0.2f );
// 	m_victoryContinueButton.SetDimensions( victoryContinueButtonDims );
// 	m_victoryContinueButton.Translate( Vec2( 0.f, 7.f ) );
// 	
// 	UpdateDebugMouse();
// 
// 	m_isMouseOverVictoryContinue = false;
// 	if( m_victoryTimer.HasElapsed() )
// 	{
// 		if( m_victoryContinueButton.IsPointInside( m_mousePositionOnUICamera ) )
// 		{
// 			m_isMouseOverVictoryContinue = true;
// 		}
// 	}
}

void Game::UpdatePaused( float deltaSeconds )
{
 	UNUSED( deltaSeconds );
// 
// 	AABB2 uiBounds = AABB2( m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight() );
// 	Vec2 uiDims = uiBounds.GetDimensions();
// 	m_pausedMenu = uiBounds;
// 	m_pausedMenu.SetDimensions( Vec2( uiDims.x, uiDims.x ) * 0.5f );
// 	AABB2 carvingPauseMenu = m_pausedMenu;
// 	Vec2 buttonDimension = carvingPauseMenu.GetDimensions();
// 	buttonDimension.x *= 0.3f;
// 	carvingPauseMenu.SetDimensions( buttonDimension );
// 	carvingPauseMenu.CarveBoxOffTop( 0.43f );
// 	m_pausedResumeButton = carvingPauseMenu.CarveBoxOffTop( 0.f, 9.f );
// 	m_pausedRestartButton = carvingPauseMenu.CarveBoxOffTop( 0.f, 9.f );
// 	m_pausedQuitButton = carvingPauseMenu.CarveBoxOffTop( 0.f, 9.f );
// 
// 	UpdateDebugMouse();
// 
// 	m_pausedResumeButtonTint= Rgba8::WHITE;
// 	m_pausedRestartButtonTint = Rgba8::WHITE;
// 	m_pausedQuitButtonTint = Rgba8::WHITE;
// 	m_isMouseOverPausedResume = false;
// 	m_isMouseOverPausedRestart = false;
// 	m_isMouseOverPausedQuit = false;
// 
// 	if( m_pausedResumeButton.IsPointInside( m_mousePositionOnUICamera ) )
// 	{
// 		m_pausedResumeButtonTint= Rgba8::YELLOW;
// 		m_isMouseOverPausedResume = true;
// 	}
// 	else if( m_pausedRestartButton.IsPointInside( m_mousePositionOnUICamera ) )
// 	{
// 		m_pausedRestartButtonTint = Rgba8::YELLOW;
// 		m_isMouseOverPausedRestart = true;
// 	}
// 	else if( m_pausedQuitButton.IsPointInside( m_mousePositionOnUICamera ) )
// 	{
// 		m_pausedQuitButtonTint = Rgba8::YELLOW;
// 		m_isMouseOverPausedQuit = true;
// 	}
}

void Game::UpdatePlaying( float deltaSeconds )
{
	m_world->Update( deltaSeconds );
	UpdateCamera( deltaSeconds );
	UpdateDebugMouse();
}


void Game::UpdatePlayingNetworked( float deltaSeconds, UDPGameConnection* udpConnection )
{
	if( udpConnection )
	{
		RemoteServer* remoteServer = (RemoteServer*)g_theServer;

		AddressedUDPPacket packet = udpConnection->PopFirstReceivedPacket();
		bool isValid = packet.isValid;
		while( isValid )
		{
			UDPPacket const& udpPacket = packet.packet;
			int packetSequenceNo = udpPacket.header.m_sequenceNo;
			int id = udpPacket.header.m_id;

			if( id == ADDENTITY )
			{
				CreateEntityMessage createMessage = *(CreateEntityMessage*)udpPacket.message;
				m_world->CreateEntity( createMessage );
				remoteServer->ACKReceivedMessage( packetSequenceNo );
				if( createMessage.entityType == ID_PLAYER )
				{
					if( createMessage.entityID == g_theClient->m_playerID )
					{
						SetPlayerToFollow( createMessage.entityID );
					}
				}
			}
			else if( id == UPDATEENTITY )
			{
				//remoteServer->ACKReceivedMessage( packetSequenceNo );
				UpdateEntityMessage updateMessage = *(UpdateEntityMessage*)udpPacket.message;
				m_world->UpdateEntity( updateMessage );
			}
			else if( id == DELETEENTITY )
			{
				remoteServer->ACKReceivedMessage( packetSequenceNo );
				DeleteEntityMessage deleteMessage = *(DeleteEntityMessage*)udpPacket.message;

				m_world->DeleteEntity( deleteMessage );
			}
			else if( id == NEXTMAP )
			{
				remoteServer->ACKReceivedMessage( packetSequenceNo );
				m_world->MoveToNextMap();
			}
			else if( id == VERIFIEDPACKET )
			{
				remoteServer->ACKMessageServerSent( packetSequenceNo );
			}

			packet = udpConnection->PopFirstReceivedPacket();
			isValid = packet.isValid;
		}
	}

	UpdateCamera( deltaSeconds );
	UpdateDebugMouse();
}

void Game::RenderVictory()
{
	return;

// 	Rgba8 backgroundTint = Rgba8( 0, 0, 0, 0 );
// 	if( m_victoryTimer.HasElapsed() )
// 	{
// 		backgroundTint.a = 150;
// 	}
// 	else
// 	{
// 		float elapsedTime = (float)m_victoryTimer.GetElapsedSeconds();
// 		float totalTime = (float)m_victoryTimer.GetSecondsRemaining() + elapsedTime;
// 
// 		float tint = 150.f * (elapsedTime / totalTime);
// 		backgroundTint.a = (unsigned char)tint;
// 	}
// 
// 	g_theRenderer->ClearScreen( Rgba8::BLACK );
// 
// 	Texture* backbuffer = g_theRenderer->GetBackBuffer();
// 	Texture* colorTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
// 	m_camera.SetColorTarget( 0, colorTarget );
// 	m_UICamera.SetColorTarget( 0, colorTarget );
// 
// 	g_theRenderer->BeginCamera( m_camera );
// 	g_theRenderer->SetModelMatrix( Mat44() );
// 	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
// 	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
// 	g_theRenderer->BindTexture( nullptr );
// 	g_theRenderer->BindShader( (Shader*)nullptr );
// 	RenderGame();
// 	g_theRenderer->EndCamera( m_camera );
// 
// 	g_theRenderer->BeginCamera( m_UICamera );
// 	RenderUI();
// 
// 	AABB2 gameCamera = AABB2( m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight() );
// 	g_theRenderer->BindTexture( nullptr );
// 	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
// 	g_theRenderer->DrawAABB2Filled( gameCamera, backgroundTint );
// 
// 	Texture* victoryMenuTex = nullptr;
// 
// 	if( m_isMouseOverVictoryContinue )
// 	{
// 		victoryMenuTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/VictoryMenuContinue.png" );
// 	}
// 	else
// 	{
// 		victoryMenuTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/VictoryMenu.png" );
// 	}
// 
// 
// 	if( m_victoryTimer.HasElapsed() )
// 	{
// 		g_theRenderer->BindTexture( victoryMenuTex );
// 		g_theRenderer->DrawAABB2Filled( m_victoryMenu, Rgba8::WHITE );
// 	}
// 
// 	g_theRenderer->EndCamera( m_UICamera );
// 
// 	g_theRenderer->BeginCamera( m_camera );
// 	g_theRenderer->SetModelMatrix( Mat44() );
// 	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
// 	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
// 	g_theRenderer->BindTexture( nullptr );
// 	g_theRenderer->BindShader( (Shader*)nullptr );
// 	RenderMouse();
// 	g_theRenderer->EndCamera( m_camera );
// 
// 	g_theRenderer->CopyTexture( backbuffer, colorTarget );
// 	m_camera.SetColorTarget( nullptr );
// 	g_theRenderer->ReleaseRenderTarget( colorTarget );
// 	GUARANTEE_OR_DIE( g_theRenderer->GetTotalRenderTargetPoolSize() < 8, "Created too many render targets" );
// 
// 	DebugRenderBeginFrame();
// 	DebugRenderWorldToCamera( &m_camera );
// 	DebugRenderScreenTo( g_theRenderer->GetBackBuffer() );
// 	DebugRenderEndFrame();
}

void Game::RenderPaused()
{
	return;

// 	g_theRenderer->ClearScreen( Rgba8::BLACK );
// 
// 	Texture* backbuffer = g_theRenderer->GetBackBuffer();
// 	Texture* colorTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
// 	m_camera.SetColorTarget( 0, colorTarget );
// 	m_UICamera.SetColorTarget( 0, colorTarget );
// 
// 	g_theRenderer->BeginCamera( m_camera );
// 	g_theRenderer->SetModelMatrix( Mat44() );
// 	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
// 	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
// 	g_theRenderer->BindTexture( nullptr );
// 	g_theRenderer->BindShader( (Shader*)nullptr );
// 	RenderGame();
// 	g_theRenderer->EndCamera( m_camera );
// 
// 	g_theRenderer->BeginCamera( m_UICamera );
// 	RenderUI();
// 
// 	AABB2 gameCamera = AABB2( m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight() );
// 	Texture* pauseTexture = nullptr;
// 
// 	if( m_isMouseOverPausedQuit )
// 	{
// 		pauseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PauseMenuQuit.png" );
// 	}
// 	else if( m_isMouseOverPausedRestart )
// 	{
// 		pauseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PauseMenuRestart.png" );
// 	}
// 	else if( m_isMouseOverPausedResume )
// 	{
// 		pauseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PauseMenuPlay.png" );
// 	}
// 	else
// 	{
// 		pauseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PauseMenu.png" );
// 	}
// 
// 	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
// 	g_theRenderer->BindTexture( nullptr );
// 	g_theRenderer->DrawAABB2Filled( gameCamera, Rgba8( 0, 0, 0, 128 ) );
// 	g_theRenderer->BindTexture( pauseTexture );
// 	g_theRenderer->DrawAABB2Filled( m_pausedMenu, Rgba8::WHITE );
// // 	g_theRenderer->DrawAlignedTextAtPosition( "RESUME", m_pausedResumeButton, 5.f, Vec2( 0.5f, 0.7f ), m_pausedResumeButtonTint );
// // 	g_theRenderer->DrawAlignedTextAtPosition( "RESTART", m_pausedRestartButton, 5.f, Vec2( 0.5f, 0.5f ), m_pausedRestartButtonTint );
// // 	g_theRenderer->DrawAlignedTextAtPosition( "QUIT", m_pausedQuitButton, 5.f, Vec2( 0.5f, 0.3f ), m_pausedQuitButtonTint );
// 	g_theRenderer->EndCamera( m_UICamera );
// 
// 	g_theRenderer->BeginCamera( m_camera );
// 	g_theRenderer->SetModelMatrix( Mat44() );
// 	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
// 	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
// 	g_theRenderer->BindTexture( nullptr );
// 	g_theRenderer->BindShader( (Shader*)nullptr );
// 	RenderMouse();
// 	g_theRenderer->EndCamera( m_camera );
// 
// 	g_theRenderer->CopyTexture( backbuffer, colorTarget );
// 	m_camera.SetColorTarget( nullptr );
// 	g_theRenderer->ReleaseRenderTarget( colorTarget );
// 	GUARANTEE_OR_DIE( g_theRenderer->GetTotalRenderTargetPoolSize() < 8, "Created too many render targets" );
// 
// 	DebugRenderBeginFrame();
// 	DebugRenderWorldToCamera( &m_camera );
// 	DebugRenderScreenTo( g_theRenderer->GetBackBuffer() );
// 	DebugRenderEndFrame();
 }

void Game::RenderPlaying()
{
//	return;
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
