#include"Game/App.hpp"
#include "Game/Server.hpp"
#include "Game/Client.hpp"
#include "Game/AuthServer.hpp"
#include "Game/PlayerClient.hpp"
#include "Game/RemoteServer.hpp"
#include "Game/SinglePlayerGame.hpp"
#include "Game/MultiPlayerGame.hpp"
#include "Game/Game.hpp"
#include "Game/TCPGameConnection.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/AudioDefinition.hpp"


//Constants for calculation ship position change
const float NOTIME = 0.f;
const char* APP_NAME = "Bullet Hell Dungeon";	// ...becomes ??? (Change this per project!)


App::App()
{
	g_theAudio = new AudioSystem();
	g_theInput = new InputSystem();
	g_theRenderer = new RenderContext();
	Game* game = new SinglePlayerGame();
	g_theServer = new AuthServer( game );
	g_theClient = new PlayerClient();
	//g_theGame =  new Game();
	g_theConsole = new DevConsole();
	g_theEventSystem = new EventSystem();
	//g_gameConfigBlackboard = new NamedStrings();

	m_TCPConnection = new TCPGameConnection();

}

App::~App() {}

void App::Startup()
{
	Clock::SystemStartup();
	float aspectRatio = g_gameConfigBlackboard->GetValue( "windowAspect", 0.f );
	g_theWindow = new Window();
	g_theWindow->Open( APP_NAME, aspectRatio, 0.90f );
	g_theWindow->SetInputSystem( g_theInput );
	g_theWindow->SetEventSystem( g_theEventSystem );

	g_theInput->Startup( g_theWindow );
	g_theRenderer->StartUp( g_theWindow );
	g_theRenderer->Setup( nullptr );
	DebugRenderSystemStartup( g_theRenderer );
	g_theServer->Startup();
	g_theClient->Startup();
	//g_theGame->Startup();
	g_theConsole->Startup();

	m_TCPConnection->Startup();

	g_theEventSystem->SubscribeToEvent("quit", CONSOLECOMMAND, QuitRequested);
	g_theEventSystem->SubscribeMethodToEvent( "StartMultiplayerServer", CONSOLECOMMAND, this, &App::StartMultiplayerServer );
	g_theEventSystem->SubscribeMethodToEvent( "ConnectToMultiplayerServer", CONSOLECOMMAND, this, &App::ConnectToMultiplayerServer );
}

void App::Shutdown()
{
	m_TCPConnection->Shutdown();
	delete m_TCPConnection;

	g_theServer->Shutdown();
	delete g_theServer;
	g_theClient->Shutdown();
	delete g_theClient;
// 	g_theGame->Shutdown();
// 	delete g_theGame;
	DebugRenderSystemShutdown();
	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theConsole->Shutdown();
	delete g_theConsole;
	delete g_theAudio;
	delete g_theEventSystem;
	Clock::SystemShutdown();
	
	g_theInput->Shutdown();
	delete g_theInput;
}


void App::RunFrame()
{
	m_previousTime = m_currentTime;
	m_currentTime = (float)GetCurrentTimeSeconds();
	m_deltaTime = Clampf( m_currentTime - m_previousTime, 0.f, 0.1f );


	BeginFrame(); //For all engine systems (Not the game)
	if( m_isPaused )
	{
		Update(NOTIME);
	} 
	else if( m_isSlowed )
	{
		Update( m_deltaTime * 0.1f );
	}
	else if( m_isSpedUp )
	{
		Update( m_deltaTime * 4.f );
	}
	else
	{
		Update(m_deltaTime);
	}


	Render();
	EndFrame(); //For all engine systems (Not the game)

}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return true;
}





bool App::IsSlowed()
{
	return m_isSlowed;
}

bool App::IsNoClipping()
{
	return m_noClip;
}

void App::BeginFrame()
{
	Clock::BeginFrame();
	g_theAudio->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theInput->BeginFrame();
	g_theConsole->BeginFrame();

	m_TCPConnection->BeginFrame();

	g_theServer->BeginFrame();
	g_theClient->BeginFrame();
}

void App::Update(float deltaSeconds)
{
	CheckButtonPresses();
	CheckController();


	g_theServer->Update( deltaSeconds );
	g_theClient->Update( deltaSeconds );
	//g_theGame->Update(deltaSeconds);
}


void App::Render()
{
	g_theClient->Render();
	//g_theGame->Render();
}
void App::EndFrame()
{
	g_theRenderer->EndFrame();
	g_theConsole->EndFrame();
	g_theInput->EndFrame();
	g_theWindow->EndFrame();
	g_theAudio->EndFrame();
	g_theServer->EndFrame();
	g_theClient->EndFrame();
}

void App::RestartGame()
{
	g_theServer->RestartGame();
}

bool App::GetDebugGameMode()
{
	return m_debugGameMode;
}

bool App::GetDebugCameraMode()
{
	return m_debugCameraMode;
}

bool App::QuitRequested( const EventArgs* args )
{
	UNUSED(args);
	g_theApp->HandleQuitRequested();
	return true;
}

bool App::StartMultiplayerServer( EventArgs const& args )
{
	m_TCPConnection->StartTCPServer( args );

	int portNumber = args.GetValue( "port", 48000 );
	UNUSED( portNumber );

	//RestartGame();
	AudioDefinition::StopAllSounds();
	g_theConsole->SetIsOpen( false );
	g_theServer->Shutdown();
	g_theClient->Shutdown();
	
	g_theEventSystem->UnsubscribeObject( g_theServer );
	g_theEventSystem->UnsubscribeObject( g_theClient );
	
	delete g_theServer;
	delete g_theClient;

	Game* game = new SinglePlayerGame();
	g_theServer = new AuthServer( game );
	g_theClient = new PlayerClient();

	g_theServer->Startup();
	g_theClient->Startup();

	g_theServer->m_TCPGameConnection = m_TCPConnection;
	((AuthServer*)g_theServer)->m_port = portNumber;

	return true;
}

bool App::ConnectToMultiplayerServer( EventArgs const& args )
{
	m_TCPConnection->TCPClientConnect( args );

	std::string ip = args.GetValue( "ip", "127.0.0.1" );
	int portNumber = args.GetValue( "port", 48000 );
	UNUSED( ip );
	UNUSED( portNumber );

	AudioDefinition::StopAllSounds();
	g_theConsole->SetIsOpen( false );
	g_theServer->Shutdown();
	g_theClient->Shutdown();

	g_theEventSystem->UnsubscribeObject( g_theServer );
	g_theEventSystem->UnsubscribeObject( g_theClient );

	delete g_theServer;
	delete g_theClient;

	Game* game = new MultiPlayerGame();
	g_theServer = new RemoteServer( game );
	g_theClient = new PlayerClient();

	g_theServer->m_TCPGameConnection = m_TCPConnection;
	g_theServer->Startup();
	g_theClient->Startup();


	return true;
}

void App::TogglePause()
{
	m_isPaused = !m_isPaused;
	if( m_isPaused )
	{
		PauseGame();
	}
	else
	{
		UnPauseGame();
	}
}

void App::PauseGame()
{
	g_theServer->PauseGame();
	m_isPaused = true;

}

void App::UnPauseGame()
{
	m_isPaused = false;
	g_theServer->UnpauseGame();
}

void App::CheckButtonPresses()
{
	const KeyButtonState& tildeKey = g_theInput->GetKeyStates( 0xC0 );	//tilde: ~
	if( tildeKey.WasJustPressed() )
	{
		g_theConsole->SetIsOpen( !g_theConsole->IsOpen() );
	}

	if( g_theInput->GetKeyStates( 0x1B ).WasJustPressed() ) //ESC
	{
		TogglePause();
		//g_theEventSystem->FireEvent("quit", CONSOLECOMMAND, nullptr);
		//HandleQuitRequested();
	}

	if( g_theInput->GetKeyStates( 0x70 ).WasJustPressed() ) //F1
	{
		m_debugGameMode = !m_debugGameMode;
	}

	if( g_theInput->GetKeyStates( 0x72 ).WasJustPressed() ) //F3
	{
		m_noClip = !m_noClip;
	}

	if( g_theInput->GetKeyStates( 0x73 ).WasJustPressed() ) //F4
	{
		m_debugCameraMode = !m_debugCameraMode;
	}

	if( g_theInput->GetKeyStates( 0x77 ).WasJustPressed() ) //F8
	{
		g_theApp->RestartGame();
	}

	if( g_theInput->GetKeyStates( 'P' ).WasJustPressed() )
	{
		TogglePause();
	}

	if( g_theInput->GetKeyStates( 'T' ).IsPressed() )
	{
		m_isSlowed = true;
	}
	else if( g_theInput->GetKeyStates( 'T' ).WasJustReleased() )
	{
		m_isSlowed = false;
	}

	if( g_theInput->GetKeyStates( 'Y' ).IsPressed() )
	{
		m_isSpedUp = true;
	}
	else if( g_theInput->GetKeyStates( 'Y' ).WasJustReleased() )
	{
		m_isSpedUp = false;
	}
}

void App::CheckController()
{
	const XboxController& controller = g_theInput->GetXboxController( 0 );
	if( controller.GetButtonState( XBOX_BUTTON_ID_START ).WasJustPressed() )
	{
		g_theApp->RestartGame();
	}

	if( controller.GetButtonState( XBOX_BUTTON_ID_BACK ).WasJustPressed() )
	{
		m_debugGameMode = !m_debugGameMode;
	}
}
