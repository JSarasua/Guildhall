#include "Engine/Network/TCPSocket.hpp"

struct MessageHeader
{
	uint16_t m_id = 0;
	uint16_t m_size = 0;
};


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

TCPSocket::TCPSocket( TCPSocket const& src )
{
	m_socket = src.m_socket;
	m_timeval = src.m_timeval;
	m_bufferSize = src.m_bufferSize;
	m_bufferPtr = src.m_bufferPtr;
	m_fdSet = src.m_fdSet;
}

TCPSocket::~TCPSocket()
{
	delete m_bufferPtr;
}

void TCPSocket::operator=( TCPSocket const& src )
{
	m_socket = src.m_socket;
	m_timeval = src.m_timeval;
	m_bufferSize = src.m_bufferSize;
	m_bufferPtr = src.m_bufferPtr;
	m_fdSet = src.m_fdSet;
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

void TCPSocket::Send( std::string dataToSend, size_t length )
{
// 	std::array<char, 256> buffer;
// 	MessageHeader* headerPtr = reinterpret_cast<MessageHeader*>(&buffer[0]);
// 	headerPtr->m_id = 1;
// 	headerPtr->m_size = 3;
// 	buffer[4] = 'F';
// 	buffer[5] = 'u';
// 	buffer[6] = 'n';

	//int iResult = send( m_socket, (char const*)&buffer[0], 7, 0 );
	int iResult = send( m_socket, dataToSend.c_str(), (int)length, 0 );
	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf("Call to send failed %i", WSAGetLastError() ) );

}

TCPData TCPSocket::Receive()
{
	std::array<char, 256> buffer;
	int iResult = ::recv( m_socket, &buffer[0], (int)buffer.size() - 1, 0 );
	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Call to recv failed %i", WSAGetLastError() ) );

	if( iResult == 0 )
	{
		g_theConsole->ErrorString( Stringf( "Call to recv return 0" ) );
	}

	TCPData data = TCPData( (int)buffer.size(), &buffer[0] );
	return data;
}

bool TCPSocket::IsDataAvailable()
{
	FD_ZERO( &m_fdSet );
	FD_SET( m_socket, &m_fdSet );
	int iResult = select( 0, &m_fdSet, NULL, NULL, &m_timeval );
	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Select on socket in non-blocking mode failed, error = %i", WSAGetLastError() ) );

	return FD_ISSET( m_socket, &m_fdSet );
}

void TCPSocket::Shutdown()
{
	int iResult = ::shutdown( m_socket, SD_SEND );
	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Call to shutdown failed, error = %i", WSAGetLastError() ) );
}

void TCPSocket::Close()
{
	int iResult = closesocket( m_socket );
	g_theConsole->GuaranteeOrError( iResult != SOCKET_ERROR, Stringf( "Call to closesocket failed, error = %i", WSAGetLastError() ) );

	m_socket = INVALID_SOCKET;
}

