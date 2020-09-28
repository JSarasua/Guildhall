#pragma once
#include "Engine/Network/NetworkSys.hpp"
#include <string>

class UDPSocket
{
public:
	UDPSocket( std::string const& host, int port );
	~UDPSocket();

	void Bind( int port );
	int Send();
	int Receive();

	void Close();

protected:

private:
	sockaddr_in m_toAddress;
	SOCKET m_socket;
};