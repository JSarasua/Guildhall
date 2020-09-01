// Network.cpp : Defines the functions for the static library.
//

// #include "pch.h"
// #include "framework.h"
// 
// // TODO: This is an example of a library function
// void fnNetwork()
// {
// }

#include "Network.h"
#include <exception>
#include <strstream>

void Network::Initialize()
{
	// Initalize winsock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD( 2, 2 );
	int iResult = WSAStartup( wVersion, &wsaData );
	if( iResult != 0 )
	{
		std::ostrstream ostr;
		ostr << "Winsock startup failed, error = " << iResult << std::endl;
		throw std::runtime_error( ostr.str() );
	}

	try
	{

	}
	catch( const std::exception& )
	{

	}
}

void Network::Uninitialize()
{
	int iResult = WSACleanup();
	
	if( iResult == SOCKET_ERROR )
	{
		std::ostrstream ostr;
		ostr << "Winsock cleanup failed, error = " << WSAGetLastError() << std::endl;
		throw std::runtime_error( ostr.str() );
	}
}
