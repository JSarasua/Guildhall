#pragma once
#include "Game/Game.hpp"
#include "App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
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
	m_camera.SetProjectionPerspective( 60.f, -0.1f, -100.f );
	//m_camera.SetOrthoView(Vec2(0.f, 0.f), Vec2(GAME_CAMERA_Y* CLIENT_ASPECT, GAME_CAMERA_Y));
	m_invertShader = g_theRenderer->GetOrCreateShader("Data/Shaders/InvertColor.hlsl");

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




// 	Vec2 movement;
// 
// 	if( leftArrow.WasJustPressed() )
// 	{
// 		movement.x -= 1.f;
// 	}
// 	if( rightArrow.WasJustPressed() )
// 	{
// 		movement.x += 1.f;
// 	}
// 	if( upArrow.WasJustPressed() )
// 	{
// 		movement.y += 1.f;
// 	}
// 	if( downArrow.WasJustPressed() )
// 	{
// 		movement.y -= 1.f;
// 	}
// 
// 	m_camera.Translate(Vec3(movement, 0.f));

	//g_theConsole->Update(g_theInput);
}

void Game::Render()
{
	g_theRenderer->BeginCamera(m_camera);
	//g_theRenderer->Draw(3, 0);
	g_theRenderer->SetBlendMode(BlendMode::ADDITIVE);

	g_theRenderer->DrawAABB2(AABB2(Vec2(2.5f,0.25f), Vec2( 9.5f, 6.5f )), Rgba8::GREEN, 0.25f);

	Texture* tex = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
	g_theRenderer->SetBlendMode(BlendMode::SOLID);
	g_theRenderer->BindTexture(tex);
	g_theRenderer->BindShader(m_invertShader);
	g_theRenderer->DrawAABB2Filled(AABB2(Vec2(3.25f,1.f), Vec2( 6.f, 3.75f )), Rgba8(255, 255, 255, 128 ));

	g_theRenderer->SetBlendMode( BlendMode::ALPHA );
	g_theRenderer->BindShader( (Shader*)nullptr );
	g_theRenderer->DrawAABB2Filled( AABB2( Vec2( 6.f, 1.f ), Vec2( 8.75f, 3.75f ) ), Rgba8( 255, 255, 255 ) );


	tex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankBase.png" );
	g_theRenderer->SetBlendMode( BlendMode::SOLID );
	g_theRenderer->BindTexture( tex );
	g_theRenderer->BindShader( m_invertShader );
	g_theRenderer->DrawAABB2Filled( AABB2( Vec2( -2.f, -2.f ), Vec2( 2.f, 2.f ) ), Rgba8( 255, 255, 255, 128 ), -10.f );

	std::vector<Vertex_PCU> verts;
	Vertex_PCU::AppendVertsCube(verts, Transform(), 0.f );
	g_theRenderer->DrawVertexArray(verts);

	RenderDevConsole();

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


	const KeyButtonState& wKey = g_theInput->GetKeyStates( 'W' );
	const KeyButtonState& aKey = g_theInput->GetKeyStates( 'A' );
	const KeyButtonState& sKey = g_theInput->GetKeyStates( 'S' );
	const KeyButtonState& dKey = g_theInput->GetKeyStates( 'D' );
	const KeyButtonState& cKey = g_theInput->GetKeyStates( 'C' );
	const KeyButtonState& spaceKey = g_theInput->GetKeyStates( SPACE_KEY );

	Vec3 translator;

	if( wKey.WasJustPressed() )
	{
		translator.z += 1.f;
	}
	if( sKey.WasJustPressed() )
	{
		translator.z -= 1.f;
	}
	if( aKey.WasJustPressed() )
	{
		translator.x -= 1.f;
	}
	if( dKey.WasJustPressed() )
	{
		translator.x += 1.f;
	}
	if( cKey.WasJustPressed() )
	{
		translator.y += 1.f;
	}
	if( spaceKey.WasJustPressed() )
	{
		translator.y -= 1.f;
	}

	m_camera.Translate( translator );



}

IntVec2 Game::GetCurrentMapBounds() const
{
	return m_world->getCurrentMapBounds();
}

void Game::RenderDevConsole()
{
	//g_theConsole->Render(*g_theRenderer, m_camera, 0.1f);
}
