#pragma once
#include "SocketStuff.h"
#include <iostream>


static const wchar_t commands[5][13] = { {26, '\0', 'c', 'o', 'u', 'r', 's', 'e', 'w', 'o', 'r', 'k', '\0'},
										{16, '\0', 'S', 't', 'a',  'r', 't', '\0' },
										{14, '\0', 'S', 't', 'o',  'p', '\0'},
										{16, '\0', 'B', 'r', 'e',  'a', 'k', '\0'},
										{16, '\0', 'R', 'e', 'a',  'd', 'y', '\0' } };

class CommandSender 
{
private:
	SocketStuff* Socket{ nullptr };
public:
	CommandSender(SocketStuff* s) :Socket(s) {};

	void sendData();

	~CommandSender()
	{
		Socket = nullptr;
	}
};

