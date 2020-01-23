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

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;




Game::Game()
{
	m_camera = Camera();
	m_UICamera = Camera();

	m_world = new World(this);
}

Game::~Game(){}

void Game::Startup()
{

	m_world->Startup();
	m_player = m_world->GetPlayer();
	m_numTilesInViewVertically = GAME_CAMERA_Y;
	m_numTilesInViewHorizontally = GAME_CAMERA_Y * CLIENT_ASPECT;
	m_camera.SetOrthoView(Vec2(0.f, 0.f), Vec2(m_numTilesInViewHorizontally, m_numTilesInViewVertically));
	m_UICamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(m_numTilesInViewHorizontally, m_numTilesInViewVertically));
}

void Game::Shutdown(){}

void Game::RunFrame(){}

void Game::Update( float deltaSeconds )
{
	m_world->Update(deltaSeconds);

	//CheckCollisions();
	//UpdateEntities( deltaSeconds );
	UpdateCamera( deltaSeconds );

	CheckButtonPresses( deltaSeconds );


}

void Game::Render()
{

	//g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 1 ) );
	//g_theRenderer->BeginCamera( m_camera );
	//RenderGame();
	//g_theRenderer->EndCamera( m_camera );


	//g_theRenderer->BeginCamera( m_UICamera );
	//RenderUI();
	//g_theRenderer->EndCamera( m_UICamera );

}


void Game::CheckCollisions()
{}

void Game::UpdateEntities( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	IntVec2 currentMapBounds = GetCurrentMapBounds();

	if( g_theApp->GetDebugCameraMode() )
	{
		if( currentMapBounds.x > currentMapBounds.y * CLIENT_ASPECT )
		{
			m_camera.SetOrthoView( Vec2( 0.f, 0.f ), Vec2((float)currentMapBounds.x, (float)currentMapBounds.x/CLIENT_ASPECT ) );
		}
		else
		{
			m_camera.SetOrthoView( Vec2( 0.f, 0.f ), Vec2((float)currentMapBounds.y * CLIENT_ASPECT, (float)currentMapBounds.y ) );
		}
	}
	else
	{
		//Using the map order, clamp to an imaginary smaller border including half the game camera size
		m_cameraPosition = m_player->GetPosition();
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
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	const XboxController& controller = g_theInput->GetXboxController(0);
	UNUSED( deltaSeconds );
	UNUSED( controller );



}

IntVec2 Game::GetCurrentMapBounds() const
{
	return m_world->getCurrentMapBounds();
}
