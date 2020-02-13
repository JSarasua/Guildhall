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

	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses( deltaSeconds );
	}

	const KeyButtonState& leftArrow = g_theInput->GetKeyStates( 0x25 );
	const KeyButtonState& upArrow = g_theInput->GetKeyStates( 0x26 );
	const KeyButtonState& rightArrow = g_theInput->GetKeyStates( 0x27 );
	const KeyButtonState& downArrow = g_theInput->GetKeyStates( 0x28 );

	Vec2 movement;

	if( leftArrow.WasJustPressed() )
	{
		movement.x -= 1.f;
	}
	if( rightArrow.WasJustPressed() )
	{
		movement.x += 1.f;
	}
	if( upArrow.WasJustPressed() )
	{
		movement.y += 1.f;
	}
	if( downArrow.WasJustPressed() )
	{
		movement.y -= 1.f;
	}

	m_camera.Translate(Vec3(movement, 0.f));

	//g_theConsole->Update(g_theInput);
}

void Game::Render()
{
	g_theRenderer->BeginCamera(m_camera);
	//g_theRenderer->Draw(3, 0);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);

	g_theRenderer->DrawAABB2(AABB2(Vec2(2.5f,0.25f), Vec2( 9.5f, 6.5f )), Rgba8::RED, 0.25f);
	Texture* tex = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
	g_theRenderer->BindTexture(tex);
	g_theRenderer->DrawAABB2Filled(AABB2(Vec2(3.25f,1.f), Vec2( 8.75f, 5.75f )), Rgba8(255,255,255));
	g_theRenderer->BindTexture( nullptr );
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
