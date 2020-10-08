#pragma once
#include "Engine/Network/NetworkSys.hpp"
#include <string>

class UDPSocket
{
public:
	UDPSocket( std::string const& host, int port );
	~UDPSocket();

	void Bind( int port );
	int Send( int length );
	int Receive();
	void Close();

	bool IsSocketValid() { return m_socket != INVALID_SOCKET; }
	bool IsSocketBound() { return m_isBound; }

	static const int BufferSize = 512;
	using Buffer = std::array<char, BufferSize>;

	Buffer& SendBuffer() { return m_sendBuffer; }
	Buffer& ReceiveBuffer() { return m_receiveBuffer; }

	std::string GetLastReceiveAddress() { return m_receiveAddress; }

protected:

private:
	sockaddr_in m_toAddress;
	sockaddr_in m_bindAddress;
	SOCKET m_socket;

	Buffer m_sendBuffer;
	Buffer m_receiveBuffer;

	std::string m_receiveAddress;

	bool m_isBound = false;
};