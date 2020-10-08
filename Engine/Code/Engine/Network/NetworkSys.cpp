#include "Engine/Network/NetworkSys.hpp"
#include "Engine/Network/TCPClient.hpp"
#include "Engine/Network/TCPServer.hpp"
#include "Engine/Network/UDPSocket.hpp"
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

	g_theEventSystem->SubscribeMethodToEvent( "OpenUDPPort", CONSOLECOMMAND, this, &NetworkSys::OpenUDPPort );
	g_theEventSystem->SubscribeMethodToEvent( "SendUDPMessage", CONSOLECOMMAND, this, &NetworkSys::SendUDPMessage );
	g_theEventSystem->SubscribeMethodToEvent( "CloseUDPPort", CONSOLECOMMAND, this, &NetworkSys::CloseUDPPort );

	WSADATA wsaData;
	WORD wVersion MAKEWORD( 2, 2 );
	int iResult = WSAStartup( wVersion, &wsaData );

	g_theConsole->GuaranteeOrError( iResult == 0, Stringf( "Call to WSAStartup failed %i", WSAGetLastError() ) );

	m_TCPServer = new TCPServer();
	m_TCPClient = new TCPClient();
	m_TCPClientToServerSocket = new TCPSocket();
	m_TCPServerToClientSocket = new TCPSocket();

	m_UDPReaderThread = new std::thread( &NetworkSys::ReaderThread, this );
	m_UDPWriterThread = new std::thread( &NetworkSys::WriterThread, this );
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
				listeningMessage.m_header.m_size = (uint16_t)listeningMessage.m_gameName.size();
				std::string listeningMessageStr = listeningMessage.ToString();

				m_TCPServerToClientSocket->Send( listeningMessageStr, listeningMessageStr.size() );

			}
		}
		else if( m_TCPServerToClientSocket->IsSocketValid() )
		{
			if( m_TCPServerToClientSocket->IsDataAvailable() )
			{
				TCPData data = m_TCPServerToClientSocket->Receive();
// 				if( data.GetLength() != 0 )
// 				{
					const char* dataArray = data.GetData();
					MessageHeader* messageHeader = (MessageHeader*)dataArray;
					if( messageHeader->m_id == CLIENTDISCONNECT )
					{
						g_theConsole->PrintString( Rgba8::GREEN, "Client is disconnecting" );
						m_TCPServerToClientSocket->Close();
					}
					else if( messageHeader->m_id == TEXTMESSAGE )
					{
						std::string dataStr = dataArray + 4;
						//std::string dataStr = std::string( data.GetData(), data.GetLength() );
						g_theConsole->PrintString( Rgba8::GREEN, "Message received from client" );
						g_theConsole->PrintString( Rgba8::WHITE, dataStr );
					}

				//}
			}
		}
	}

	if( nullptr != m_TCPClientToServerSocket )
	{
		if( m_TCPClientToServerSocket->IsSocketValid() && m_TCPClientToServerSocket->IsDataAvailable() )
		{
			TCPData data = m_TCPClientToServerSocket->Receive();
			const char* dataArray = data.GetData();

			MessageHeader* messageHeader = (MessageHeader*)dataArray;
			if( messageHeader->m_id == SERVERLISTENING )
			{
				g_theConsole->PrintString( Rgba8::GREEN, "Server is listening for game:" );
				std::string dataStr = dataArray + 4;
				g_theConsole->PrintString( Rgba8::WHITE, dataStr );
			}
			else
			{
				std::string dataStr = dataArray + 4;
				g_theConsole->PrintString( Rgba8::GREEN, "Message received from server" );
				g_theConsole->PrintString( Rgba8::WHITE, dataStr );
			}


		}
	}

	if( nullptr != m_UDPSocket )
	{
		if( m_UDPSocket->IsSocketValid() )
		{
			if( m_readerQueue.size() > 0 )
			{
				std::string message = m_readerQueue.pop();
				//const char* message = m_readerQueue.pop().c_str();
				MessageHeader* messageHeader = (MessageHeader*)&message;
				if( messageHeader->m_id == TEXTMESSAGE )
				{

				}
				g_theConsole->PrintString( Rgba8::GREEN, "UDP Message received" );
				g_theConsole->PrintString( Rgba8::WHITE, message );

			}

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

	m_isQuitting = true;
	m_writerQueue.Quit();
	m_UDPReaderThread->join();
	m_UDPWriterThread->join();
}

bool NetworkSys::StartTCPServer( EventArgs const& args )
{
	int port = args.GetValue( "port", 48000 );

	m_TCPServer->Bind( port );
	m_TCPServer->Listen();

	g_theConsole->PrintString( Rgba8::WHITE, Stringf( "Server listening on port: %i", port ) );
	return true;
}

bool NetworkSys::SendMessageToClient( EventArgs const& args )
{
	if( m_TCPServerToClientSocket->IsSocketValid() )
	{
		std::string message = args.GetValue( "msg", "Invalid message" );
		TextMessage textMessage;
		textMessage.m_data = message;
		textMessage.m_header.m_size = (uint16_t)message.size();
		std::string textMessageStr = textMessage.ToString();
		m_TCPServerToClientSocket->Send( textMessageStr, textMessageStr.size() );
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
	if( m_TCPServerToClientSocket->IsSocketValid() )
	{
		m_TCPServerToClientSocket->Close();
	}
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
// 		TextMessage textMessage;
// 		textMessage.m_data = message;
// 		textMessage.m_header.m_size = (uint16_t)message.size();
// 		std::string testMessageStr = textMessage.ToString();
// 		m_TCPClientToServerSocket->Send( testMessageStr, testMessageStr.size() );
		m_TCPClientToServerSocket->Send( message, message.size() );
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

bool NetworkSys::OpenUDPPort( EventArgs const& args )
{
	int bindPort = args.GetValue( "bindPort", 48000 );
	int sendToPort = args.GetValue( "sendToPort", 48001 );

	m_UDPSocket = new UDPSocket( "127.0.0.1", sendToPort );
	m_UDPSocket->Bind( bindPort );
// 	m_UDPReceiverSocket = new UDPSocket( "127.0.0.1", bindPort );
// 	m_UDPSenderSocket = new UDPSocket( "127.0.0.1", sendToPort );
// 
// 	m_UDPReceiverSocket->Bind( bindPort );

	return false;
}

bool NetworkSys::SendUDPMessage( EventArgs const& args )
{
	if( m_UDPSocket )
	{
		std::string message = args.GetValue("msg","Default Message" );

// 		TextMessage textMessage;
// 		textMessage.m_data = message;
// 		textMessage.m_header.m_size = (uint16_t)message.size();
// 		std::string textMessageStr = textMessage.ToString();
/*		m_TCPServerToClientSocket->Send( textMessageStr, textMessageStr.size() );*/
//		m_TCPServerToClientSocket->Send( textMessageStr, textMessageStr.size() );

		UDPSocket::Buffer& buffer = m_UDPSocket->SendBuffer();
		memcpy( &buffer[0], message.c_str(), message.size() );
		m_UDPSocket->Send( (int)message.size() );
	}

	return false;
}

bool NetworkSys::CloseUDPPort( EventArgs const& args )
{
	//TODO

	return false;
}

//Calls recv on udp socket
//when it gets a message it puts it in queue
void NetworkSys::ReaderThread()
{
	while( !m_isQuitting )
	{
		if( m_UDPSocket && m_UDPSocket->IsSocketValid() && m_UDPSocket->IsSocketBound() )
		{
			int length = m_UDPSocket->Receive();

			if( length > 0 )
			{
				UDPSocket::Buffer& buffer = m_UDPSocket->ReceiveBuffer();
				std::string message = std::string( &buffer[0], length );

				m_readerQueue.push( message );
			}
		}

	}
}


//When a message is put in queue, it will write it to send
void NetworkSys::WriterThread()
{
	while( !m_isQuitting )
	{
		if( m_UDPSocket && m_UDPSocket->IsSocketValid() )
		{
			std::string message = m_writerQueue.pop();
			UDPSocket::Buffer& buffer = m_UDPSocket->SendBuffer();
			strcpy_s( &buffer[0], UDPSocket::BufferSize, message.c_str() );

			m_UDPSocket->Send( (int)message.size() );
		}
	}
}

