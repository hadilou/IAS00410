#pragma once
#include "SocketStuff.h"

void wchToCh(char* wCharFormatBuf, int* bufSize);

class DataReceiverAndAnalyzer 
{
private:
	SocketStuff* Socket{ nullptr };
	char recvbuf2[RECV_BUF_LEN];
public:
	DataReceiverAndAnalyzer(SocketStuff *s):Socket(s) {};

	void receiveData();
	void analyzeData();

	void setSocket(SocketStuff* s)
	{
		Socket = s;
	}

	~DataReceiverAndAnalyzer()
	{
		Socket = nullptr;
	}
};

