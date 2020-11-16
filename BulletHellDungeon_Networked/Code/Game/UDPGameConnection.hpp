#pragma once
#include "Game/GameCommon.hpp"
#include <string>
#include "Engine/Queues/BlockingQueue.hpp"
#include "Engine/Queues/SynchronizedLockFreeQueue.hpp"

#include <thread>
#include <atomic>

class UDPSocket;

struct BufferMessage;

class UDPGameConnection
{
public:
	UDPGameConnection( std::string const& host, int port );
	~UDPGameConnection();

	void Bind( int port );
	int Send( int length );
	int Receive();
	void Close();

	bool IsSocketValid();
	bool IsSocketBound();

	static const int BufferSize = 512;
	using Buffer = std::array<char, BufferSize>;

	Buffer& SendBuffer();
	Buffer& ReceiveBuffer();

	void SendUDPMessage( std::string const& message );
	BufferMessage ReceiveMessage();

	std::string GetLastReceiveAddress();

	AddressedUDPPacket PopFirstReceivedPacket();

private:
	void ReaderThread();
	void WriterThread();

private:
	BlockingQueue<std::string> m_writerQueue;
	SynchronizedLockFreeQueue<AddressedUDPPacket> m_readerQueue;

	std::thread* m_UDPReaderThread = nullptr;
	std::thread* m_UDPWriterThread = nullptr;
	std::atomic<bool> m_isQuitting = false;

	UDPSocket* m_UDPSocket = nullptr;
};

struct BufferMessage
{
	int messageLength;
	UDPGameConnection::Buffer const& buffer;
};