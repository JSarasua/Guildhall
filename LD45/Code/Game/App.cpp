#include"Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

App* g_theApp = nullptr;
RenderContext* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;

//Constants for calculation ship position change
const float TIMEPERFRAME = 1.f/60.f;
const float SLOWTIME = 1.f/600.f;
const float NOTIME = 0.f;

App::App()
{
	g_theInput = new InputSystem();
	g_theRenderer = new RenderContext();
	m_game =  new Game();

}

App::~App() {}

void App::Startup()
{
	g_theInput->Startup();
	g_theRenderer->StartUp();
	m_game->Startup();
}

void App::Shutdown()
{
	m_game->Shutdown();
	delete m_game;
	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theInput->Shutdown();
	delete g_theInput;
}


void App::RunFrame()
{
	m_previousTime = m_currentTime;
	m_currentTime = (float)GetCurrentTimeSeconds();
	m_deltaTime = Clampf( m_currentTime - m_previousTime, 0.f, 0.1f );
	CheckButtonPresses();

	if( m_isPaused )
	{
		Update(NOTIME);
	} 
	else if( m_isSlowed )
	{
		Update(m_deltaTime * 0.1f);
	}
	else
	{
		Update(m_deltaTime);
	}


	BeginFrame(); //For all engine systems (Not the game)
	Render();
	EndFrame(); //For all engine systems (Not the game)

}



bool App::HandleKeyPressed( unsigned char keyCode )
{
	UNUSED(keyCode);
	return true;
}

bool App::HandleKeyReleased( unsigned char keyCode )
{
	UNUSED(keyCode);
	return true;
}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return true;
}



bool App::IsUpArrowPressed()
{
	return m_isUpArrowPressed;
}

bool App::IsLeftArrowPressed()
{
	return m_isLeftArrowPressed;
}

bool App::IsRightArrowPressed()
{
	return m_isRightArrowPressed;
}

bool App::IsSlowed()
{
	return m_isSlowed;
}

void App::BeginFrame()
{
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
}

void App::Update(float deltaSeconds)
{
	m_game->Update(deltaSeconds);
	if( g_theInput->GetKeyStates( 0x1B ).IsPressed() )
	{
		HandleQuitRequested();
	}

}


void App::Render()
{
	m_game->Render();

}
void App::EndFrame()
{
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();
}

void App::RestartGame()
{
	m_game->Shutdown();
	delete m_game;
	m_game = new Game();
	m_game->Startup();
}

void App::CheckButtonPresses()
{
	const KeyButtonState& f1Key = g_theInput->GetKeyStates(0x70); //F1 Key
	const KeyButtonState& f8Key = g_theInput->GetKeyStates(0x77); //F8 key

	if( f1Key.WasJustPressed() )
	{
		m_game->toggleDebugMode();
	}
	if( f8Key.WasJustPressed() )
	{
		RestartGame();
	}


}
