#include "Game/Client.hpp"
#include "Game/Game.hpp"
#include "Game/Server.hpp"
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
	case PAUSED: //UpdatePaused( deltaSeconds );
		break;
	case DEATH: //UpdateDeath( deltaSeconds );
		break;
	case VICTORY: //UpdateVictory( deltaSeconds );
		break;
	default: ERROR_AND_DIE( "Invalid Game State" );
		break;
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

}

void Client::UpdateVictory( float deltaSeconds )
{

}

void Client::UpdatePaused( float deltaSeconds )
{

}

void Client::UpdatePlaying( float deltaSeconds )
{
	g_theGame->Update( deltaSeconds );
	g_theGame->UpdateCamera( deltaSeconds );
	UpdateDebugMouse();
}

void Client::RenderLoading()
{
	//g_theGame->RenderLoading();
	BeginRender();
	Texture* loadingScreenTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/LoadingScreen.png" );

	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
	g_theRenderer->BindTexture( loadingScreenTex );
	g_theRenderer->BindShader( (Shader*)nullptr );
	AABB2 cameraBounds = AABB2( m_camera->GetOrthoBottomLeft(), m_camera->GetOrthoTopRight() );
	g_theRenderer->DrawAABB2Filled( cameraBounds, Rgba8::WHITE );
	EndRender();
}

void Client::RenderAttract()
{
//	g_theGame->RenderAttract();
	BeginRender();

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

	EndRender();
}

void Client::RenderDeath()
{
	g_theGame->RenderDeath();
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

	g_theRenderer->BeginCamera( *m_camera );
}

void Client::EndRender()
{
	g_theRenderer->EndCamera( *m_camera );

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

