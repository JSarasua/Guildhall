#include"Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//Constants for calculation ship position change
const float NOTIME = 0.f;
const char* APP_NAME = "Testgame2D";	// ...becomes ??? (Change this per project!)


App::App()
{
	g_theAudio = new AudioSystem();
	g_theInput = new InputSystem();
	g_theRenderer = new RenderContext();
	g_theGame =  new Game();
	g_theConsole = new DevConsole();
	g_theEventSystem = new EventSystem();
	//g_gameConfigBlackboard = new NamedStrings();

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
	DebugRenderSystemStartup( g_theRenderer );
	g_theGame->Startup();
	g_theConsole->Startup();

	g_theEventSystem->SubscribeToEvent("QUIT", CONSOLECOMMAND, QuitRequested);
}

void App::Shutdown()
{
	g_theGame->Shutdown();
	delete g_theGame;;
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
}

void App::Update(float deltaSeconds)
{
	CheckButtonPresses();
	CheckController();


	g_theGame->Update(deltaSeconds);
}


void App::Render()
{
	g_theGame->Render();

}
void App::EndFrame()
{
	g_theRenderer->EndFrame();
	g_theConsole->EndFrame();
	g_theInput->EndFrame();
	g_theWindow->EndFrame();
	g_theAudio->EndFrame();
}

void App::RestartGame()
{
	g_theGame->Shutdown();
	delete g_theGame;;
	g_theGame = new Game();
	g_theGame->Startup();
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

void App::TogglePause()
{
	m_isPaused = !m_isPaused;
	Clock* masterClock = Clock::GetMaster();

	if( m_isPaused )
	{
		masterClock->Pause();
	}
	else
	{
		masterClock->Resume();
	}
}

void App::PauseGame()
{
	m_isPaused = true;

	Clock* masterClock = Clock::GetMaster();
	masterClock->Pause();
}

void App::UnPauseGame()
{
	m_isPaused = false;

	Clock* masterClock = Clock::GetMaster();
	masterClock->Resume();
}

void App::CheckButtonPresses()
{
	const KeyButtonState& tildeKey = g_theInput->GetKeyStates( 0xC0 );	//tilde: ~
	if( tildeKey.WasJustPressed() )
	{
		g_theConsole->SetIsOpen( !g_theConsole->IsOpen() );
	}

	if( g_theInput->GetKeyStates( 0x1B ).IsPressed() ) //ESC
	{
		g_theEventSystem->FireEvent("QUIT", CONSOLECOMMAND, nullptr);
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