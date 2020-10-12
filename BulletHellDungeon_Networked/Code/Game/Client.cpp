#include "Game/Client.hpp"
#include "Game/Game.hpp"
#include "Game/Server.hpp"
#include "Game/Actor.hpp"
#include "Game/App.hpp"
#include "Game/AudioDefinition.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>


Client::Client()
{

}

Client::~Client()
{

}

void Client::Startup()
{
	g_theInput->HideSystemCursor();

	m_deathTimer.SetSeconds( 2.5 );
	m_deathTimer.Stop();

	m_victoryTimer.SetSeconds( 2.5 );
	m_victoryTimer.Stop();
}

void Client::Shutdown()
{

}

void Client::BeginFrame()
{
	if( m_frameCounter == 1 )
	{
		g_theGame->LoadAssets();
		g_theGame->InitializeDefinitions();
	}


	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses();
		UpdateGameState();
	}
}

void Client::EndFrame()
{
}

void Client::Update( float deltaSeconds )
{
	UpdateGameState();

	UpdateCamera();

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


	if( g_theGame->m_player )
	{
		if( !g_theGame->m_player->IsAlive() )
		{
// 			if( m_gameState == PLAYING || m_gameState == PAUSED )
// 			{
// 				//m_gameState = DEATH;
// 				m_deathTimer.Reset();
// 				AudioDefinition::StopAllSounds();
// 			}
			if( !m_deathTimer.IsRunning() )
			{
				m_deathTimer.Reset();
				AudioDefinition::StopAllSounds();
			}
		}
	}
}

void Client::UpdateCamera()
{
	m_camera = g_theGame->GetCameraPointer();
	m_UICamera = g_theGame->GetUICameraPointer();
}

void Client::Render()
{
	//g_theGame->Render();
	
	//BeginRender();

	//eGameState currentGamestate = g_theServer->GetCurrentGameState();
	UpdateGameState();
	switch( m_gameState )
	{
	case LOADING: RenderLoading();
		break;
	case ATTRACT: RenderAttract();
		break;
	case DEATH: RenderDeath();
		break;
	case VICTORY: RenderVictory();
		break;
	case PAUSED: RenderPaused();
		break;
	case PLAYING: RenderPlaying();
		break;
	default: ERROR_AND_DIE( "Invalid Game State" );
		break;
	}

	//EndRender();

}

void Client::UpdateDebugMouse()
{
	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedPos();
	AABB2 orthoBounds( m_camera->GetOrthoBottomLeft(), m_camera->GetOrthoTopRight() );
	//Should pass in camera
	//Use UV because our value is between 0 to 1
	Vec2 mouseDrawPosOnCamera = orthoBounds.GetPointAtUV( mouseNormalizedPos );

	m_mousePositionOnMainCamera = mouseDrawPosOnCamera;

	AABB2 orthoBoundsUI( m_UICamera->GetOrthoBottomLeft(), m_UICamera->GetOrthoTopRight() );
	Vec2 mouseDrawPosOnUICamera = orthoBoundsUI.GetPointAtUV( mouseNormalizedPos );
	m_mousePositionOnUICamera = mouseDrawPosOnUICamera;
}

void Client::UpdateGameState()
{
	m_gameState = g_theServer->GetCurrentGameState();
}

void Client::UpdateLoading( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if( m_frameCounter > 1 )
	{

	}
	else if( m_frameCounter == 1 )
	{
		AudioDefinition* attractScreenSound = AudioDefinition::GetAudioDefinition( "AttractMusic" );
		attractScreenSound->PlaySound();
	}
	else if( m_frameCounter == 0 )
	{
		SoundID loadingSound = g_theAudio->CreateOrGetSound( "Data/Audio/Anticipation.mp3" );
		g_theAudio->PlaySound( loadingSound );
	}

	if( m_frameCounter == 2 )
	{

	}

	m_frameCounter++;
}

void Client::UpdateAttract( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	UpdateDebugMouse();

	AABB2 mainMenuBounds = AABB2( m_camera->GetOrthoBottomLeft(), m_camera->GetOrthoTopRight() );
	AABB2 playButtonBounds = mainMenuBounds;
	AABB2 quitButtonBounds;
	Vec2 buttonDimensions = mainMenuBounds.GetDimensions();
	buttonDimensions *= 0.1f;
	playButtonBounds.SetDimensions( buttonDimensions );
	playButtonBounds.Translate( Vec2( 0.f, -1.f ) );
	quitButtonBounds = playButtonBounds;
	quitButtonBounds.Translate( Vec2( 0.f, -1.f ) );


	m_isMouseOverMainMenuPlay = false;
	m_isMouseOverMainMenuQuit = false;

	if( playButtonBounds.IsPointInside( m_mousePositionOnMainCamera ) )
	{
		m_isMouseOverMainMenuPlay = true;
	}
	else if( quitButtonBounds.IsPointInside( m_mousePositionOnMainCamera ) )
	{
		m_isMouseOverMainMenuQuit = true;
	}
}

void Client::UpdateDeath( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	AABB2 uiBounds = AABB2( m_UICamera->GetOrthoBottomLeft(), m_UICamera->GetOrthoTopRight() );
	Vec2 uiDims = uiBounds.GetDimensions();
	m_deadMenu = uiBounds;
	m_deadMenu.SetDimensions( uiDims * 0.8f );
	m_deadContinueButton = m_deadMenu.GetBoxAtBottom( 0.5f );
	Vec2 deadContinueButtonDims = m_deadContinueButton.GetDimensions();
	deadContinueButtonDims = deadContinueButtonDims * Vec2( 0.2f, 0.2f );
	m_deadContinueButton.SetDimensions( deadContinueButtonDims );
	m_deadContinueButton.Translate( Vec2( 0.f, 7.f ) );

	UpdateDebugMouse();

	m_isMouseOverDeadContinue = false;
	if( m_deathTimer.HasElapsed() )
	{
		if( m_deadContinueButton.IsPointInside( m_mousePositionOnUICamera ) )
		{
			m_isMouseOverDeadContinue = true;
		}
	}
}

void Client::UpdateVictory( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	AABB2 uiBounds = AABB2( m_UICamera->GetOrthoBottomLeft(), m_UICamera->GetOrthoTopRight() );
	Vec2 uiDims = uiBounds.GetDimensions();
	m_victoryMenu = uiBounds;
	m_victoryMenu.SetDimensions( uiDims * 0.8f );
	m_victoryContinueButton = m_victoryMenu.GetBoxAtBottom( 0.5f );
	Vec2 victoryContinueButtonDims = m_victoryContinueButton.GetDimensions();
	victoryContinueButtonDims = victoryContinueButtonDims * Vec2( 0.2f, 0.2f );
	m_victoryContinueButton.SetDimensions( victoryContinueButtonDims );
	m_victoryContinueButton.Translate( Vec2( 0.f, 7.f ) );

	UpdateDebugMouse();

	m_isMouseOverVictoryContinue = false;
	if( m_victoryTimer.HasElapsed() )
	{
		if( m_victoryContinueButton.IsPointInside( m_mousePositionOnUICamera ) )
		{
			m_isMouseOverVictoryContinue = true;
		}
	}
}

void Client::UpdatePaused( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	AABB2 uiBounds = AABB2( m_UICamera->GetOrthoBottomLeft(), m_UICamera->GetOrthoTopRight() );
	Vec2 uiDims = uiBounds.GetDimensions();
	m_pausedMenu = uiBounds;
	m_pausedMenu.SetDimensions( Vec2( uiDims.x, uiDims.x ) * 0.5f );
	AABB2 carvingPauseMenu = m_pausedMenu;
	Vec2 buttonDimension = carvingPauseMenu.GetDimensions();
	buttonDimension.x *= 0.3f;
	carvingPauseMenu.SetDimensions( buttonDimension );
	carvingPauseMenu.CarveBoxOffTop( 0.43f );
	m_pausedResumeButton = carvingPauseMenu.CarveBoxOffTop( 0.f, 9.f );
	m_pausedRestartButton = carvingPauseMenu.CarveBoxOffTop( 0.f, 9.f );
	m_pausedQuitButton = carvingPauseMenu.CarveBoxOffTop( 0.f, 9.f );

	UpdateDebugMouse();

	m_pausedResumeButtonTint= Rgba8::WHITE;
	m_pausedRestartButtonTint = Rgba8::WHITE;
	m_pausedQuitButtonTint = Rgba8::WHITE;
	m_isMouseOverPausedResume = false;
	m_isMouseOverPausedRestart = false;
	m_isMouseOverPausedQuit = false;

	if( m_pausedResumeButton.IsPointInside( m_mousePositionOnUICamera ) )
	{
		m_pausedResumeButtonTint= Rgba8::YELLOW;
		m_isMouseOverPausedResume = true;
	}
	else if( m_pausedRestartButton.IsPointInside( m_mousePositionOnUICamera ) )
	{
		m_pausedRestartButtonTint = Rgba8::YELLOW;
		m_isMouseOverPausedRestart = true;
	}
	else if( m_pausedQuitButton.IsPointInside( m_mousePositionOnUICamera ) )
	{
		m_pausedQuitButtonTint = Rgba8::YELLOW;
		m_isMouseOverPausedQuit = true;
	}
}

void Client::UpdatePlaying( float deltaSeconds )
{
// 	g_theGame->Update( deltaSeconds );
// 	g_theGame->UpdateCamera( deltaSeconds );
	UpdateDebugMouse();
}

void Client::RenderLoading()
{
	//g_theGame->RenderLoading();
	BeginRender();
	g_theRenderer->BeginCamera( *m_camera );

	Texture* loadingScreenTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/LoadingScreen.png" );
	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
	g_theRenderer->BindTexture( loadingScreenTex );
	g_theRenderer->BindShader( (Shader*)nullptr );
	AABB2 cameraBounds = AABB2( m_camera->GetOrthoBottomLeft(), m_camera->GetOrthoTopRight() );
	g_theRenderer->DrawAABB2Filled( cameraBounds, Rgba8::WHITE );

	g_theRenderer->EndCamera( *m_camera );
	EndRender();
}

void Client::RenderAttract()
{
//	g_theGame->RenderAttract();
	BeginRender();
	g_theRenderer->BeginCamera( *m_camera );
	Texture* mainMenuTex;

	if( m_isMouseOverMainMenuPlay )
	{
		mainMenuTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/MainMenuPlay.png" );
	}
	else if( m_isMouseOverMainMenuQuit )
	{
		mainMenuTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/MainMenuQuit.png" );
	}
	else
	{
		mainMenuTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/MainMenu.png" );
	}
	AABB2 cameraBounds = AABB2( m_camera->GetOrthoBottomLeft(), m_camera->GetOrthoTopRight() );


	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
	g_theRenderer->BindTexture( mainMenuTex );
	g_theRenderer->BindShader( (Shader*)nullptr );
	g_theRenderer->DrawAABB2Filled( cameraBounds, Rgba8::WHITE );

	RenderMouse();

	g_theRenderer->EndCamera( *m_camera );
	EndRender();
}

void Client::RenderDeath()
{
// 	g_theGame->RenderDeath();

	Rgba8 backgroundTint = Rgba8( 0, 0, 0, 0 );
	if( m_deathTimer.HasElapsed() )
	{
		backgroundTint.a = 150;
	}
	else
	{
		float elapsedTime = (float)m_deathTimer.GetElapsedSeconds();
		float totalTime = (float)m_deathTimer.GetSecondsRemaining() + elapsedTime;

		float tint = 150.f * (elapsedTime / totalTime);
		backgroundTint.a = (unsigned char)tint;
	}

	BeginRender();
	g_theRenderer->BeginCamera( *m_camera );

	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindShader( (Shader*)nullptr );
	g_theGame->RenderGame();

	g_theRenderer->EndCamera( *m_camera );

	g_theRenderer->BeginCamera( *m_UICamera );
	RenderUI();

	AABB2 gameCamera = AABB2( m_UICamera->GetOrthoBottomLeft(), m_UICamera->GetOrthoTopRight() );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->DrawAABB2Filled( gameCamera, backgroundTint );

	Texture* deathMenuTex = nullptr;

	if( m_isMouseOverDeadContinue )
	{
		deathMenuTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/DeadMenuContinue.png" );
	}
	else
	{
		deathMenuTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/DeadMenu.png" );
	}


	if( m_deathTimer.HasElapsed() )
	{
		g_theRenderer->BindTexture( deathMenuTex );
		g_theRenderer->DrawAABB2Filled( m_deadMenu, Rgba8::WHITE );
	}

	g_theRenderer->EndCamera( *m_UICamera );

	g_theRenderer->BeginCamera( *m_camera );
	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindShader( (Shader*)nullptr );
	RenderMouse();
	g_theRenderer->EndCamera( *m_camera );

	EndRender();
}

void Client::RenderVictory()
{
	g_theGame->RenderVictory();
}

void Client::RenderPaused()
{
	g_theGame->RenderPaused();
}

void Client::RenderPlaying()
{
	g_theGame->RenderPlaying();
}

void Client::BeginRender()
{
	g_theRenderer->ClearScreen( Rgba8::BLACK );

	 m_backBuffer = g_theRenderer->GetBackBuffer();
	m_colorTarget = g_theRenderer->AcquireRenderTargetMatching( m_backBuffer );
	m_camera->SetColorTarget( 0, m_colorTarget );

	//g_theRenderer->BeginCamera( *m_camera );
}

void Client::EndRender()
{
	//g_theRenderer->EndCamera( *m_camera );

	g_theRenderer->CopyTexture( m_backBuffer, m_colorTarget );
	m_camera->SetColorTarget( nullptr );
	g_theRenderer->ReleaseRenderTarget( m_colorTarget );
}

void Client::RenderMouse()
{
	AABB2 mouseAABB;
	mouseAABB.SetCenter( m_mousePositionOnMainCamera );
	mouseAABB.SetDimensions( Vec2( 0.5f, 0.5f ) );
	Texture* reticle = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/reticle.png" );
	g_theRenderer->BindTexture( reticle );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->DrawAABB2Filled( mouseAABB, Rgba8::WHITE );
}

void Client::RenderWorld()
{
	Texture const& tex = g_tileSpriteSheet->GetTexture();
	g_theRenderer->BindTexture( &tex );

	std::vector<Vertex_PCU> const& verts = g_theServer->GetTileVertsToRender();
	g_theRenderer->DrawVertexArray( verts );
}

void Client::RenderUI()
{
	Texture* healthBarTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/HealthBar.png" );
	float playerHealth = (float)g_theServer->GetPlayerHealth();
	float playerHealthFraction = playerHealth / (float)PLAYER_HEALTH;

	AABB2 gameCamera = AABB2( m_UICamera->GetOrthoBottomLeft(), m_UICamera->GetOrthoTopRight() );
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

	float bossHealth = (float)g_theServer->GetBossHealth();
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
	SpriteDefinition const* weaponSprite = g_theServer->GetPlayerWeaponSprite();
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

void Client::RenderConsole()
{
	g_theConsole->Render( *g_theRenderer, *m_UICamera, TESTTEXTLINEHEIGHT );

}

void Client::CheckButtonPresses()
{
	XboxController const& controller = g_theInput->GetXboxController( 0 );
	KeyButtonState const& leftMouseButton = g_theInput->GetMouseButton( LeftMouseButton );

	if( m_gameState == ATTRACT )
	{
		if( leftMouseButton.WasJustReleased() )
		{
			if( m_isMouseOverMainMenuPlay )
			{
				m_gameState = PLAYING;
				g_theApp->UnPauseGame();

				AudioDefinition::StopAllSounds();
				AudioDefinition* gamePlayAudio = AudioDefinition::GetAudioDefinition( "GamePlayMusic" );
				gamePlayAudio->PlaySound();
			}
			else if( m_isMouseOverMainMenuQuit )
			{
				g_theApp->HandleQuitRequested();
			}

		}
	}
	if( m_gameState == PLAYING )
	{
		if( controller.GetButtonState( XBOX_BUTTON_ID_Y ).WasJustPressed() )
		{
			g_theConsole->SetIsOpen( !g_theConsole->IsOpen() );
		}

		const KeyButtonState& f5Key = g_theInput->GetKeyStates( 0x74 );
		//const KeyButtonState& f6Key = g_theInput->GetKeyStates( F6_KEY );
		if( f5Key.WasJustPressed() )
		{
			AudioDefinition* gamePlaySound = AudioDefinition::GetAudioDefinition( "GamePlayMusic" );
			gamePlaySound->StopSound();
			g_theGame->RebuildWorld();

			gamePlaySound->PlaySound();
		}
// 		if( f6Key.WasJustPressed() )
// 		{
// 			m_world->MoveToNextMap();
// 		}
	}

	if( m_gameState == PAUSED )
	{
		if( leftMouseButton.WasJustReleased() )
		{
			if( m_pausedRestartButton.IsPointInside( m_mousePositionOnUICamera ) )
			{
				g_theGame->RebuildWorld();
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
					g_theGame->RebuildWorld();
					g_theApp->UnPauseGame();
					m_gameState = ATTRACT;

					AudioDefinition::StopAllSounds();
					AudioDefinition* attractScreenSound = AudioDefinition::GetAudioDefinition( "AttractMusic" );
					attractScreenSound->PlaySound();
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
					g_theGame->RebuildWorld();
					g_theApp->UnPauseGame();
					m_gameState = ATTRACT;

					AudioDefinition::StopAllSounds();
					AudioDefinition* attractScreenSound = AudioDefinition::GetAudioDefinition( "AttractMusic" );
					attractScreenSound->PlaySound();
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


	g_theServer->UpdateGameState( m_gameState );
}

