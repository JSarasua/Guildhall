#pragma once
#include "Game/Game.hpp"
#include "App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/World.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include <string>
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Time.hpp"
//#include "Engine/Renderer/RenderContext.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;




Game::Game()
{
	m_camera = Camera();
	m_UICamera = Camera();

	m_world = new World(this);
}

Game::~Game(){}

void Game::Startup()
{

	g_theRenderer->CreateOrGetBitmapFont("Fonts/SquirrelFixedFont.png");

	m_world->Startup();
	m_player = m_world->GetPlayer();
	m_numTilesInViewVertically = GAME_CAMERA_Y;
	m_numTilesInViewHorizontally = GAME_CAMERA_Y * CLIENT_ASPECT;
	m_camera.SetOrthoView(Vec2(0.f, 0.f), Vec2(m_numTilesInViewHorizontally, m_numTilesInViewVertically));
	m_UICamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(160.f, 90.f));
}

void Game::Shutdown(){}

void Game::RunFrame(){}

void Game::Update( float deltaSeconds )
{
	switch( m_currentGameState )
	{
	case LOADING: UpdateLoading( deltaSeconds );
		break;
	case ATTRACT: UpdateAttract( deltaSeconds );
		break;
	case DEATH: UpdateDeath( deltaSeconds );
		break;
	case VICTORY: UpdateVictory( deltaSeconds );
		break;
	case PAUSED: UpdatePaused( deltaSeconds );
		break;
	case PLAYING: UpdatePlaying( deltaSeconds );
		break;
	default: ERROR_AND_DIE("Invalid Game State");
		break;
	}

}

void Game::Render()
{

// 	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
// 	g_theRenderer->BeginCamera( m_camera );

	switch( m_currentGameState )
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
	default: ERROR_AND_DIE("Invalid Game State");
		break;
	}
	//RenderGame();
// 	g_theRenderer->EndCamera( m_camera );
// 
// 
// 	g_theRenderer->BeginCamera( m_UICamera );
// 	RenderUI();
// 	g_theRenderer->EndCamera( m_UICamera );

}


void Game::CheckCollisions()
{}

void Game::UpdateEntities( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Game::UpdateCameraPlaying( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	IntVec2 currentMapBounds = GetCurrentMapBounds();

	if( g_theApp->GetDebugCameraMode() )
	{
		if( currentMapBounds.x > currentMapBounds.y * CLIENT_ASPECT )
		{
			m_camera.SetOrthoView( Vec2( 0.f, 0.f ), Vec2( (float)currentMapBounds.x, (float)currentMapBounds.x/CLIENT_ASPECT ) );
		}
		else
		{
			m_camera.SetOrthoView( Vec2( 0.f, 0.f ), Vec2( (float)currentMapBounds.y * CLIENT_ASPECT, (float)currentMapBounds.y  ) );
		}
	}
	else
	{
		//Using the map order, clamp to an imaginary smaller border including half the game camera size
		m_cameraPosition = m_lastPlayerPosition;
		m_cameraPosition.x = Clampf( m_cameraPosition.x, m_numTilesInViewHorizontally/2.f, currentMapBounds.x - m_numTilesInViewHorizontally/2.f );
		m_cameraPosition.y = Clampf( m_cameraPosition.y, m_numTilesInViewVertically/2.f, currentMapBounds.y - m_numTilesInViewVertically/2.f );


		m_camera.SetOrthoView( Vec2( m_cameraPosition.x - m_numTilesInViewHorizontally/2.f, m_cameraPosition.y - m_numTilesInViewVertically/2.f ), Vec2( m_cameraPosition.x + m_numTilesInViewHorizontally/2.f, m_cameraPosition.y + m_numTilesInViewVertically/2.f ) );
	}

}

void Game::RenderGame()
{
	m_world->Render();
}

void Game::RenderUI()
{


	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	AABB2 lifeAABB2 = AABB2(Vec2(-3.f,-3.f),Vec2(3.f,3.f));
	lifeAABB2.Translate(Vec2( 0.f, 85.f));
	Texture* baseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankBase.png" );
	for( int livesIndex = 0; livesIndex < m_world->GetCurrentLives(); livesIndex++ )
	{
		lifeAABB2.Translate(Vec2(5.f,0.f));
		g_theRenderer->BindTexture( baseTexture );
		g_theRenderer->DrawAABB2Filled( lifeAABB2, Rgba8( 255, 255, 255, 255 ) );
	}

// 	if( m_world->IsGameCompleted() )
// 	{
// 		AABB2 fullScreenAABB2( Vec2( 0.f, 0.f ), Vec2( 200.f, 100.f ) );
// 		g_theRenderer->BindTexture( nullptr );
// 		g_theRenderer->DrawAABB2Filled( fullScreenAABB2, Rgba8( 0, 0, 0, 255 ) );
// 		g_theRenderer->DrawRing( Vec2( 100.f, 50.f ), 30.f, Rgba8( 0, 255, 0, 255 ), 10.f );
// 	}
// 	else if( m_world->GetCurrentLives() <= 0 && m_world->GetPlayer()->IsGarbage() )
// 	{
// 		AABB2 fullScreenAABB2( Vec2( 0.f, 0.f ), Vec2( 200.f, 100.f ) );
// 		g_theRenderer->BindTexture( nullptr );
// 		g_theRenderer->DrawAABB2Filled( fullScreenAABB2, Rgba8( 0, 0, 0, 255 ) );
// 		DrawLine( Vec2( 5.f, 5.f ), Vec2( 195.f, 95.f ), Rgba8( 255, 0, 0, 255 ), 10.f );
// 		DrawLine( Vec2( 195.f, 5.f ), Vec2( 5.f, 95.f ), Rgba8( 255, 0, 0, 255 ), 10.f );
// 	}



}



IntVec2 Game::GetCurrentMapBounds() const
{
	return m_world->getCurrentMapBounds();
}

void Game::UpdateLoading( float deltaSeconds )
{
	static int frameCounter = 0;

	m_world->Update( 0.f );
	CheckButtonPressesLoading( deltaSeconds );


	if( frameCounter > 1 )
	{
		m_currentGameState = ATTRACT;
	}
	else if( frameCounter == 1 )
	{
		LoadAssets();

		

	}
	else if( frameCounter == 0 )
	{
		SoundID loadingSound = g_theAudio->CreateOrGetSound( "Data/Audio/Anticipation.mp3" );
		g_theAudio->PlaySound(loadingSound);
	}

	frameCounter++;
}

void Game::UpdateAttract( float deltaSeconds )
{
	m_world->Update( 0.f );
	CheckButtonPressesAttract( deltaSeconds );
}

void Game::UpdateDeath( float deltaSeconds )
{
	m_world->Update( deltaSeconds * 0.5f );
	CheckButtonPressesDeath( deltaSeconds );
}

void Game::UpdateVictory( float deltaSeconds )
{
	m_world->Update( deltaSeconds * 0.5f );

	static float victoryTime = (float)GetCurrentTimeSeconds();
	float currentTime = (float)GetCurrentTimeSeconds();

	if( currentTime > victoryTime + 1.f )
	{
		CheckButtonPressesVictory( deltaSeconds );
	}


}

void Game::UpdatePaused( float deltaSeconds )
{
	m_world->Update( 0.f );
	CheckButtonPressesPaused( deltaSeconds );
}

void Game::UpdatePlaying( float deltaSeconds )
{
	m_world->Update( deltaSeconds );
	if( !m_world->GetPlayer()->IsGarbage() )
	{
		m_player = m_world->GetPlayer();
		m_lastPlayerPosition = m_player->GetPosition();
	}

	//CheckCollisions();
	//UpdateEntities( deltaSeconds );
	UpdateCameraPlaying( deltaSeconds );

	CheckButtonPressesPlaying( deltaSeconds );


	if( m_world->IsGameCompleted() )
	{
		m_currentGameState = VICTORY;
	}
	else if( m_world->GetCurrentLives() <= 0 && m_world->GetPlayer()->IsGarbage() )
	{
		m_currentGameState = DEATH;
	}
}

void Game::CheckButtonPressesLoading( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::CheckButtonPressesAttract( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	const XboxController& controller = g_theInput->GetXboxController( 0 );
	if( controller.GetButtonState( XBOX_BUTTON_ID_START ).WasJustPressed() )
	{
		m_currentGameState = PLAYING;
	}

	if( controller.GetButtonState( XBOX_BUTTON_ID_BACK ).WasJustPressed() )
	{
		g_theApp->HandleQuitRequested();
	}

	const KeyButtonState& ESCKey = g_theInput->GetKeyStates( 0x1B ); //ESC



	if( ESCKey.WasJustPressed() )
	{
		g_theApp->HandleQuitRequested();
	}
}

void Game::CheckButtonPressesDeath( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	const XboxController& controller = g_theInput->GetXboxController( 0 );
	if( controller.GetButtonState( XBOX_BUTTON_ID_START ).WasJustPressed() )
	{
		//Rebuild
		delete m_world;
		m_world = new World(this);
		m_world->Startup();
		m_currentGameState = PLAYING;
	}

	if( controller.GetButtonState( XBOX_BUTTON_ID_BACK ).WasJustPressed() )
	{
		m_currentGameState = ATTRACT;
	}


	const KeyButtonState& pKey = g_theInput->GetKeyStates( 'P' );
	const KeyButtonState& ESCKey = g_theInput->GetKeyStates( 0x1B ); //ESC

	if( pKey.WasJustPressed() )
	{
		delete m_world;
		m_world = new World( this );
		m_world->Startup();
		m_currentGameState = PLAYING;
	}

	if( ESCKey.WasJustPressed() )
	{
		delete m_world;
		m_world = new World( this );
		m_world->Startup();
		m_currentGameState = ATTRACT;
	}

}

void Game::CheckButtonPressesVictory( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	const XboxController& controller = g_theInput->GetXboxController( 0 );
	if( controller.GetButtonState( XBOX_BUTTON_ID_START ).WasJustPressed() )
	{
		//Rebuild
		delete m_world;
		m_world = new World( this );
		m_world->Startup();
		m_currentGameState = ATTRACT;
	}

	if( controller.GetButtonState( XBOX_BUTTON_ID_BACK ).WasJustPressed() )
	{
		delete m_world;
		m_world = new World( this );
		m_world->Startup();
		m_currentGameState = ATTRACT;
	}

	const KeyButtonState& pKey = g_theInput->GetKeyStates( 'P' );
	const KeyButtonState& ESCKey = g_theInput->GetKeyStates( 0x1B ); //ESC

	if( pKey.WasJustPressed() )
	{
		delete m_world;
		m_world = new World( this );
		m_world->Startup();
		m_currentGameState = ATTRACT;
	}

	if( ESCKey.WasJustPressed() )
	{
		delete m_world;
		m_world = new World( this );
		m_world->Startup();
		m_currentGameState = ATTRACT;
	}

}

void Game::CheckButtonPressesPaused( float deltaSeconds )
{
	UNUSED( deltaSeconds );


	const XboxController& controller = g_theInput->GetXboxController( 0 );
	const KeyButtonState& pKey = g_theInput->GetKeyStates( 'P' );
	const KeyButtonState& ESCKey = g_theInput->GetKeyStates( 0x1B ); //ESC

	if( controller.GetButtonState( XBOX_BUTTON_ID_BACK ).WasJustPressed() )
	{
		delete m_world;
		m_world = new World( this );
		m_world->Startup();
		m_currentGameState = ATTRACT;
	}

	if( controller.GetButtonState( XBOX_BUTTON_ID_START ).WasJustPressed() )
	{
		m_currentGameState = PLAYING;
	}


	if( pKey.WasJustPressed() )
	{
		m_currentGameState = PLAYING;
	}

	if( ESCKey.WasJustPressed() )
	{
		delete m_world;
		m_world = new World( this );
		m_world->Startup();
		m_currentGameState = ATTRACT;
	}



}

void Game::CheckButtonPressesPlaying( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	const XboxController& controller = g_theInput->GetXboxController( 0 );
	const KeyButtonState& ESCKey = g_theInput->GetKeyStates( 0x1B ); //ESC
	const KeyButtonState& pKey = g_theInput->GetKeyStates( 'P' );


	if( controller.GetButtonState( XBOX_BUTTON_ID_START ).WasJustPressed() )
	{
		m_currentGameState = PAUSED;
	}
	if( controller.GetButtonState( XBOX_BUTTON_ID_BACK ).WasJustPressed() )
	{
		m_currentGameState = PAUSED;
	}






	if( pKey.WasJustPressed() )
	{
		m_currentGameState = PAUSED;
	}
	if( ESCKey.WasJustPressed() )
	{
		m_currentGameState = PAUSED;
	}





}

void Game::RenderLoading()
{
	//RenderGame();
	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	g_theRenderer->BeginCamera( m_camera );



	g_theRenderer->DrawTextAtPosition("LOADING", Vec2(2.f,5.f), 1.f);



	g_theRenderer->EndCamera( m_camera );


}

void Game::RenderAttract()
{
	//RenderGame();
	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	g_theRenderer->BeginCamera( m_UICamera );

	g_theRenderer->DrawTextAtPosition("INCURSION", Vec2(44.f,60.f), 8.f);


	g_theRenderer->EndCamera( m_UICamera );


}

void Game::RenderDeath()
{
	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	g_theRenderer->BeginCamera( m_camera );

	RenderGame();

	g_theRenderer->EndCamera( m_camera );







	static float originalTime = (float)GetCurrentTimeSeconds();
	static float alphaTimer = (float)GetCurrentTimeSeconds();
	static float currentTime = (float)GetCurrentTimeSeconds();

	if( currentTime < originalTime + 2.f )
	{
		currentTime = (float)GetCurrentTimeSeconds();
	}

	static unsigned char fadeInAlpha = 0;

	if( currentTime > alphaTimer + 0.1f )
	{
		alphaTimer = currentTime;
		fadeInAlpha += 10;
	}

	g_theRenderer->BeginCamera( m_UICamera );

	AABB2 fullScreenAABB2( Vec2( 0.f, 0.f ), Vec2( 160.f, 90.f ) );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->SetBlendMode( BlendMode::ALPHA );
	g_theRenderer->DrawAABB2Filled( fullScreenAABB2, Rgba8( 0, 0, 0, fadeInAlpha ) );


	if( currentTime > originalTime + 2.f )
	{
		g_theRenderer->DrawTextAtPosition( "YOU DIED", Vec2( 40.f, 42.f ), 8.f, Rgba8::RED );
		g_theRenderer->DrawTextAtPosition( "Press [START] to Restart", Vec2( 22.f, 34.f ), 4.f, Rgba8::WHITE );
		g_theRenderer->DrawTextAtPosition( "Press [BACK] to Return to Menu", Vec2( 20.f, 26.f ), 4.f, Rgba8::WHITE );
	}



	g_theRenderer->EndCamera( m_UICamera );
}

void Game::RenderVictory()
{



	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	g_theRenderer->BeginCamera( m_camera );

	RenderGame();

	g_theRenderer->EndCamera( m_camera );




	g_theRenderer->BeginCamera( m_UICamera );

	static float originalTime = (float)GetCurrentTimeSeconds();
	static float alphaTimer = (float)GetCurrentTimeSeconds();
	static float currentTime = (float)GetCurrentTimeSeconds();
	
	if(currentTime < originalTime + 0.5f)
	{
		currentTime = (float)GetCurrentTimeSeconds();
	}

	static unsigned char fadeInAlpha = 0;

	if( currentTime > alphaTimer + 0.1f )
	{
		alphaTimer = currentTime;
		fadeInAlpha += 30;
	}



	AABB2 fullScreenAABB2( Vec2( 0.f, 0.f ), Vec2( 160.f, 90.f ) );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->DrawAABB2Filled( fullScreenAABB2, Rgba8( 0, 0, 0, fadeInAlpha ) );

	if( currentTime > originalTime + 0.5f )
	{
		g_theRenderer->DrawTextAtPosition( "O=======O", Vec2( 44.f, 50.f ), 8.f );
		g_theRenderer->DrawTextAtPosition( "|VICTORY|", Vec2( 44.f, 42.f ), 8.f );
		g_theRenderer->DrawTextAtPosition( "O=======O", Vec2( 44.f, 34.f ), 8.f );
	}


	g_theRenderer->EndCamera( m_UICamera );

}

void Game::RenderPaused()
{
	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	g_theRenderer->BeginCamera( m_camera );

	RenderGame();

	g_theRenderer->EndCamera( m_camera );




	g_theRenderer->BeginCamera( m_UICamera );

	AABB2 fullScreenAABB2( Vec2( 0.f, 0.f ), Vec2( 160.f, 90.f ) );
	RenderUI();
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawAABB2Filled( fullScreenAABB2, Rgba8( 0, 0, 0, 128 ) );
	g_theRenderer->DrawTextAtPosition("Game Paused", Vec2(36.f, 50.f), 8);
	g_theRenderer->DrawTextAtPosition("Press [START] to Resume", Vec2(34.f, 40.f), 4);
	g_theRenderer->DrawTextAtPosition("Press [BACK] to Quit", Vec2(40.f, 35.f), 4);
	g_theRenderer->EndCamera( m_UICamera );


}

void Game::RenderPlaying()
{
	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	g_theRenderer->BeginCamera( m_camera );

	RenderGame();

	g_theRenderer->EndCamera( m_camera );


	g_theRenderer->BeginCamera( m_UICamera );
	RenderUI();
	g_theRenderer->EndCamera( m_UICamera );
}

void Game::LoadAssets()
{

	//Audio
	g_theAudio->CreateOrGetSound( "Data/Audio/AttractMusic.mp3" );
	g_theAudio->CreateOrGetSound( "Data/Audio/EnemyDied.wav" );
	g_theAudio->CreateOrGetSound( "Data/Audio/EnemyHit.wav" );
	g_theAudio->CreateOrGetSound( "Data/Audio/EnemyShoot.wav" );
	g_theAudio->CreateOrGetSound( "Data/Audio/GameOver.mp3" );
	g_theAudio->CreateOrGetSound( "Data/Audio/GameplayMusic.mp3" );
	g_theAudio->CreateOrGetSound( "Data/Audio/MapExited.mp3" );
	g_theAudio->CreateOrGetSound( "Data/Audio/Pause.mp3" );
	g_theAudio->CreateOrGetSound( "Data/Audio/PlayerDied.wav" );
	g_theAudio->CreateOrGetSound( "Data/Audio/PlayerHit.wav" );
	g_theAudio->CreateOrGetSound( "Data/Audio/PlayerShootNormal.ogg" );
	g_theAudio->CreateOrGetSound( "Data/Audio/QuitGame.mp3" );
	g_theAudio->CreateOrGetSound( "Data/Audio/StartGame.mp3" );
	g_theAudio->CreateOrGetSound( "Data/Audio/Unpause.mp3" );
	g_theAudio->CreateOrGetSound( "Data/Audio/Victory.mp3" );


	//Textures
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankTop.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankBase.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTurretTop.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTurretBase.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTank4.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Extras_4x4.png" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Explosion_5x5.png" );
}
