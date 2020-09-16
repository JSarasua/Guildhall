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
	if( m_TCPServerToClientSocket )
	{
		delete m_TCPServerToClientSocket;
	}
	if( m_TCPClientToServerSocket )
	{
		delete m_TCPClientToServerSocket;
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

	m_TCPServer = new TCPServer();
	m_TCPClient = new TCPClient();
	m_TCPClientToServerSocket = new TCPSocket();
	m_TCPServerToClientSocket = new TCPSocket();
}

void NetworkSys::BeginFrame()
{
	//check socket
	if( nullptr != m_TCPServerToClientSocket )
	{
		if( m_TCPServer->m_listenSocket != INVALID_SOCKET &&
			!m_TCPServerToClientSocket->IsSocketValid() )
		{
			*m_TCPServerToClientSocket = m_TCPServer->Accept();
			if( m_TCPServerToClientSocket->IsSocketValid() )
			{
				ServerListeningMessage listeningMessage;
				listeningMessage.m_gameName= "Doomenstein";
				std::string listeningMessageStr = listeningMessage.ToString();

				m_TCPServerToClientSocket->Send( listeningMessageStr, listeningMessageStr.size() );

			}
		}
		else if( m_TCPServerToClientSocket->IsSocketValid() )
		{
			if( m_TCPServerToClientSocket->IsDataAvailable() )
			{
				TCPData data = m_TCPServerToClientSocket->Receive();
				if( data.GetLength() != 0 )
				{
					const char* dataArray = data.GetData();
					std::string dataStr = dataArray + 4;
					//std::string dataStr = std::string( data.GetData(), data.GetLength() );
					g_theConsole->PrintString( Rgba8::GREEN, "Message received from client" );
					g_theConsole->PrintString( Rgba8::WHITE, dataStr );
				}
			}
		}
	}

	if( nullptr != m_TCPClientToServerSocket )
	{
		if( m_TCPClientToServerSocket->IsSocketValid() && m_TCPClientToServerSocket->IsDataAvailable() )
		{
			TCPData data = m_TCPClientToServerSocket->Receive();
			const char* dataArray = data.GetData();

			std::string dataStr = dataArray + 4;
			g_theConsole->PrintString( Rgba8::GREEN, "Message received from server" );
			g_theConsole->PrintString( Rgba8::WHITE, dataStr );
		}
	}
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
	//int port = args.GetValue( "port", 48000 );
	int port = 48000;

	m_TCPServer->Bind( port );
	m_TCPServer->Listen();
	//FINISH

	return true;
}

bool NetworkSys::SendMessageToClient( EventArgs const& args )
{
	if( m_TCPServerToClientSocket->IsSocketValid() )
	{
		std::string message = args.GetValue( "msg", "Invalid message" );
		TextMessage textMessage;
		textMessage.m_data = message;
		std::string testMessageStr = textMessage.ToString();
		m_TCPServerToClientSocket->Send( testMessageStr, testMessageStr.size() );
	}
	else
	{
		g_theConsole->ErrorString( "Can't call send message to client when not connected to client" );
	}
	return true;
}

bool NetworkSys::StopTCPServer( EventArgs const& args )
{
	UNUSED( args );

	m_TCPServer->StopListen();
	m_TCPServerToClientSocket->Close();
	return true;
}

bool NetworkSys::TCPClientConnect( EventArgs const& args )
{
	std::string host = args.GetValue("host", "" );
	int port = args.GetValue("port", 48000 );
	*m_TCPClientToServerSocket = m_TCPClient->Connect( host, port );

	return true;
}

bool NetworkSys::SendMessageToServer( EventArgs const& args )
{
	if( m_TCPClientToServerSocket->IsSocketValid() )
	{
		std::string message = args.GetValue( "msg", "Invalid message" );
		TextMessage textMessage;
		textMessage.m_data = message;
		std::string testMessageStr = textMessage.ToString();
		m_TCPClientToServerSocket->Send( testMessageStr, testMessageStr.size() );
	}
	else
	{
		g_theConsole->ErrorString( "Can't call send message to server when not connected to server" );
	}
	return true;
}

bool NetworkSys::DisconnectClient( EventArgs const& args )
{
	UNUSED( args );
	ClientDisconnecting clientDisconnecting;
	std::string disconnectStr = clientDisconnecting.ToString();
	m_TCPClientToServerSocket->Send( disconnectStr, disconnectStr.size() );
	m_TCPClientToServerSocket->Close();
	return true;
}

