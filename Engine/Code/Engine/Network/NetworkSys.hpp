#pragma once
#include "Engine/Core/EngineCommon.hpp"

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif // !_WINSOCK_DEPRECATED_NO_WARNINGS



#include <string>
#include <array>
#include <vector>
#include <winsock2.h>
#include <WS2tcpip.h>

class TCPServer;
class TCPClient;
class TCPSocket;

class NetworkSys
{
public:
	NetworkSys() = default;
	~NetworkSys();

	void Startup();
	void BeginFrame();
	//void EndFrame();
	void Shutdown();


	TCPServer* CreateTCPServer();
	TCPClient* CreateTCPClient();

	//Server commands
	bool StartTCPServer( EventArgs const& args );
	bool SendMessageToClient( EventArgs const& args );
	bool StopTCPServer( EventArgs const& args );
	//Client commands
	bool TCPClientConnect( EventArgs const& args );
	bool SendMessageToServer( EventArgs const& args );
	bool DisconnectClient( EventArgs const& args );

private:
	std::vector<TCPServer*> m_TCPServers;
	std::vector<TCPClient*> m_TCPClients;

	TCPServer* m_TCPServer = nullptr;
	TCPClient* m_TCPClient = nullptr;
	TCPSocket* m_TCPServerSocket = nullptr;
	TCPSocket* m_TCPClientSocket = nullptr;
};