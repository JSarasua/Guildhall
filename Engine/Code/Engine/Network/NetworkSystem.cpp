#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Core/StringUtils.hpp"



#pragma  comment( lib, "Ws2_32.lib" )

NetworkSystem::NetworkSystem()
{
	m_listenSocket = INVALID_SOCKET;
	m_clientSocket = INVALID_SOCKET;
	m_isListening = false;
	m_listenPort = -1;
	m_timeVal = timeval{ 0l, 0l };

	FD_ZERO( &m_listenSet );
}

NetworkSystem::~NetworkSystem()
{
	m_isListening = false;
	m_listenSocket = INVALID_SOCKET;
	m_clientSocket = INVALID_SOCKET;
	m_listenPort = -1;
	FD_ZERO( & m_listenSet );
	m_timeVal = timeval{ 0l, 0l };
}

void NetworkSystem::Startup()
{
	g_theEventSystem->SubscribeMethodToEvent( "StartTCPServer", CONSOLECOMMAND, this, &NetworkSystem::StartTCPServer  );
	
// 	g_theEventSystem->SubscribeMethodToEvent( "SendMessage", CONSOLECOMMAND, this, &NetworkSystem::StartTCPServer );
// 	g_theEventSystem->SubscribeMethodToEvent( "StopServer", CONSOLECOMMAND, this, &NetworkSystem::StartTCPServer );
// 	g_theEventSystem->SubscribeMethodToEvent( "Connect", CONSOLECOMMAND, this, &NetworkSystem::ConnectToTCPServer );
// 	g_theEventSystem->SubscribeMethodToEvent( "Disconnect", CONSOLECOMMAND, this, &NetworkSystem::DisconnectToTCPServer );
// 	g_theEventSystem->SubscribeToEvent()


	WSADATA wsaData;
	WORD wVersion MAKEWORD(2, 2);
	int iResult = WSAStartup( wVersion, &wsaData );

	g_theConsole->GuaranteeOrError( iResult == 0, Stringf( "Call to WSAStartup failed %i", WSAGetLastError()) );
}

void NetworkSystem::BeginFrame()
{
	if( m_isListening )
	{
		if( m_listenSocket == INVALID_SOCKET )
		{
			addrinfo addrHintsIn;
			addrinfo* pAddrOut;

			ZeroMemory( &addrHintsIn, sizeof( addrHintsIn ) );
			addrHintsIn.ai_family = AF_INET;		//IPv4 address
			addrHintsIn.ai_socktype = SOCK_STREAM;
			addrHintsIn.ai_protocol = IPPROTO_TCP;
			addrHintsIn.ai_flags	= AI_PASSIVE;

			std::string serverPort( "48000" );
			//NULL should be changed to hostname.c_str()
			int iResult = getaddrinfo( NULL, serverPort.c_str(), &addrHintsIn, &pAddrOut ); //NULL because we'ring binding to local host

																						   //Doesn't call WSAGetLastError
			g_theConsole->GuaranteeOrError( iResult == 0, Stringf( "Call to getaddrinfo failed %i", iResult ) );


			//classify this
			m_listenSocket = socket( pAddrOut->ai_family, pAddrOut->ai_socktype, pAddrOut->ai_protocol );

			g_theConsole->GuaranteeOrError( m_listenSocket != INVALID_SOCKET, Stringf( "Call to socket failed %i", WSAGetLastError() ) );


			u_long blockingMode = 1;
			iResult = ioctlsocket( m_listenSocket, FIONBIO, &blockingMode );

			g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf("Call to ioctlsocket failed: %i", WSAGetLastError()) );
		
			iResult = bind( m_listenSocket, pAddrOut->ai_addr, (int)pAddrOut->ai_addrlen );
			g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf("Call to bind failed %i", WSAGetLastError() ) );

			iResult = listen( m_listenSocket, SOMAXCONN ); //Passing in max to get largest reasonable size
			g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Call to listen failed %i", WSAGetLastError() ) );
		}
		else if( m_clientSocket == INVALID_SOCKET )
		{
			FD_ZERO( &m_listenSet );
			FD_SET( m_listenSocket, &m_listenSet );
			int iResult = select( 0, &m_listenSet, nullptr, nullptr, &m_timeVal );

			g_theConsole->GuaranteeOrError( iResult != INVALID_SOCKET, Stringf("Call to select failed %i", WSAGetLastError() ) );
		
			if( FD_ISSET( m_listenSocket, &m_listenSet ) )
			{
				m_clientSocket = accept( m_listenSocket, NULL, NULL );
				g_theConsole->GuaranteeOrError( m_clientSocket != INVALID_SOCKET, Stringf( "Call to accept failed %i", WSAGetLastError() ) );
				g_theConsole->PrintString( Rgba8::WHITE, Stringf("Client connected from %s", GetAddress().c_str() ) );
			}
		}
		else
		{
			static bool message = false;
			if( !message )
			{
				std::array<char, 256> buffer;
				int iResult = ::recv( m_clientSocket, &buffer[0], (int)buffer.size() - 1, 0 );
				g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Call to recv failed %i", WSAGetLastError() ) );

				if( iResult == 0 )
				{
					g_theConsole->ErrorString( Stringf( "Call to recv return 0" ) );
				}
				else
				{
					buffer[iResult] = NULL;
					g_theConsole->PrintString( Rgba8::WHITE, Stringf( "Client messageg: %s", &buffer[0] ) );
				
					std::string msg("Hello client");
					iResult = send( m_clientSocket, msg.c_str(), (int)msg.length(), 0 );
					g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Call to send failed %i", WSAGetLastError() ) );
				}
			}
			message = true;
		}
	}
}

void NetworkSystem::EndFrame()
{

}

void NetworkSystem::Shutdown()
{
	int iResult = WSACleanup();
	if( iResult == SOCKET_ERROR )
	{
		g_theConsole->ErrorString( Stringf("Call to WSACleanp failed %i", WSAGetLastError() ) );
	}
}

std::string NetworkSystem::GetAddress()
{
	std::array<char, 128> addressStr;
	sockaddr clientAddr;
	int addrSize = sizeof( clientAddr );

	int iResult = getpeername( m_clientSocket, &clientAddr, &addrSize );

	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf("Call to getpeername failed %i", WSAGetLastError() ) );

	DWORD outlen = static_cast<DWORD>(addressStr.size());
	iResult = WSAAddressToStringA( &clientAddr, addrSize, nullptr, &addressStr[0], &outlen );

	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf("Call to WSAAddressToStringA failed %i", WSAGetLastError() ) );

	addressStr[outlen - 1] = NULL;

	if( iResult == SOCKET_ERROR )
	{
		return std::string();
	}
	else
	{
		return std::string( &addressStr[0] );
	}

}


bool NetworkSystem::StartTCPServer( EventArgs const& args )
{
	UNUSED( args );
	//start listening to data from the client
	//int port = args.GetValue( "port", 48000 );
	m_isListening = true;
	return true;
}


