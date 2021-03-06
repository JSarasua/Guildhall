#include "Game/TCPGameConnection.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Network/TCPClient.hpp"
#include "Engine/Network/TCPServer.hpp"
#include "Engine/Network/TCPSocket.hpp"
#include "Engine/Core/StringUtils.hpp"

#pragma  comment( lib, "Ws2_32.lib" )

TCPGameConnection::~TCPGameConnection()
{
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

void TCPGameConnection::Startup()
{
// 	g_theEventSystem->SubscribeMethodToEvent( "StartTCPServer", CONSOLECOMMAND, this, &TCPGameConnection::StartTCPServer );
// 	g_theEventSystem->SubscribeMethodToEvent( "SendMessageToClient", CONSOLECOMMAND, this, &TCPGameConnection::SendMessageToClient );
// 	g_theEventSystem->SubscribeMethodToEvent( "StopTCPServer", CONSOLECOMMAND, this, &TCPGameConnection::StopTCPServer );
// 
// 	g_theEventSystem->SubscribeMethodToEvent( "TCPClientConnect", CONSOLECOMMAND, this, &TCPGameConnection::TCPClientConnect );
// 	g_theEventSystem->SubscribeMethodToEvent( "SendMessageToServer", CONSOLECOMMAND, this, &TCPGameConnection::SendMessageToServer );
// 	g_theEventSystem->SubscribeMethodToEvent( "DisconnectClient", CONSOLECOMMAND, this, &TCPGameConnection::DisconnectClient );

	WSADATA wsaData;
	WORD wVersion MAKEWORD( 2, 2 );
	int iResult = WSAStartup( wVersion, &wsaData );

	g_theConsole->GuaranteeOrError( iResult == 0, Stringf( "Call to WSAStartup failed %i", WSAGetLastError() ) );

	m_TCPServer = new TCPServer();
	m_TCPClient = new TCPClient();
	m_TCPClientToServerSocket = new TCPSocket();
	m_TCPServerToClientSocket = new TCPSocket();
}

void TCPGameConnection::BeginFrame()
{
	//check socket
	if( nullptr != m_TCPServerToClientSocket )
	{
		if( m_TCPServer->IsListenSocketValid() &&
			!m_TCPServerToClientSocket->IsSocketValid() )
		{
			*m_TCPServerToClientSocket = m_TCPServer->Accept();
			if( m_TCPServerToClientSocket->IsSocketValid() )
			{
// 				ServerListeningMessage listeningMessage;
// 				listeningMessage.m_gameName= "BulletHellDungeon";
// 				listeningMessage.m_header.m_size = (uint16_t)listeningMessage.m_gameName.size();
// 				std::string listeningMessageStr = listeningMessage.ToString();
// 
// 				m_TCPServerToClientSocket->Send( listeningMessageStr, listeningMessageStr.size() );

			}
		}
		else if( m_TCPServerToClientSocket->IsSocketValid() )
		{
			if( m_TCPServerToClientSocket->IsDataAvailable() )
			{
				TCPData data = m_TCPServerToClientSocket->Receive();
				// 				if( data.GetLength() != 0 )
				// 				{
				//const char* dataArray = data.GetData();
				int dataLength = (int)data.GetLength();
				if( dataLength > 0 )
				{
					std::string dataStr;
					dataStr.resize( dataLength );
					memcpy( &dataStr[0], data.GetData(), dataLength );

					std::string receiveAddress = m_TCPServerToClientSocket->GetAddress();
					receiveAddress = SplitStringOnDelimeter( receiveAddress.c_str(), ':' )[0];

					EventArgs args;
					args.SetValue( "data", dataStr );
					args.SetValue( "length", dataLength );
					args.SetValue( "ip", receiveAddress );
					g_theEventSystem->FireEvent( "TCPMessageReceived", NOCONSOLECOMMAND, &args );
				}
			}
		}
	}

	if( nullptr != m_TCPClientToServerSocket )
	{
		if( m_TCPClientToServerSocket->IsSocketValid() && m_TCPClientToServerSocket->IsDataAvailable() )
		{
			TCPData data = m_TCPClientToServerSocket->Receive();

			int dataLength = (int)data.GetLength();
			if( dataLength > 0 )
			{
				std::string dataStr;
				dataStr.resize( dataLength );
				memcpy( &dataStr[0], data.GetData(), dataLength );

				std::string receiveAddress = m_TCPClientToServerSocket->GetAddress();
				receiveAddress = SplitStringOnDelimeter( receiveAddress.c_str(), ':' )[0];
				EventArgs args;
				args.SetValue( "data", dataStr );
				args.SetValue( "length", dataLength );
				args.SetValue( "ip", receiveAddress );
				g_theEventSystem->FireEvent( "TCPMessageReceived", NOCONSOLECOMMAND, &args );
			}
		}
	}

}

void TCPGameConnection::Shutdown()
{
	int iResult = WSACleanup();
	if( iResult == SOCKET_ERROR )
	{
		g_theConsole->ErrorString( Stringf( "Call to WSACleanp failed %i", WSAGetLastError() ) );
	}

}

bool TCPGameConnection::StartTCPServer( EventArgs const& args )
{
	int port = args.GetValue( "port", 48000 );

	m_TCPServer->Bind( port );
	m_TCPServer->Listen();

	g_theConsole->PrintString( Rgba8::WHITE, Stringf( "Server listening on port: %i", port ) );
	return true;
}

bool TCPGameConnection::SendMessageToClient( EventArgs const& args )
{
	if( m_TCPServerToClientSocket->IsSocketValid() )
	{
		std::string message = args.GetValue( "msg", "Invalid message" );
		size_t messageSize = args.GetValue( "size", 0 );

		m_TCPServerToClientSocket->Send( message, messageSize );
	}
	else
	{
		g_theConsole->ErrorString( "Can't call send message to client when not connected to client" );
	}
	return true;
}

bool TCPGameConnection::SendMessageToClient( char const* message, int size )
{
	if( m_TCPServerToClientSocket->IsSocketValid() )
	{
		m_TCPServerToClientSocket->Send( message, size );
	}
	else
	{
		g_theConsole->ErrorString( "Can't call send message to client when not connected to client" );
	}
	return true;
}

bool TCPGameConnection::StopTCPServer( EventArgs const& args )
{
	UNUSED( args );

	m_TCPServer->StopListen();
	if( m_TCPServerToClientSocket->IsSocketValid() )
	{
		m_TCPServerToClientSocket->Close();
	}
	return true;
}

bool TCPGameConnection::TCPClientConnect( EventArgs const& args )
{
	std::string host = args.GetValue( "host", "" );
	int port = args.GetValue( "port", 48000 );
	*m_TCPClientToServerSocket = m_TCPClient->Connect( host, port );

	return true;
}

bool TCPGameConnection::SendMessageToServer( EventArgs const& args )
{
	if( m_TCPClientToServerSocket->IsSocketValid() )
	{
		std::string message = args.GetValue( "msg", "Invalid message" );
		size_t size = args.GetValue( "size", 0 );
		m_TCPClientToServerSocket->Send( message, size );
	}
	else
	{
		g_theConsole->ErrorString( "Can't call send message to server when not connected to server" );
	}
	return true;
}

bool TCPGameConnection::SendMessageToServer( char const* message, int size )
{
	if( m_TCPClientToServerSocket->IsSocketValid() )
	{
		m_TCPClientToServerSocket->Send( message, size );
	}
	else
	{
		g_theConsole->ErrorString( "Can't call send message to server when not connected to server" );
	}
	return true;
}

bool TCPGameConnection::DisconnectClient( EventArgs const& args )
{
	UNUSED( args );
	ClientDisconnecting clientDisconnecting;
	std::string disconnectStr = clientDisconnecting.ToString();
	m_TCPClientToServerSocket->Send( disconnectStr, disconnectStr.size() );
	m_TCPClientToServerSocket->Close();
	return true;
}

