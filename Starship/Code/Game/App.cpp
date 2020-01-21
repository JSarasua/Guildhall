#include"Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"

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
	if( keyCode == 0x1B ) //ESC pressed
	{
		HandleQuitRequested();
	}
	else if( keyCode == 'T' )
	{
		m_isSlowed = true;
	}
	else if( keyCode == 0x26 )
	{
		m_isUpArrowPressed = true;
	}
	else if( keyCode == 0x25 )
	{
		m_isLeftActuallyPressed = true;
		if( !m_isRightActuallyPressed )
		{
			m_isLeftArrowPressed = true;
		}
		else
		{
			m_isRightArrowPressed = false;
		}

	}
	else if( keyCode == 0x27 )
	{
		m_isRightActuallyPressed = true;
		if( !m_isLeftActuallyPressed )
		{
			m_isRightArrowPressed = true;
		}
		else
		{
			m_isLeftArrowPressed = false;
		}

	}
	else if( keyCode == 'O' )
	{
		if( !m_isOstillHeld )
		{
			m_game->CreateAsteroid();

		}
		m_isOstillHeld = true;
	}
	else if( keyCode == 0x20 )
	{
		if( m_game->GetCurrentLevel() == 0 )
		{
			m_game->IncrementLevel();
		}
		else if( !m_isSpaceStillHeld )
		{
			m_game->ShootBullet();
		}
		m_isSpaceStillHeld = true;
		
	}
	
	return true;
}

bool App::HandleKeyReleased( unsigned char keyCode )
{
	if( keyCode == 'P' )
	{
		m_isPaused = !m_isPaused;
	}
	else if( keyCode == 'T' )
	{
		m_isSlowed = false;
	}
	else if( keyCode == 0x26 )
	{
		m_isUpArrowPressed = false;
	}
	else if( keyCode == 0x25 )
	{
		m_isLeftActuallyPressed = false;
		m_isLeftArrowPressed = false;
	}
	else if( keyCode == 0x27 )
	{
		m_isRightActuallyPressed = false;
		m_isRightArrowPressed = false;
	}
	else if( keyCode == 0x20 )
	{
		m_isSpaceStillHeld = false;
	}
	else if( keyCode == 'O' )
	{
		m_isOstillHeld = false;
	}
	else if( keyCode == 'N' )
	{
		if( m_game->GetCurrentLevel() == 0 )
		{
			m_game->IncrementLevel();
		}
		else
		{
			m_game->ResurrectPlayer();
		}

	}
	else if( keyCode == 0x70 )
	{
		m_game->FlipDebugMode();
	}
	else if( keyCode == 0x77 )
	{
		RestartGame();
	}
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

}


void App::Render()
{
	m_game->Render();

}
void App::EndFrame()
{
	g_theRenderer->EndFrame();
}

void App::RestartGame()
{
	m_game->Shutdown();
	delete m_game;
	m_game = new Game();
	m_game->Startup();
}
