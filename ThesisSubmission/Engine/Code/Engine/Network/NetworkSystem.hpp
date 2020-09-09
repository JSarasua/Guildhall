#pragma once
#include "Engine/Core/EngineCommon.hpp"


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

	bool StartTCPServer( EventArgs const& args );
protected:

private:
	int m_listenPort = -1;
	bool m_isListening = false;
	unsigned long long  m_listenSocket;
};