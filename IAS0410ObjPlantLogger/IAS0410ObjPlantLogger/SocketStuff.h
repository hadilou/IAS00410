#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <mutex>
#include <condition_variable>
#include <atomic>

constexpr auto PORT = "1234";
constexpr auto SERVER_IP_ADDRESS = "127.0.0.1";
constexpr auto RECV_BUF_LEN = 512;

class DataReceiverAndAnalyzer;
class CommandSender;
class KeyboardReceiver;

class SocketStuff
{
private:
	bool connectedToServer{ false };
	int sendCommand{ 0 };
	std::mutex mMutex;
	std::condition_variable mCondVar;

	char recvbuf[RECV_BUF_LEN];
	int iResult{ 0 };

	WSADATA wsaData;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	SOCKET ConnectSocket{ INVALID_SOCKET };

	std::atomic<bool> checkingKeyboard{ false };
	std::atomic<bool> receivingData{ false };
	std::atomic<bool> sendingData{ false };

	std::atomic<bool> plantActive{ true };

public:
	SocketStuff();
	bool connectToServer(void);
	bool closeConnection(void);

	bool isPlantActive() { return plantActive.load(); }


	
	friend DataReceiverAndAnalyzer;
	friend CommandSender;
	friend KeyboardReceiver;
};

