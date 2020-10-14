#include "Game/AuthServer.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Time/Clock.hpp"
#include "Game/Actor.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Game/WeaponDefinition.hpp"

AuthServer::AuthServer() : Server()
{
	//g_theGame = new Game();
}

AuthServer::~AuthServer()
{
	//delete g_theGame;
}

void AuthServer::Startup()
{
	g_theGame->Startup();
}

void AuthServer::Shutdown()
{
	g_theGame->Shutdown();
}

void AuthServer::BeginFrame()
{

}

void AuthServer::EndFrame()
{

}

void AuthServer::RestartGame()
{
	g_theGame->Shutdown();
	delete g_theGame;
	g_theGame = new Game();
	g_theGame->Startup();
}

void AuthServer::PauseGame()
{
	if( g_theGame->m_gameState == PLAYING )
	{
		g_theGame->m_gameState = PAUSED;
	}

	Clock* masterClock = Clock::GetMaster();
	masterClock->Pause();
}

void AuthServer::UnpauseGame()
{
	if( g_theGame->m_gameState == PAUSED )
	{
		g_theGame->m_gameState = PLAYING;
	}

	Clock* masterClock = Clock::GetMaster();
	masterClock->Resume();
}

void AuthServer::Update( float deltaSeconds )
{
	g_theGame->Update( deltaSeconds );
}

void AuthServer::UpdateGameState( eGameState newGamestate )
{
	g_theGame->SetGameState( newGamestate );
}

int AuthServer::GetPlayerHealth()
{
	return g_theGame->GetPlayerHealth();
}

int AuthServer::GetBossHealth()
{
	return g_theGame->GetBossHealth();
}

SpriteDefinition const* AuthServer::GetPlayerWeaponSprite() const
{
	return g_theGame->m_player->GetCurrentWeapon()->GetWeaponSpriteDef();
}

std::vector<Vertex_PCU> const& AuthServer::GetTileVertsToRender()
{
	return g_theGame->GetTileVertsToRender();
}

std::vector<Entity*> const& AuthServer::GetEntitiesToRender()
{
	return g_theGame->GetEntitiesToRender();
}

eGameState AuthServer::GetCurrentGameState()
{
	return g_theGame->m_gameState;
}

