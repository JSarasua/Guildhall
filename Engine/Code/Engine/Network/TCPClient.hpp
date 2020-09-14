#pragma once
#include "Engine/Network/TCPSocket.hpp"

class TCPClient
{
	friend class NetworkSys;
public:
	TCPClient() {}
	~TCPClient() {}

	TCPSocket Connect( std::string const& host, int port );
	

private:
	//Mode m_mode

};