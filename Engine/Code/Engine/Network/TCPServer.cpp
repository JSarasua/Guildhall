#include "Engine/Network/TCPServer.hpp"

TCPServer::TCPServer()
{
	m_listenSocket = INVALID_SOCKET;
	m_timeval = timeval{0l, 0l};
	FD_ZERO( &m_listenSet );
}

TCPServer::~TCPServer()
{}

void TCPServer::Bind( int port )
{
	

	addrinfo addrHintsIn;
	addrinfo* pAddrOut;

	ZeroMemory( &addrHintsIn, sizeof( addrHintsIn ) );
	addrHintsIn.ai_family = AF_INET;		//IPv4 address
	addrHintsIn.ai_socktype = SOCK_STREAM;
	addrHintsIn.ai_protocol = IPPROTO_TCP;
	addrHintsIn.ai_flags	= AI_PASSIVE;

	std::string serverPort( "48000" );
	serverPort = std::string( Stringf("%i", port ) );
	//NULL should be changed to hostname.c_str()
	int iResult = getaddrinfo( NULL, serverPort.c_str(), &addrHintsIn, &pAddrOut ); //NULL because we'ring binding to local host

																					//Doesn't call WSAGetLastError
	g_theConsole->GuaranteeOrError( iResult == 0, Stringf( "Call to getaddrinfo failed %i", iResult ) );


	//classify this
	m_listenSocket = socket( pAddrOut->ai_family, pAddrOut->ai_socktype, pAddrOut->ai_protocol );

	g_theConsole->GuaranteeOrError( m_listenSocket != INVALID_SOCKET, Stringf( "Call to socket failed %i", WSAGetLastError() ) );


	u_long blockingMode = 1;
	iResult = ioctlsocket( m_listenSocket, FIONBIO, &blockingMode );

	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Call to ioctlsocket failed: %i", WSAGetLastError() ) );

	iResult = bind( m_listenSocket, pAddrOut->ai_addr, (int)pAddrOut->ai_addrlen );
	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Call to bind failed %i", WSAGetLastError() ) );

}

void TCPServer::Listen()
{
	int iResult = listen( m_listenSocket, SOMAXCONN ); //Passing in max to get largest reasonable size
	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Call to listen failed %i", WSAGetLastError() ) );

}

void TCPServer::StopListen()
{
	if( m_listenSocket != INVALID_SOCKET )
	{
		int iResult = closesocket( m_listenSocket );
		g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Call to closesocket on server listen failed: %i", WSAGetLastError() ) );
	}
	m_listenSocket = INVALID_SOCKET;
}

TCPSocket TCPServer::Accept()
{
	SOCKET clientSocket = INVALID_SOCKET;

	FD_ZERO( &m_listenSet );
	FD_SET( m_listenSocket, &m_listenSet );
	int iResult = select( 0, &m_listenSet, nullptr, nullptr, &m_timeval );

	g_theConsole->GuaranteeOrError( iResult != INVALID_SOCKET, Stringf( "Call to select failed %i", WSAGetLastError() ) );

	if( FD_ISSET( m_listenSocket, &m_listenSet ) )
	{
		clientSocket = accept( m_listenSocket, NULL, NULL );
		g_theConsole->GuaranteeOrError( clientSocket != INVALID_SOCKET, Stringf( "Call to accept failed %i", WSAGetLastError() ) );
		
		
		TCPSocket tcpSocket( clientSocket );
		g_theConsole->PrintString( Rgba8::WHITE, Stringf( "Client connected from %s", tcpSocket.GetAddress().c_str() ) );
		return tcpSocket;
	}


	//Should only be called in error
	return TCPSocket( clientSocket );
}

