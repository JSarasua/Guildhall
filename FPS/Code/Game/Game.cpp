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


	//m_world = new World(this);
}

Game::~Game(){}

void Game::Startup()
{
	m_camera = Camera();
	m_camera.SetColorTarget(nullptr); // we use this
	m_camera.SetOrthoView(Vec2(0.f, 0.f), Vec2(GAME_CAMERA_Y* CLIENT_ASPECT, GAME_CAMERA_Y));
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
	Rgba8 clearColor;
	clearColor.g = 0;
	clearColor.b = 0;
	float colorVal = m_currentTime * 45.f;
	clearColor.r = (unsigned char)colorVal;
	clearColor.b = (unsigned char)colorVal;

	m_camera.SetClearMode( CLEAR_COLOR_BIT, clearColor, 0.f, 0 );
}

void Game::Render()
{
	g_theRenderer->BeginCamera(m_camera);
	//g_theRenderer->Draw(3, 0);

	g_theRenderer->DrawAABB2(AABB2(Vec2(0.25f,2.5f), Vec2( 6.5f, 9.5f )), Rgba8::RED, 0.25f);
	g_theRenderer->DrawAABB2Filled(AABB2(Vec2(1.f,3.25f), Vec2( 5.75f, 8.75f )), Rgba8(255,255,0));
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
