#include"Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Console/DevConsole.hpp"

//Constants for calculation ship position change
const float NOTIME = 0.f;

App::App()
{
	g_theInput = new InputSystem();
	g_theRenderer = new RenderContext();
	g_theGame =  new Game();
	g_theConsole = new DevConsole();
	//g_gameConfigBlackboard = new NamedStrings();

}

App::~App() {}

void App::Startup()
{
	g_theInput->Startup();
	g_theRenderer->StartUp();
	g_theGame->Startup();
	g_theConsole->Startup();
}

void App::Shutdown()
{
	g_theGame->Shutdown();
	delete g_theGame;;
	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theInput->Shutdown();
	delete g_theInput;
	g_theConsole->Shutdown();
	delete g_theConsole;
}


void App::RunFrame()
{
	m_previousTime = m_currentTime;
	m_currentTime = (float)GetCurrentTimeSeconds();
	m_deltaTime = Clampf( m_currentTime - m_previousTime, 0.f, 0.1f );


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


	BeginFrame(); //For all engine systems (Not the game)
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
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
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
	g_theInput->EndFrame();
	g_theConsole->EndFrame();
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

void App::CheckButtonPresses()
{
	if( g_theInput->GetKeyStates( 0x1B ).IsPressed() ) //ESC
	{
		HandleQuitRequested();
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
		m_isPaused = !m_isPaused;
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
