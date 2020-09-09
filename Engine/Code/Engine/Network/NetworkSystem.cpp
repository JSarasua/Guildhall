#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <winsock2.h>
#include <WS2tcpip.h>

#pragma  comment( lib, "Ws2_32.lib" )

NetworkSystem::NetworkSystem()
{
	m_listenSocket = INVALID_SOCKET;
}

NetworkSystem::~NetworkSystem()
{
	m_isListening = false;
	m_listenSocket = INVALID_SOCKET;
}

void NetworkSystem::Startup()
{
	g_theEventSystem->SubscribeMethodToEvent( "StartTCPServer", CONSOLECOMMAND, this, &NetworkSystem::StartTCPServer  );

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
			addrHintsIn.ai_family = AF_INET;
			addrHintsIn.ai_socktype = SOCK_STREAM;
			addrHintsIn.ai_protocol = IPPROTO_TCP;
			addrHintsIn.ai_flags	= AI_PASSIVE;

			std::string serverPort( "48000" );
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
			g_theConsole->GuaranteeOrError( iResult == SOCKET_ERROR, Stringf( "Call to listen failed %i", WSAGetLastError() ) );
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

bool NetworkSystem::StartTCPServer( EventArgs const& args )
{
	UNUSED( args );
	//start listening to data from the client
	//int port = args.GetValue( "port", 48000 );
	return true;
}


