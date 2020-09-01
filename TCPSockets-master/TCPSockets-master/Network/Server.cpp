// Server

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>
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

    // Resolve the port locally
    struct addrinfo  addrHintsIn;
    struct addrinfo* pAddrOut = NULL;

    ZeroMemory(&addrHintsIn, sizeof(addrHintsIn));
    addrHintsIn.ai_family = AF_INET;
    addrHintsIn.ai_socktype = SOCK_STREAM;
    addrHintsIn.ai_protocol = IPPROTO_TCP;
    addrHintsIn.ai_flags = AI_PASSIVE;

    // Get the Server IP address and port
    std::string serverPort("48000");
    iResult = getaddrinfo(NULL, serverPort.c_str(), &addrHintsIn, &pAddrOut);
    if (iResult != 0)
    {
        std::cerr << "Call to getaddrinfo() failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // Create a socket to listen for connectons.
    SOCKET listenSocket = INVALID_SOCKET;
    listenSocket = socket(pAddrOut->ai_family, pAddrOut->ai_socktype, pAddrOut->ai_protocol);
    if (listenSocket == INVALID_SOCKET)
    {
        std::cerr << "Listen socket creation error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(pAddrOut);
        WSACleanup();
        return 1;
    }

    // Bind the listening socket
    iResult = bind(listenSocket, pAddrOut->ai_addr, (int)pAddrOut->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "Sever socket bind failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(pAddrOut);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(pAddrOut);

    // Start listening for client connections.
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "Listen on server socket failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Server is listening for client connections" << std::endl;

    // Accept client connections
    SOCKET clientSocket = INVALID_SOCKET;
    clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET)
    {
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    sockaddr clientAddr;
    int addrSize = sizeof(clientAddr);
    iResult = getpeername(clientSocket, &clientAddr, &addrSize);
    if (iResult == 0)
    {
        std::array<char, 64> addressStr;
        DWORD outLen = 64;
        iResult = WSAAddressToStringA(&clientAddr, addrSize, NULL, &addressStr[0], &outLen);
        if (iResult == 0)
        {
            addressStr[outLen] = NULL;
            std::cout << "Client connected from address: "
                << std::string(&addressStr[0]) << std::endl;
        }
    }

    std::string quote("Four score and seven years ago our fathers brought forth on this continent, "
        "a new nation, conceived in Liberty, and dedicated to the proposition that all men are created equal. "
        "Now we are engaged in a great civil war, testing whether that nation, or any nation so conceived and "
        "so dedicated, can long endure. We are met on a great battle-field of that war. We have come to "
        "dedicate a portion of that field, as a final resting place for those who here gave their lives that "
        "that nation might live. It is altogether fitting and proper that we should do this. But, in a larger "
        "sense, we can not dedicate -- we can not consecrate -- we can not hallow -- this ground. The brave "
        "men, living and dead, who struggled here, have consecrated it, far above our poor power to add or "
        "detract. The world will little note, nor long remember what we say here, but it can never forget what "
        "they did here. It is for us the living, rather, to be dedicated here to the unfinished work which they "
        "who fought here have thus far so nobly advanced. It is rather for us to be here dedicated to the great "
        "task remaining before us -- that from these honored dead we take increased devotion to that cause for "
        "which they gave the last full measure of devotion -- that we here highly resolve that these dead shall "
        "not have died in vain -- that this nation, under God, shall have a new birth of freedom -- and that "
        "government of the people, by the people, for the people, shall not perish from the earth."
        " Abraham Lincoln");

    // Read data from the client
    int iSendResult;
    std::array<char, 512> receiveBuffer;
    iResult = recv(clientSocket, &(receiveBuffer[0]), static_cast<int>(receiveBuffer.size()), 0);
    if (iResult < receiveBuffer.size())
    {
        receiveBuffer[iResult] = NULL;
        std::cout << "Received message from client!" << std::endl
            << "\tMessage length: " << iResult << std::endl
            << "\tMessage: " << std::string(&receiveBuffer[0]) << std::endl;

        iSendResult = send(clientSocket, &quote[0], static_cast<int>(quote.length()), 0);
        if (iSendResult == SOCKET_ERROR)
        {
            std::cerr << "Send to clent failed: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
    }
    else if (iResult == 0)
    {
        std::cout << "Connection is closing" << std::endl;
    }
    else
    {
        std::cerr << "Receive from client failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return 1;
    }

    iResult = shutdown(clientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "Clent socket shutdown failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
