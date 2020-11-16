#include "Game/RemoteServer.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Time/Clock.hpp"
#include "Game/Actor.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Game/Game.hpp"
#include "Game/UDPGameConnection.hpp"

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

	g_theEventSystem->SubscribeMethodToEvent( "TCPMessageReceived", NOCONSOLECOMMAND, this, &RemoteServer::HandleReceiveTCPMessage );
	g_theEventSystem->SubscribeMethodToEvent( "UpdateInput", NOCONSOLECOMMAND, this, &RemoteServer::HandleInput );

	m_port = g_theGame->m_rand.RollRandomIntInRange( 48500, 48550 );
	if( m_TCPGameConnection )
	{
		//EventArgs args;
		TCPMessage message;
		message.m_id = ADDPLAYER;
		message.m_port = (uint16_t)m_port;
		char const* messageStr = (char const*)&message;
		int messageSize = sizeof( message );

		m_TCPGameConnection->SendMessageToServer( messageStr, messageSize );
	}
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
	g_theGame->UpdateRemote( m_UDPGameConnection, deltaSeconds );
	//g_theGame->Update( deltaSeconds );
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

bool RemoteServer::HandleReceiveTCPMessage( EventArgs const& args )
{
	EventArgs dcArgs;
	m_TCPGameConnection->DisconnectClient( dcArgs );

	std::string data = args.GetValue( "data", std::string() );
	//int length = args.GetValue( "length", 0 );

	if( data.size() == 0 )
	{
		return true;
	}
	TCPMessage message = *(TCPMessage*)data.c_str();

	if( message.m_id == ADDPLAYER )
	{
		g_theClient->SetPlayerID( message.m_playerID );
		int port = message.m_port;
		std::string host = "127.0.0.1";
		UDPGameConnection* newUDPConnection = new UDPGameConnection( host, port );
		newUDPConnection->Bind( m_port );

		if( m_UDPGameConnection )
		{
			delete m_UDPGameConnection;
			m_UDPGameConnection = newUDPConnection;
		}
		else
		{
			m_UDPGameConnection = newUDPConnection;
		}
	}

	return true;
}

bool RemoteServer::HandleInput( EventArgs const& args )
{
	if( m_UDPGameConnection )
	{
		int changeWeapons = args.GetValue( "changeWeapons", 0 );
		bool isDodging = args.GetValue( "isDodging", false );
		bool isShooting = args.GetValue( "isShooting", false );
		Vec2 mousePos = args.GetValue( "mousePos", Vec2() );
		Vec2 moveVec = args.GetValue( "moveVec", Vec2() );

		InputMessage message;
		message.changeWeapons = changeWeapons;
		message.isDodging = isDodging;
		message.isShooting = isShooting;
		message.mousePos = mousePos;
		message.moveVec = moveVec;
		InputPacket packet;
		packet.header.m_id = UPDATEPLAYER;
		packet.message = message;
		
		std::string messageStr = packet.ToString();
		m_UDPGameConnection->SendUDPMessage( messageStr );
		

	}

	return true;
}

