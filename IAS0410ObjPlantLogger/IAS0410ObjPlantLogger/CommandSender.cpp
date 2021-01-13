#include "CommandSender.h"
#include <functional>

void CommandSender::sendData()
{
	while (Socket->sendingData.load())
	{
		std::unique_lock<std::mutex> mlock(Socket->mMutex);
		Socket->mCondVar.wait(mlock); 
		if (Socket->connectedToServer)
		{
			Socket->iResult = send(Socket->ConnectSocket, (char*)commands[Socket->sendCommand], commands[Socket->sendCommand][0], 0);
			if (Socket->iResult == SOCKET_ERROR)
			{
				std::cerr << "Send op failed: " << WSAGetLastError() << std::endl;
				closesocket(Socket->ConnectSocket);
				WSACleanup();
				exit(-1);
			}
		}
	}

	std::cout << "sender exited" << std::endl;
}