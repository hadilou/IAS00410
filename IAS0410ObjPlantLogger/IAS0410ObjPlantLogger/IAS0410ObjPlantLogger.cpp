#include <iostream>
#include <stdio.h>
#include <fstream>
#include <inttypes.h>
#include <sstream>
#include <thread>
#include "CommandSender.h"
#include "DataReceiverAndAnalyzer.h"
#include "KeyboardReceiver.h"
#include "SocketStuff.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#pragma warning(disable:4996)
UINT WINAPI codepage = GetConsoleOutputCP();


int main(int argc, char* argv[])
{
	SetConsoleOutputCP(1252);

	SocketStuff mySock;
	KeyboardReceiver Keyboard(&mySock);
	DataReceiverAndAnalyzer receiver(&mySock);
	CommandSender sender(&mySock);
	
	if (!mySock.connectToServer()) 
	{
		std::cout << "Failed to connect  server." << std::endl;
		return -1;
	}

	std::thread keyboard(&KeyboardReceiver::checkKeyboard, &Keyboard);
	std::thread receiveData(&DataReceiverAndAnalyzer::receiveData, &receiver);
	std::thread sendData(&CommandSender::sendData, &sender);

	while (mySock.isPlantActive())
	{
	}

	keyboard.join();
	receiveData.join();
	sendData.join();

	return 0;
}