#pragma once
#include "Engine/Core/EngineCommon.hpp"

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif // !_WINSOCK_DEPRECATED_NO_WARNINGS



#include <string>
#include <array>
#include <winsock2.h>
#include <WS2tcpip.h>

class NetworkSystem
{
public:
	NetworkSystem();
	~NetworkSystem();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	bool IsListening() { return m_isListening; }
	void SetIsListening( bool isListening ) { m_isListening = isListening; }
	std::string GetAddress();

	bool StartTCPServer( EventArgs const& args );
protected:

private:
	int m_listenPort = -1;
	bool m_isListening = false;
	FD_SET m_listenSet;
	SOCKET  m_listenSocket;
	SOCKET m_clientSocket;
	timeval m_timeVal;
};