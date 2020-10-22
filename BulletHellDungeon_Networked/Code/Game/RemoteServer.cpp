#include "Game/RemoteServer.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Time/Clock.hpp"
#include "Game/Actor.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Game/Game.hpp"

RemoteServer::RemoteServer( Game* game ): Server( game )
{
	//g_theGame = new Game();
}

RemoteServer::~RemoteServer()
{
	//delete g_theGame;
}

void RemoteServer::Startup()
{
	g_theGame->Startup();
}

void RemoteServer::Shutdown()
{
	g_theGame->Shutdown();
}

void RemoteServer::BeginFrame()
{

}

void RemoteServer::EndFrame()
{

}

void RemoteServer::RestartGame()
{
	g_theGame->Shutdown();
	delete g_theGame;
	g_theGame = new Game();
	g_theGame->Startup();
}

void RemoteServer::PauseGame()
{
	if( g_theGame->m_gameState == PLAYING )
	{
		g_theGame->m_gameState = PAUSED;
	}

	Clock* masterClock = Clock::GetMaster();
	masterClock->Pause();
}

void RemoteServer::UnpauseGame()
{
	if( g_theGame->m_gameState == PAUSED )
	{
		g_theGame->m_gameState = PLAYING;
	}

	Clock* masterClock = Clock::GetMaster();
	masterClock->Resume();
}

void RemoteServer::Update( float deltaSeconds )
{
	g_theGame->Update( deltaSeconds );
}

void RemoteServer::UpdateGameState( eGameState newGamestate )
{
	g_theGame->SetGameState( newGamestate );
}

int RemoteServer::GetPlayerHealth()
{
	return g_theGame->GetPlayerHealth();
}

int RemoteServer::GetBossHealth()
{
	return g_theGame->GetBossHealth();
}

SpriteDefinition const* RemoteServer::GetPlayerWeaponSprite() const
{
	return g_theGame->m_player->GetCurrentWeapon()->GetWeaponSpriteDef();
}

std::vector<Vertex_PCU> const& RemoteServer::GetTileVertsToRender()
{
	return g_theGame->GetTileVertsToRender();
}

std::vector<Entity*> const& RemoteServer::GetEntitiesToRender()
{
	return g_theGame->GetEntitiesToRender();
}

eGameState RemoteServer::GetCurrentGameState()
{
	return g_theGame->m_gameState;
}

