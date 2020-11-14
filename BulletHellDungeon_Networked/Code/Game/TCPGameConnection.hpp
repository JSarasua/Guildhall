#pragma once
#include "Engine/Core/EngineCommon.hpp"

class TCPServer;
class TCPClient;
class TCPSocket;

class TCPGameConnection
{
public:
	TCPGameConnection() = default;
	~TCPGameConnection();

	void Startup();
	void BeginFrame();
	void Shutdown();

	//Server commands
	bool StartTCPServer( EventArgs const& args );
	bool SendMessageToClient( EventArgs const& args );
	bool SendMessageToClient( char const* message, int size );
	bool StopTCPServer( EventArgs const& args );
	//Client commands
	bool TCPClientConnect( EventArgs const& args );
	bool SendMessageToServer( EventArgs const& args );
	bool SendMessageToServer( char const* message, int size );
	bool DisconnectClient( EventArgs const& args );

private:
	TCPServer* m_TCPServer = nullptr;
	TCPClient* m_TCPClient = nullptr;
	TCPSocket* m_TCPServerToClientSocket = nullptr;
	TCPSocket* m_TCPClientToServerSocket = nullptr;
};