#pragma once
#include "Engine/Network/NetworkSys.hpp"
#include "Engine/Network/TCPSocket.hpp"

class TCPServer
{
	friend class NetworkSys;
public:
	TCPServer();
	~TCPServer();

	void Bind( int port );
	void Listen();
	void StopListen();
	TCPSocket Accept();
	bool IsListenSocketValid() { return m_listenSocket != INVALID_SOCKET; }


private:
	//Mode m_mode; //may need for blocking/nonblocking
	// int m_listnPort = 48000;
	FD_SET m_listenSet;
	SOCKET m_listenSocket;
	timeval m_timeval;
};