#include "Engine/Network/NetworkSys.hpp"
#include "Engine/Network/TCPClient.hpp"
#include "Engine/Network/TCPServer.hpp"
#include "Engine/Core/StringUtils.hpp"

#pragma  comment( lib, "Ws2_32.lib" )

NetworkSys::~NetworkSys()
{
	for( size_t clientIndex = 0; clientIndex < m_TCPClients.size(); clientIndex++ )
	{
		TCPClient* client = m_TCPClients[clientIndex];
		if( client )
		{
			delete client;
			
		}
	}

	for( size_t serverIndex = 0; serverIndex < m_TCPServers.size(); serverIndex++ )
	{
		TCPServer* server = m_TCPServers[serverIndex];
		if( server )
		{
			delete server;

		}
	}

	m_TCPClients.clear();
	m_TCPServers.clear();

	if( m_TCPServer )
	{
		delete m_TCPServer;
	}
	if( m_TCPClient )
	{
		delete m_TCPClient;
	}
	if( m_TCPServerSocket )
	{
		delete m_TCPServerSocket;
	}
	if( m_TCPClientSocket )
	{
		delete m_TCPClientSocket;
	}
}

void NetworkSys::Startup()
{
	g_theEventSystem->SubscribeMethodToEvent( "StartTCPServer", CONSOLECOMMAND, this, &NetworkSys::StartTCPServer );
	g_theEventSystem->SubscribeMethodToEvent( "SendMessageToClient", CONSOLECOMMAND, this, &NetworkSys::SendMessageToClient );
	g_theEventSystem->SubscribeMethodToEvent( "StopTCPServer", CONSOLECOMMAND, this, &NetworkSys::StopTCPServer );
	
	g_theEventSystem->SubscribeMethodToEvent( "TCPClientConnect", CONSOLECOMMAND, this, &NetworkSys::TCPClientConnect );
	g_theEventSystem->SubscribeMethodToEvent( "SendMessageToServer", CONSOLECOMMAND, this, &NetworkSys::SendMessageToServer );
	g_theEventSystem->SubscribeMethodToEvent( "DisconnectClient", CONSOLECOMMAND, this, &NetworkSys::DisconnectClient );


	WSADATA wsaData;
	WORD wVersion MAKEWORD( 2, 2 );
	int iResult = WSAStartup( wVersion, &wsaData );

	g_theConsole->GuaranteeOrError( iResult == 0, Stringf( "Call to WSAStartup failed %i", WSAGetLastError() ) );

}

void NetworkSys::Shutdown()
{
	int iResult = WSACleanup();
	if( iResult == SOCKET_ERROR )
	{
		g_theConsole->ErrorString( Stringf( "Call to WSACleanp failed %i", WSAGetLastError() ) );
	}
}

bool NetworkSys::StartTCPServer( EventArgs const& args )
{
	int port = args.GetValue( "port", 48000 );
	m_TCPServer = new TCPServer();

	m_TCPServer->Bind( port );
	m_TCPServer->Listen();

	//FINISH

	return true;
}

bool NetworkSys::SendMessageToClient( EventArgs const& args )
{
	//REMEMBER TCPSOCKET ISNT FINISHED
	if( m_TCPServerSocket )
	{

	}
	return true;
}

bool NetworkSys::StopTCPServer( EventArgs const& args )
{
	return true;
}

bool NetworkSys::TCPClientConnect( EventArgs const& args )
{
	return true;
}

bool NetworkSys::SendMessageToServer( EventArgs const& args )
{
	return true;
}

bool NetworkSys::DisconnectClient( EventArgs const& args )
{
	return true;
}

