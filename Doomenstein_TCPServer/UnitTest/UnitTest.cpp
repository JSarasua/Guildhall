#include "pch.h"
#include "Engine/Network/UDPSocket.hpp"
#include "CppUnitTest.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

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

		TEST_METHOD(UDPSocketTest)
		{
			Logger::WriteMessage("Starting UDP Socket Test");

			UDPSocket socket1 = UDPSocket("127.0.0.1", 48000);
			socket1.Bind(48001);

			UDPSocket socket2 = UDPSocket("127.0.0.1", 48001);
			socket2.Bind( 48000 );
			Assert::AreEqual( true, false );
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
