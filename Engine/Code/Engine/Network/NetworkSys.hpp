#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Queues/BlockingQueue.hpp"
#include "Engine/Queues/SynchronizedLockFreeQueue.hpp"

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif // !_WINSOCK_DEPRECATED_NO_WARNINGS



#include <string>
#include <array>
#include <vector>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <atomic>

class TCPServer;
class TCPClient;
class TCPSocket;
class UDPSocket;

constexpr uint16_t SERVERLISTENING = 1;
constexpr uint16_t TEXTMESSAGE = 2;
constexpr uint16_t CLIENTDISCONNECT = 3;

struct MessageHeader
{
	uint16_t m_id = 0;
	uint16_t m_size = 0;
};

struct ServerListeningMessage
{
	MessageHeader m_header = MessageHeader{ SERVERLISTENING, 0};
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
	MessageHeader m_header = MessageHeader{ TEXTMESSAGE, 0 };
	std::string m_data;

public:
	std::string ToString()
	{
		std::string textMessageStr;
		textMessageStr.append( (char*)&m_header, 4 );
		textMessageStr.append( m_data );
		return textMessageStr;
	}
};

struct ClientDisconnecting
{
	MessageHeader m_header = MessageHeader{ CLIENTDISCONNECT, 0 };

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

	//UDP Commands
	bool OpenUDPPort( EventArgs const& args );
	bool SendUDPMessage( EventArgs const& args );
	bool CloseUDPPort( EventArgs const& args );

	void ReaderThread();
	void WriterThread();

private:
	std::vector<TCPServer*> m_TCPServers;
	std::vector<TCPClient*> m_TCPClients;

	TCPServer* m_TCPServer = nullptr;
	TCPClient* m_TCPClient = nullptr;
	TCPSocket* m_TCPServerToClientSocket = nullptr;
	TCPSocket* m_TCPClientToServerSocket = nullptr;

// 	UDPSocket* m_UDPReceiverSocket = nullptr;
// 	UDPSocket* m_UDPSenderSocket = nullptr;
	UDPSocket* m_UDPSocket = nullptr;

	BlockingQueue<std::string> m_writerQueue;
	SynchronizedLockFreeQueue<std::string> m_readerQueue;

	std::thread* m_UDPReaderThread = nullptr;
	std::thread* m_UDPWriterThread = nullptr;
	std::atomic<bool> m_isQuitting = false;
};