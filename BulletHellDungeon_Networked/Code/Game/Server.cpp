#include "Game/Server.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Time/Clock.hpp"
#include "Game/Actor.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Game/WeaponDefinition.hpp"

Server::Server()
{
	g_theGame = new Game();
}

Server::~Server()
{
	delete g_theGame;
}

// void Server::Startup()
// {
// 	g_theGame->Startup();
// }
// 
// void Server::Shutdown()
// {
// 	g_theGame->Shutdown();
// }
// 
// void Server::BeginFrame()
// {
// 
// }
// 
// void Server::EndFrame()
// {
// 
// }
// 
// void Server::RestartGame()
// {
// 	g_theGame->Shutdown();
// 	delete g_theGame;
// 	g_theGame = new Game();
// 	g_theGame->Startup();
// }
// 
// void Server::PauseGame()
// {
// 	if( g_theGame->m_gameState == PLAYING )
// 	{
// 		g_theGame->m_gameState = PAUSED;
// 	}
// 
// 	Clock* masterClock = Clock::GetMaster();
// 	masterClock->Pause();
// }
// 
// void Server::UnpauseGame()
// {
// 	if( g_theGame->m_gameState == PAUSED )
// 	{
// 		g_theGame->m_gameState = PLAYING;
// 	}
// 
// 	Clock* masterClock = Clock::GetMaster();
// 	masterClock->Resume();
// }
// 
// void Server::Update( float deltaSeconds )
// {
// 	g_theGame->Update( deltaSeconds );
// }
// 
// void Server::UpdateGameState( eGameState newGamestate )
// {
// 	g_theGame->SetGameState( newGamestate );
// }
// 
// int Server::GetPlayerHealth()
// {
// 	return g_theGame->GetPlayerHealth();
// }
// 
// int Server::GetBossHealth()
// {
// 	return g_theGame->GetBossHealth();
// }
// 
// SpriteDefinition const* Server::GetPlayerWeaponSprite() const
// {
// 	return g_theGame->m_player->GetCurrentWeapon()->GetWeaponSpriteDef();
// }
// 
// std::vector<Vertex_PCU> const& Server::GetTileVertsToRender()
// {
// 	return g_theGame->GetTileVertsToRender();
// }
// 
// std::vector<Entity*> const& Server::GetEntitiesToRender()
// {
// 	return g_theGame->GetEntitiesToRender();
// }
// 
// eGameState Server::GetCurrentGameState()
// {
// 	return g_theGame->m_gameState;
// }
// 
