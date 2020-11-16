#include "Game/UDPGameConnection.hpp"
#include "Engine/Network/UDPSocket.hpp"

UDPGameConnection::UDPGameConnection( std::string const& host, int port )
{
	m_UDPSocket = new UDPSocket( host, port );

	m_UDPReaderThread = new std::thread( &UDPGameConnection::ReaderThread, this );
	m_UDPWriterThread = new std::thread( &UDPGameConnection::WriterThread, this );
}

UDPGameConnection::~UDPGameConnection()
{
	if( m_UDPSocket )
	{
		delete m_UDPSocket;
		m_UDPSocket = nullptr;
	}

	m_isQuitting = true;
	m_UDPReaderThread->join();
	m_UDPWriterThread->join();

	delete m_UDPReaderThread;
	delete m_UDPWriterThread;

}

void UDPGameConnection::Bind( int port )
{
	m_UDPSocket->Bind( port );
}

int UDPGameConnection::Send( int length )
{
	return m_UDPSocket->Send( length );
}

int UDPGameConnection::Receive()
{
	return m_UDPSocket->Receive();
}

void UDPGameConnection::Close()
{
	m_UDPSocket->Close();
}

bool UDPGameConnection::IsSocketValid()
{
	return m_UDPSocket->IsSocketValid();
}

bool UDPGameConnection::IsSocketBound()
{
	return m_UDPSocket->IsSocketBound();
}

UDPGameConnection::Buffer& UDPGameConnection::SendBuffer()
{
	Buffer& buffer = m_UDPSocket->SendBuffer();
	return buffer;
}

UDPGameConnection::Buffer& UDPGameConnection::ReceiveBuffer()
{
	Buffer& buffer = m_UDPSocket->ReceiveBuffer();
	return buffer;
}

void UDPGameConnection::SendUDPMessage( std::string const& message )
{
	m_writerQueue.push( message );
}

BufferMessage UDPGameConnection::ReceiveMessage()
{
	int messageSize = Receive();
	BufferMessage bufferMessage{ messageSize, ReceiveBuffer() };
	return bufferMessage;
}

std::string UDPGameConnection::GetLastReceiveAddress()
{
	return m_UDPSocket->GetLastReceiveAddress();
}

AddressedUDPPacket UDPGameConnection::PopFirstReceivedPacket()
{
	AddressedUDPPacket packet = m_readerQueue.pop();
	return packet;
}

//Calls recv on udp socket
//when it gets a message it puts it in queue
void UDPGameConnection::ReaderThread()
{
	while( !m_isQuitting )
	{
		if( m_UDPSocket && m_UDPSocket->IsSocketValid() && m_UDPSocket->IsSocketBound() )
		{
			int length = m_UDPSocket->Receive();

			if( length > 0 )
			{
				UDPSocket::Buffer& buffer = m_UDPSocket->ReceiveBuffer();
				std::string message = std::string( &buffer[0], length );

				AddressedUDPPacket UDPPacket;
				UDPPacket.isValid = true;
				UDPPacket.packet = UDPPacket::ToPacket( message.c_str(), (int)message.size() );
				UDPPacket.IPAddress = m_UDPSocket->GetLastReceiveAddress();

				m_readerQueue.push( UDPPacket );
			}
		}
	}
}


//When a message is put in queue, it will write it to send
void UDPGameConnection::WriterThread()
{
	while( !m_isQuitting )
	{
		if( m_UDPSocket && m_UDPSocket->IsSocketValid() )
		{
			std::string message = m_writerQueue.pop();
			if( message.size() != 0 )
			{
				UDPSocket::Buffer& buffer = m_UDPSocket->SendBuffer();
				memcpy( &buffer, message.c_str(), message.size() );
				//strcpy_s( &buffer[0], UDPSocket::BufferSize, message.c_str() );

				m_UDPSocket->Send( (int)message.size() );
			}

		}
	}
}