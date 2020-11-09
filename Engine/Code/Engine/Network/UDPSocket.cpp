#include "Engine/Network/UDPSocket.hpp"
#include <iostream>

#pragma  comment( lib, "Ws2_32.lib" )

#ifdef TEST_MODE
#define LOG_ERROR(...) printf(Stringf(__VA_ARGS__) + std::string("\n"))
#else
#define LOG_ERROR(...) g_theConsole->ErrorString(Stringf(__VA_ARGS__))
#endif

UDPSocket::UDPSocket( std::string const& host, int port ) : m_socket( INVALID_SOCKET )
{
	m_toAddress.sin_family = AF_INET;
	m_toAddress.sin_port = htons( (u_short)port );
	m_toAddress.sin_addr.s_addr = inet_addr( host.c_str() );
	
	m_socket = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( m_socket == INVALID_SOCKET )
	{
		std::cout << "Socket instantiate failed, error = " << WSAGetLastError() << std::endl;
	}

// 	struct timeval timeout;
// 	timeout.tv_sec = 10;
// 	timeout.tv_usec = 0;

// 	int iResult = setsockopt( m_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof( timeout ) );
// 	if( iResult == SOCKET_ERROR )
// 	{
// 		std::cout << "Socket setting timeout failed, error = " << WSAGetLastError() << std::endl;
// 	}
}

UDPSocket::~UDPSocket()
{
	Close();
}

void UDPSocket::Bind( int port )
{
	m_bindAddress.sin_family = AF_INET;
	m_bindAddress.sin_port = htons( (u_short)port );
	m_bindAddress.sin_addr.s_addr = htonl( INADDR_ANY );

	int iResult = ::bind( m_socket, (SOCKADDR*)& m_bindAddress, sizeof( m_bindAddress ) );

	if( iResult != 0 )
	{
		std::cout << "Socket instantiate failed, error = " << WSAGetLastError() << std::endl;

	}
	else
	{
		m_isBound = true;
	}
}

int UDPSocket::Send( int length )
{
	int iResult = ::sendto( m_socket, &m_sendBuffer[0], (int)length, 0, reinterpret_cast<SOCKADDR*>(&m_toAddress), sizeof(m_toAddress) );
	if( iResult == SOCKET_ERROR )
	{
		LOG_ERROR("Socket bind failed, error = %i",WSAGetLastError() );
	}
	return iResult;
}

int UDPSocket::Receive()
{
	int fromLen = BufferSize;

	int iResult = ::recvfrom( m_socket, &m_receiveBuffer[0], BufferSize, 0, reinterpret_cast<SOCKADDR*>(&m_bindAddress), &fromLen );
	if( iResult == SOCKET_ERROR )
	{
		int error = WSAGetLastError();
		if( error != 10060 )
		{
			std::cout << "Socket recvfrom failed, error = " << WSAGetLastError() << std::endl;
		}
	}
	else
	{
		std::array<char, 128> addressStr;
		DWORD outlen = static_cast<DWORD>(addressStr.size());
		int addrSize = sizeof( m_bindAddress );
		int iResultOfReceiveAddress = WSAAddressToStringA( reinterpret_cast<SOCKADDR*>(&m_bindAddress), addrSize, nullptr, &addressStr[0], &outlen );
		
		if( iResultOfReceiveAddress == SOCKET_ERROR )
		{
			std::cout << "Call to WSAAddressToStringA failed: " << WSAGetLastError() << std::endl;
		}

		addressStr[outlen - 1] = NULL;

		m_receiveAddress = std::string( &addressStr[0] );
	}

	return iResult;
}

void UDPSocket::Close()
{
	if( m_socket != INVALID_SOCKET )
	{
		int iResult = closesocket( m_socket );
		if( iResult == SOCKET_ERROR )
		{
			std::cout << "Socket close failed, error = " << WSAGetLastError() << std::endl;

		}
		m_isBound = false;
	}
}

