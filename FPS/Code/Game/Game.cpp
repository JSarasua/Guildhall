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

	//m_world = new World(this);
}

Game::~Game(){}

void Game::Startup()
{
	m_camera.SetOrthoView(Vec2(0.f, 0.f), Vec2(GAME_CAMERA_Y* CLIENT_ASPECT, GAME_CAMERA_Y));
// 	m_world->Startup();
// 	m_player = m_world->GetPlayer();
// 	m_numTilesInViewVertically = GAME_CAMERA_Y;
// 	m_numTilesInViewHorizontally = GAME_CAMERA_Y * CLIENT_ASPECT;
// 	m_camera.SetOrthoView(Vec2(0.f, 0.f), Vec2(m_numTilesInViewHorizontally, m_numTilesInViewVertically));
// 	m_UICamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(m_numTilesInViewHorizontally, m_numTilesInViewVertically));
}

void Game::Shutdown(){}

void Game::RunFrame(){}

void Game::Update( float deltaSeconds )
{
	m_currentTime += deltaSeconds;
	if( m_currentTime > 255.f )
	{
		m_currentTime = 0.f;
	}
	Rgba8 color;
	color.g = 0;
	color.b = 0;
	float colorVal = m_currentTime * 45.f;
	color.r = (unsigned char)colorVal;
	color.b = (unsigned char)colorVal;

	m_camera.SetClearMode( CLEAR_COLOR_BIT, color, 0.f, 0 );
}

void Game::Render()
{
	g_theRenderer->BeginCamera(m_camera);
	g_theRenderer->EndCamera(m_camera);
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
}

void Game::RenderGame()
{
	//m_world->Render();
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
