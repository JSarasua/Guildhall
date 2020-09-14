#include "Engine/Network/TCPSocket.hpp"


TCPSocket::TCPSocket()
{
	m_socket = INVALID_SOCKET;
	m_timeval = timeval{0l, 0l};
	m_bufferSize = 256;
	m_receiveSize = 0;
	m_bufferPtr = nullptr;
	FD_ZERO( & m_fdSet );
	m_bufferPtr = new char[m_bufferSize];
}

TCPSocket::TCPSocket( SOCKET socket, size_t bufferSize /*= 256 */ )
{
	m_socket = socket;
	m_timeval = timeval{ 0l, 0l };
	m_bufferSize = bufferSize;
	m_receiveSize = 0;
	m_bufferPtr = nullptr;
	FD_ZERO( &m_fdSet );
	m_bufferPtr = new char[m_bufferSize];
}

std::string TCPSocket::GetAddress()
{
	std::array<char, 128> addressStr;
	sockaddr clientAddr;
	int addrSize = sizeof( clientAddr );

	int iResult = getpeername( m_socket, &clientAddr, &addrSize );

	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Call to getpeername failed %i", WSAGetLastError() ) );

	DWORD outlen = static_cast<DWORD>(addressStr.size());
	iResult = WSAAddressToStringA( &clientAddr, addrSize, nullptr, &addressStr[0], &outlen );

	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Call to WSAAddressToStringA failed %i", WSAGetLastError() ) );

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

