#include "Game/AuthServer.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Time/Clock.hpp"
#include "Game/Actor.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Game/Game.hpp"
#include "Game/RemoteClient.hpp"
#include "Game/UDPGameConnection.hpp"
#include "Game/Client.hpp"

AuthServer::AuthServer( Game* game ) : Server( game )
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

	g_theEventSystem->SubscribeMethodToEvent( "TCPMessageReceived", NOCONSOLECOMMAND, this, &AuthServer::HandleReceiveTCPMessage );

	m_clients.reserve(4);
	m_clients.push_back( nullptr );
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

bool AuthServer::HandleReceiveTCPMessage( EventArgs const& args )
{
	std::string data = args.GetValue("data", std::string() );
	int length = args.GetValue("length", 0 );

	if( data.size() == 0 )
	{
		return true;
	}

	Header header = *(Header*)data.c_str();
	
	if( header.m_id == ADDPLAYER )
	{
		int playerCount = (int)m_clients.size() + 1;
		int playerSlot = playerCount - 1;
		if( playerCount < 4 )
		{
			std::string host = "127.0.0.1";
			int port = 48010 + playerCount - 1;
			UDPGameConnection* newUDPConnection = new UDPGameConnection( host, port );
			Client* newRemoteClient = new RemoteClient( newUDPConnection );
			m_clients.push_back( nullptr );

			if( g_theGame->m_gameState != ATTRACT )
			{
				g_theGame->AddPlayer( playerSlot );
			}
		}
	}






	return true;
}

