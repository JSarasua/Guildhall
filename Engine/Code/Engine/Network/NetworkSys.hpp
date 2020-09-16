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

struct MessageHeader
{
	uint16_t m_id = 0;
	uint16_t m_size = 0;
};

struct ServerListeningMessage
{
	MessageHeader m_header = MessageHeader{ 1, 0};
	std::string m_gameName;

public:
	std::string ToString()
	{
		std::string listeningMessageStr;
		listeningMessageStr.append( (char*)&m_header, 4 );
		listeningMessageStr.append( m_gameName );
		return listeningMessageStr;
	}
};

struct TextMessage
{
	MessageHeader m_header = MessageHeader{ 2, 0 };
	std::string m_data;

public:
	std::string ToString()
	{
		std::string testMessageStr;
		testMessageStr.append( (char*)&m_header, 4 );
		testMessageStr.append( m_data );
		return testMessageStr;
	}
};

struct ClientDisconnecting
{
	MessageHeader m_header = MessageHeader{ 3, 0 };

public:
	std::string ToString()
	{
		std::string clientDisconnecting;
		clientDisconnecting.append( (char*)&m_header, 4 );
		return clientDisconnecting;
	}
};



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
	TCPSocket* m_TCPServerToClientSocket = nullptr;
	TCPSocket* m_TCPClientToServerSocket = nullptr;
};