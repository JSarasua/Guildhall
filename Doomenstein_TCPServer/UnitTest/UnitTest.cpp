#include "pch.h"
#include "Engine/Network/UDPSocket.hpp"
#include "CppUnitTest.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

#define  TEST_MODE

#pragma  comment( lib, "Ws2_32.lib" )

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		TEST_METHOD_INITIALIZE( InitUDPSocketTest )
		{
			WSADATA wsaData;
			WORD wVersion MAKEWORD( 2, 2 );
			int iResult = WSAStartup( wVersion, &wsaData );

			if( iResult != 0 )
			{
				std::cout << "Networking System: WSAStartup failed with " << WSAGetLastError() << std::endl;
			}
		}

		static void ReaderThread( UDPSocket& socket, std::string& messageToRead )
		{
			int length = socket.Receive();
			UDPSocket::Buffer& buffer = socket.ReceiveBuffer();
			buffer[length] = NULL;
			
			messageToRead = std::string(&buffer[0] + 4);
		}

		static void WriterThread( UDPSocket& socket, std::string const& messageToWrite )
		{
			UDPSocket::Buffer& buffer = socket.SendBuffer();

			TextMessage message;
			message.m_data = messageToWrite;
			std::string messageStr = message.ToString();
			memcpy( &buffer[0], messageStr.c_str(), (int)messageStr.size() + 4 );
			socket.Send( (int)messageStr.size() );
		}

		TEST_METHOD(UDPSocketTest)
		{
			Logger::WriteMessage("Starting UDP Socket Test");

			UDPSocket socket1 = UDPSocket( "127.0.0.1", 48001 );
			socket1.Bind( 48000 );

			UDPSocket socket2 = UDPSocket( "127.0.0.1", 48000 );
			socket2.Bind( 48001 );

			std::string m_messagesToSendToSocket1 = "Hello I'm Socket 1, sending to socket 2";
			std::string m_messagesToSendToSocket2 = "Hello I'm Socket 2 sending to Socket 1";
			std::string m_messagesToReceiveFromSocket1;
			std::string m_messagesToReceiveFromSocket2;
			
			std::thread readerOne( &UnitTest::ReaderThread, std::ref( socket1 ), std::ref( m_messagesToReceiveFromSocket2 ) );
 			std::thread readerTwo( &UnitTest::ReaderThread, std::ref(socket2), std::ref(m_messagesToReceiveFromSocket1) );
 			std::thread writerOne( &UnitTest::WriterThread, std::ref(socket1), std::cref(m_messagesToSendToSocket2) );
 			std::thread writerTwo( &UnitTest::WriterThread, std::ref(socket2), std::cref(m_messagesToSendToSocket1) );

			readerOne.join();
			readerTwo.join();
			writerOne.join();
			writerTwo.join();
			Assert::AreEqual( m_messagesToReceiveFromSocket1, m_messagesToSendToSocket2 );
			Assert::AreEqual( m_messagesToReceiveFromSocket2, m_messagesToSendToSocket1 );

			socket1.Close();
			socket2.Close();
		}

		TEST_METHOD_CLEANUP( DeinitUPDSocketTest )
		{

			int iResult = WSACleanup();
			if( iResult == SOCKET_ERROR )
			{
				std::cout << "Call to WSACleanp failed " << WSAGetLastError() << std::endl;
			}
		}
	};
}
