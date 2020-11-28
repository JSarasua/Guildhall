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
	g_theEventSystem->SubscribeMethodToEvent( "UpdateInput", NOCONSOLECOMMAND, this, &AuthServer::HandleInput );

	m_clients.reserve(4);
}

void AuthServer::Shutdown()
{
	g_theGame->Shutdown();
}

void AuthServer::BeginFrame()
{
	for( size_t clientIndex = 0; clientIndex< m_clients.size(); clientIndex++ )
	{
		m_clients[clientIndex]->BeginFrame();
	}
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

	for( size_t clientIndex = 0; clientIndex < m_clients.size(); clientIndex++ )
	{
		m_clients[clientIndex]->Update( deltaSeconds );
	}
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
	//int length = args.GetValue("length", 0 );

	if( data.size() == 0 )
	{
		return true;
	}
	TCPMessage message = *(TCPMessage*)data.c_str();
	
	if( message.m_id == ADDPLAYER )
	{
		int sendToPort = message.m_port;
		int playerCount = (int)m_clients.size() + 1;
		int playerSlot = playerCount;
		if( playerCount < 4 )
		{
			std::string host = args.GetValue("ip","127.0.0.1");
			int port = 48010 + playerCount - 1;
			UDPGameConnection* newUDPConnection = new UDPGameConnection( host, sendToPort );
			newUDPConnection->Bind( port );
			Client* newRemoteClient = new RemoteClient( newUDPConnection );
			m_clients.push_back( newRemoteClient );

			if( g_theGame->m_gameState != ATTRACT )
			{
				g_theGame->AddPlayer( playerSlot );
			}

			newRemoteClient->SetPlayerID( playerSlot );
			uint16_t playerID = (uint16_t)playerSlot;

			TCPMessage connectMessage;
			connectMessage.m_id = ADDPLAYER;
			connectMessage.m_playerID = playerID;
			connectMessage.m_port = (uint16_t)port;

			char const* messageStr = (char const*)&connectMessage;

			m_TCPGameConnection->SendMessageToClient( messageStr, sizeof( connectMessage ) );

			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );


			for( size_t clientIndex = 0; clientIndex < m_clients.size(); clientIndex++ )
			{
				EventArgs createArgs;
				createArgs.SetValue( "position", Vec2() );
				createArgs.SetValue( "velocity", Vec2() );
				createArgs.SetValue( "orientation", 0.f );
				createArgs.SetValue( "speedMultiplier", 1.f );
				createArgs.SetValue( "entityType", ID_PLAYER );
				createArgs.SetValue( "defIndex", 0 );
				createArgs.SetValue( "entityID", (int)clientIndex + 1 );
				((RemoteClient*)newRemoteClient)->HandleCreateEntity( createArgs );
			}
			//Send Message of UDP connection
			//m_TCPGameConnection->SendMessageToClient()
		}
		EventArgs tcpArgs;
		tcpArgs.SetValue( "port", m_port );
		m_TCPGameConnection->StopTCPServer( tcpArgs );
		m_TCPGameConnection->StartTCPServer( tcpArgs );
	}






	return true;
}

bool AuthServer::HandleInput( EventArgs const& args )
{
	g_theEventSystem->FireEvent( "Input", NOCONSOLECOMMAND, &args );

	return true;
}

