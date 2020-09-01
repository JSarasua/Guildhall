// Client

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <array>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

int main()
{
    // Initalize winsock
    WSADATA wsaData;
    WORD wVersion = MAKEWORD(2, 2);
    int iResult = WSAStartup(wVersion, &wsaData);
    if (iResult != 0)
    {
        std::cerr << "Winsock startup failed" << std::endl;
        return 1;
    }

    // Open the client socket
    struct addrinfo  addrHintsIn;
    struct addrinfo* pAddrOut = NULL;

    ZeroMemory(&addrHintsIn, sizeof(addrHintsIn));
    addrHintsIn.ai_family = AF_INET;
    addrHintsIn.ai_socktype = SOCK_STREAM;
    addrHintsIn.ai_protocol = IPPROTO_TCP;

    // Get the Server IP address and port
    std::string hostname("MathTixWinDev");
    std::string serverPort("48000");
    iResult = getaddrinfo(hostname.c_str(), serverPort.c_str(), &addrHintsIn, &pAddrOut);
    if (iResult != 0)
    {
        std::cerr << "Call to getaddrinfo() failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // Create the socket
    SOCKET serverSocket = INVALID_SOCKET;
    serverSocket = socket(pAddrOut->ai_family, pAddrOut->ai_socktype, pAddrOut->ai_protocol);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(pAddrOut);
        WSACleanup();
        return 1;
    }

    // Connect to the server
    iResult = connect(serverSocket, pAddrOut->ai_addr, (int)pAddrOut->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }
    freeaddrinfo(pAddrOut);

    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Server connection failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // Send a message to the server
    std::string messageToServer("Send me some data");
    iResult = send(serverSocket, messageToServer.c_str(), static_cast<int>(messageToServer.length()), 0);
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "Send message to server failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Shudown the sockent for sending
    iResult = shutdown(serverSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "Server socket shutdown failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    // Receive messages from the server
    std::array<char, 128> receiveBuffer;
    do {
        iResult = recv(serverSocket, &receiveBuffer[0], /*static_cast<int>(receiveBuffer.size())*/ 64, 0);
        if (iResult > 0)
        {
            receiveBuffer[iResult] = NULL;
            std::cout << std::string(&receiveBuffer[0]) << std::endl;
        }
        else if (iResult == 0)
        {
            std::cerr << "Severver connection closed" << std::endl;
        }
        else
        {
            std::cerr << "Recieve from server failed: " << WSAGetLastError() << std::endl;
        }

    } while (iResult > 0);

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
