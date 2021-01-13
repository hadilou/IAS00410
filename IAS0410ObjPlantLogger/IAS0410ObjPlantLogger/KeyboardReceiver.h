#pragma once
#include <string>
#include "SocketStuff.h"
class KeyboardReceiver 
{
private:
	std::string command;
	SocketStuff* Socket{ nullptr };
public:
	KeyboardReceiver(SocketStuff* s) : Socket(s) {};

	void checkKeyboard();

	~KeyboardReceiver() { Socket = nullptr; }
};

