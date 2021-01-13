#include "KeyboardReceiver.h"
#include <iostream>
#include <mutex>

void KeyboardReceiver::checkKeyboard()
{
	while (Socket->checkingKeyboard.load())
	{
		std::cin >> command;
		if (command == "start")
		{
			Socket->sendCommand = 1;
			Socket->mCondVar.notify_one();
		}
		else if (command == "stop")
		{
			Socket->sendCommand = 2;
			Socket->mCondVar.notify_one();
		}
		else if (command == "break")
		{
			Socket->sendCommand = 3;
			Socket->mCondVar.notify_one();
		}
		else if (command == "connect" && !Socket->connectedToServer)
		{
			Socket->sendCommand = 5; 
			
		}
		else if (command == "ready") 
		{
			Socket->sendCommand = 4; 
			Socket->mCondVar.notify_one();
		}
		else if(command == "exit")
		{
			
			Socket->closeConnection(); 
			Socket->plantActive.store(false);
			Socket->sendingData.store(false);
			Socket->mCondVar.notify_one();
			Socket->receivingData.store(false);
			Socket->checkingKeyboard.store(false);

		}
		else
		{
			std::cerr << "Command not available..." << std::endl;
		}
	}

	std::cout << "Keyboard exited" << std::endl;
}