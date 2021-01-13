#include "SocketStuff.h"
#include <iostream>

SocketStuff::SocketStuff() {}

bool SocketStuff::connectToServer()
{
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); 
	if (iResult != 0)
	{
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(SERVER_IP_ADDRESS, PORT, &hints, &result);
	if (iResult != 0)
	{
		WSACleanup();
		return false;
	}

	ConnectSocket = INVALID_SOCKET;

	ptr = result;

	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET)
	{
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}


	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return false;
	}
	connectedToServer = true;

	checkingKeyboard.store(true);
	receivingData.store(true);
	sendingData.store(true);

	return true;
}

bool SocketStuff::closeConnection()
{
	if (connectedToServer)
	{
		std::cerr << "Connection closed!" << std::endl;

		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			std::cerr << "shutdown failed: " << WSAGetLastError() << std::endl;
		}
		closesocket(ConnectSocket);
		WSACleanup();

		connectedToServer = false;
	}

	return true;
}