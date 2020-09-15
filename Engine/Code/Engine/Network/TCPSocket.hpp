#pragma once
#include "Engine/Network/NetworkSys.hpp"

class TCPData
{
public:
	TCPData() {}
	TCPData( size_t length, char* dataPtr ) :
		m_length( length ),
		m_dataPtr( dataPtr )
	{}

	size_t GetLength() { return m_length; }
	char const* GetData() { return m_dataPtr; }

private:
	size_t m_length = 0;
	char* m_dataPtr = nullptr;
};

class TCPSocket
{
public:
	TCPSocket();
	TCPSocket( SOCKET socket, size_t bufferSize = 256 );
	TCPSocket( TCPSocket const& src );
	~TCPSocket();
	void operator=( TCPSocket const& src );

	std::string GetAddress();

	void Send( std::string dataToSend, size_t length ); //Might not need length
	TCPData Receive();
	bool IsDataAvailable();
	void Shutdown();
	void Close();
	bool IsSocketValid() { return m_socket != INVALID_SOCKET; }

private:
	//Mode m_mode;
	SOCKET m_socket;
	FD_SET m_fdSet;
	timeval m_timeval;

	size_t m_bufferSize = 0;
	size_t m_receiveSize = 0;
	char* m_bufferPtr = nullptr;
};