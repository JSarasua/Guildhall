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
#include "Game/Map.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;




Game::Game()
{
	m_camera = Camera();
	m_UICamera = Camera();
}

Game::~Game(){}

void Game::Startup()
{
	m_camera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_SIZE_X,WORLD_SIZE_Y));
	m_UICamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_SIZE_X,WORLD_SIZE_Y));

	m_map = new Map(IntVec2(INTWORLD_SIZE_X, INTWORLD_SIZE_Y), INTWORLD_SIZE_PIECES, this);
}

void Game::Shutdown(){}

void Game::RunFrame(){}

void Game::Update( float deltaSeconds )
{
	//CheckCollisions();
	//UpdateEntities( deltaSeconds );
	UpdateCamera( deltaSeconds );

	CheckButtonPresses( deltaSeconds );

	m_map->Update(deltaSeconds);


}

void Game::Render()
{

	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	g_theRenderer->BeginCamera( m_camera );
	RenderGame();
	m_map->Render();
	g_theRenderer->EndCamera( m_camera );


	g_theRenderer->BeginCamera( m_UICamera );
	RenderUI();
	g_theRenderer->EndCamera( m_UICamera );

}

int Game::CollidingWithWall( Vec2 m_position, float radius, bool isPlayer )
{
	if( isPlayer )
	{
		if( WORLD_SIZE_X < m_position.x + radius )
		{
			return 1;
		}
		else if( WORLD_SIZE_Y < m_position.y + radius )
		{
			return 2;
		}
		else if( 0.f > m_position.x - radius )
		{
			return 3;
		}
		else if( 0.f > m_position.y - radius )
		{
			return 4;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if( WORLD_SIZE_X < m_position.x - radius )
		{
			return 1;
		}
		else if( WORLD_SIZE_Y < m_position.y - radius )
		{
			return 2;
		}
		else if( 0.f > m_position.x + radius )
		{
			return 3;
		}
		else if( 0.f > m_position.y + radius )
		{
			return 4;
		}
		else
		{
			return 0;
		}
	}

}

bool Game::isDebugMode()
{
	return m_debugMode;
}

void Game::toggleDebugMode()
{
	m_debugMode = !m_debugMode;
}

void Game::CheckCollisions()
{}

void Game::UpdateEntities( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Game::UpdateCamera( float deltaSeconds )
{
	m_maxCameraShake -= CAMERAABLATIONPERSECOND * deltaSeconds;
	m_maxCameraShake = Clampf(m_maxCameraShake, 0.f, 1.f);
	Vec2 currentShake = Vec2(m_rand.RollRandomFloatInRange(-m_maxCameraShake,m_maxCameraShake),m_rand.RollRandomFloatInRange(-m_maxCameraShake,m_maxCameraShake));

	m_camera.SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_camera.Translate2D(currentShake);
}

void Game::RenderGame()
{
	g_theRenderer->DrawLine(Vec2(25.f,25.f), Vec2(175.f,75.f), Rgba8(128,255,128), 1.f);

	g_theRenderer->DrawRing(Vec2(100.f,50.f), 5.f, Rgba8(255,0,0), 1.f);

	AABB2 square = AABB2(Vec2(10.f,10.f), Vec2(190.f,90.f));
	g_theRenderer->DrawAABB2(square, Rgba8(128,128,255), 1.f);
}

void Game::RenderUI()
{
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	const XboxController& controller = g_theInput->GetXboxController(0);
	UNUSED(deltaSeconds);
	UNUSED(controller);


	/*
	a.	(1) Pressing ‘ESC’ closes the app.
b.	(1) Pressing ‘P’ toggles pausing the game.
c.	(1) Holding ‘T’ slows game simulation time to 1/10 the normal rate.
d.	(1) Holding ‘Y’ speeds game simulation time up to 4x the normal rate.
e.	(1) Pressing F1 toggles the debug display of inner (cyan) and outer (magenta) radii for all Entities.
f.	(1) Pressing F3 toggles debug “noclip” cheat, allowing the PlayerTank to move through solid tiles.
g.	(1) Pressing F4 toggles the debug camera, which shows the entire current Map onscreen.
h.	(1) Pressing F8 does a “hard restart” of the game, deleting and re-new-ing the Game instance.

	
	*/
	/*
	const KeyButtonState& f8Key = g_theInput->GetKeyStates(0x77); //F8 key
	if( f8Key.WasJustPressed() )
	{
		m_debugMode = !m_debugMode;
	}
	*/
}