#include "Engine/Network/TCPClient.hpp"


TCPSocket TCPClient::Connect( std::string const& host, int port )
{
	// Resolve the port locally
	addrinfo  addrHintsIn;
	addrinfo* pAddrOut = NULL;

	ZeroMemory( &addrHintsIn, sizeof( addrHintsIn ) );
	addrHintsIn.ai_family = AF_INET;
	addrHintsIn.ai_socktype = SOCK_STREAM;
	addrHintsIn.ai_protocol = IPPROTO_TCP;

	std::string serverPort( "48000" );
	serverPort = std::string( Stringf( "%i", port ) );
	int iResult = ::getaddrinfo( host.c_str(), serverPort.c_str(), &addrHintsIn, &pAddrOut );

	// Create the connection socket.
	SOCKET socket = ::socket( pAddrOut->ai_family, pAddrOut->ai_socktype, pAddrOut->ai_protocol );
	g_theConsole->GuaranteeOrError( socket != INVALID_SOCKET, Stringf( "Connection socket creation error, error = %i", WSAGetLastError() ) );

	// Connect to the server
	iResult = ::connect( socket, pAddrOut->ai_addr, (int)pAddrOut->ai_addrlen );
	if( iResult == SOCKET_ERROR )
	{
		closesocket( socket );
		socket = INVALID_SOCKET;
	}
	freeaddrinfo( pAddrOut );

	g_theConsole->GuaranteeOrError( socket != INVALID_SOCKET, "Connection failed" );

	//Mode check should probably have an if block around this
	u_long winsockmode = 1;
	iResult = ioctlsocket( socket, FIONBIO, &winsockmode );
	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "IOCTL on server listen socket failed, error = %i", WSAGetLastError() ) );

	return TCPSocket( socket );
}

