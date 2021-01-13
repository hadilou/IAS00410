#include "DataReceiverAndAnalyzer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Timestamp.h"
#include <iomanip>

void DataReceiverAndAnalyzer::receiveData()
{
	while (Socket->receivingData.load())
	{
		if (Socket->connectedToServer)
		{
			try
			{
				Socket->iResult = recv(Socket->ConnectSocket, Socket->recvbuf, RECV_BUF_LEN, 0);
			}
			catch (const std::exception& e)
			{
				std::cerr << (" RECVBIG recv() error %ld.\n", WSAGetLastError());
			}
			
			if (Socket->iResult > 0)
			{
				std::cout << "Received Toatal Bits No> " << Socket->iResult << std::endl;

				memcpy(recvbuf2, Socket->recvbuf, sizeof(char) * Socket->iResult);
				int bytesReceived = Socket->iResult;
				wchToCh(recvbuf2, &bytesReceived);
				analyzeData();
			}
			else if (Socket->iResult == 0)
			{
				Socket->closeConnection();
			}
			else 
			{
				if (Socket->connectedToServer) 
				{
					Socket->connectedToServer = false;
				}
				else 
				{

				}
				std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
			}
		}
		else if (Socket->sendCommand == 5)
		{
			if (Socket->connectToServer())
			{
				Socket->connectedToServer = true;
			}
			else
			{
				std::cerr << "Reconnection failed" << std::endl;
				Socket->sendCommand = -1; 
			}
			
		}
	}

	std::cout << "DataReceiverAndAnalyzer exited" << std::endl;
}

void DataReceiverAndAnalyzer::analyzeData()
{

	if (!strcmp(recvbuf2, "Identify"))
	{
		Socket->sendCommand = 0;
		Socket->mCondVar.notify_one();
	}
	else if (!strcmp(recvbuf2, "Accepted"))
	{
		Socket->connectedToServer = true;
	}
	else if (!strcmp(recvbuf2, "Not accepted"))
	{
		std::cerr << "Wrong password!" << std::endl;
	}
	else
	{
		

		std::stringstream ss;
		std::fstream file;
		file.open("DebugLog.txt", std::ios::app); 

		int len;
		memcpy_s(&len, sizeof(int), &Socket->recvbuf[0], sizeof(int));

		int numberOfChannels;
		memcpy_s(&numberOfChannels, sizeof(int), &Socket->recvbuf[4], sizeof(int));

		int numberOfPoints;
		memcpy_s(&numberOfPoints, sizeof(int), &Socket->recvbuf[8], sizeof(int));

		bool analyzing = true;
		char channelBuf[30];
		int bufLocation = 12;
		char lastPtn[30];

		ss << "Measurement results at " << Timestamp::getLocalTime();
		while (analyzing)
		{
			strcpy_s(channelBuf, sizeof(channelBuf), &Socket->recvbuf[bufLocation]);

			if (!strcmp(channelBuf, "Furnace"))
			{
				ss << &Socket->recvbuf[bufLocation] << ":\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1;
				strcpy_s(lastPtn, sizeof(lastPtn), "Furnace");

			}
			else if (!strcmp(channelBuf, "Inside temperature"))
			{
				double insideTemperature;
				memcpy_s(&insideTemperature, sizeof(double), &Socket->recvbuf[bufLocation + strlen(channelBuf) + 1], sizeof(double));
				ss << &Socket->recvbuf[bufLocation] << " = ";
				ss << std::setprecision(1) << std::fixed << insideTemperature << "°C\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1 + sizeof(double);
				numberOfPoints -= 1;
			}
			else if (!strcmp(channelBuf, "Gas inlet flow"))
			{
				double gasInletFlow;
				memcpy_s(&gasInletFlow, sizeof(double), &Socket->recvbuf[bufLocation + strlen(channelBuf) + 1], sizeof(double));
				ss << &Socket->recvbuf[bufLocation] << " = ";
				ss << std::setprecision(3) << std::fixed << gasInletFlow << "m³/s\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1 + sizeof(double);
				numberOfPoints -= 1;

			}
			else if (!strcmp(channelBuf, "Gas outlet flow")) 
			{
				double gasOuletFlow;
				memcpy_s(&gasOuletFlow, sizeof(double), &Socket->recvbuf[bufLocation + strlen(channelBuf) + 1], sizeof(double));
				ss << &Socket->recvbuf[bufLocation] << " = ";
				ss << std::setprecision(3) << std::fixed << gasOuletFlow << "m³/s\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1 + sizeof(double);
				numberOfPoints -= 1;

			}
			else if (!strcmp(channelBuf, "Mixer"))
			{
				ss << &Socket->recvbuf[bufLocation] << ":\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1;
				strcpy_s(lastPtn, sizeof(lastPtn), "Mixer");
			}
			else if (!strcmp(channelBuf, "NO input flow"))
			{
				double noInputFlow;
				memcpy_s(&noInputFlow, sizeof(double), &Socket->recvbuf[bufLocation + strlen(channelBuf) + 1], sizeof(double));
				ss << &Socket->recvbuf[bufLocation] << " = ";
				ss << std::setprecision(3) << std::fixed << noInputFlow << "m³/s\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1 + sizeof(double);
				numberOfPoints -= 1;

			}
			else if (!strcmp(channelBuf, "O2 input flow"))
			{
				double o2InputFlow;
				memcpy_s(&o2InputFlow, sizeof(double), &Socket->recvbuf[bufLocation + strlen(channelBuf) + 1], sizeof(double));
				ss << &Socket->recvbuf[bufLocation] << " = ";
				ss << std::setprecision(3) << std::fixed << o2InputFlow << "m³/s\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1 + sizeof(double);
				numberOfPoints -= 1;

			}
			else if (!strcmp(channelBuf, "N2 input flow"))
			{
				double n2InputFlow;
				memcpy_s(&n2InputFlow, sizeof(double), &Socket->recvbuf[bufLocation + strlen(channelBuf) + 1], sizeof(double));
				ss << &Socket->recvbuf[bufLocation] << " = ";
				ss << std::setprecision(3) << std::fixed << n2InputFlow << "m³/s\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1 + sizeof(double);
				numberOfPoints -= 1;

			}
			else if (!strcmp(channelBuf, "Output flow"))
			{
				double outputFlow;
				memcpy_s(&outputFlow, sizeof(double), &Socket->recvbuf[bufLocation + strlen(channelBuf) + 1], sizeof(double));
				ss << &Socket->recvbuf[bufLocation] << " = ";
				ss << std::setprecision(3) << std::fixed << outputFlow << "m³/s\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1 + sizeof(double);

				numberOfPoints -= 1;

			}
			else if (!strcmp(channelBuf, "Bubbler"))
			{
				ss << &Socket->recvbuf[bufLocation] << ":\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1;
				strcpy_s(lastPtn, sizeof(lastPtn), "Bubbler");
			}
			else if (!strcmp(channelBuf, "Temperature"))
			{
				double temperature;
				memcpy_s(&temperature, sizeof(double), &Socket->recvbuf[bufLocation + strlen(channelBuf) + 1], sizeof(double));
				ss << &Socket->recvbuf[bufLocation] << " = ";
				ss << std::setprecision(1) << std::fixed << temperature << "°C\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1 + sizeof(double);
				numberOfPoints -= 1;

			}
			else if (!strcmp(channelBuf, "Level"))
			{
				int level;
				memcpy_s(&level, sizeof(int), &Socket->recvbuf[bufLocation + strlen(channelBuf) + 1], sizeof(int));
				ss << &Socket->recvbuf[bufLocation] << " = ";
				ss << level << "%\n"; 
				bufLocation = bufLocation + strlen(channelBuf) + 1 + sizeof(int);
				numberOfPoints -= 1;

			}
			else if (!strcmp(channelBuf, "Cooler"))
			{
				ss << &Socket->recvbuf[bufLocation] << ":\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1;
				strcpy_s(lastPtn, sizeof(lastPtn), "Cooler");
			}
			else if (!strcmp(channelBuf, "Input flow"))
			{
				double inputFlow;
				memcpy_s(&inputFlow, sizeof(double), &Socket->recvbuf[bufLocation + strlen(channelBuf) + 1], sizeof(double));
				ss << &Socket->recvbuf[bufLocation] << " = ";
				ss << std::setprecision(3) << std::fixed << inputFlow << "m³/s\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1 + sizeof(double);
				numberOfPoints -= 1;

			}
			else if (!strcmp(channelBuf, "Input temperature"))
			{
				double inputTemperature;
				memcpy_s(&inputTemperature, sizeof(double), &Socket->recvbuf[bufLocation + strlen(channelBuf) + 1], sizeof(double));
				ss << &Socket->recvbuf[bufLocation] << " = ";
				ss << std::setprecision(1) << std::fixed << inputTemperature << "°C\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1 + sizeof(double);
				numberOfPoints -= 1;

			}
			else if (!strcmp(channelBuf, "Output temperature"))
			{
				double inputTemperature;
				memcpy_s(&inputTemperature, sizeof(double), &Socket->recvbuf[bufLocation + strlen(channelBuf) + 1], sizeof(double));
				ss << &Socket->recvbuf[bufLocation] << " = ";
				ss << std::setprecision(1) << std::fixed << inputTemperature << "°C\n";
				bufLocation = bufLocation + strlen(channelBuf) + 1 + sizeof(double);
				numberOfPoints -= 1;

			}
			if (numberOfPoints == 0) 
			{
				
				memcpy_s(&numberOfPoints, sizeof(int), &Socket->recvbuf[bufLocation], sizeof(int)); //get number of point in next ch
				bufLocation += sizeof(int);
			
			}
			if (bufLocation >= len)
			{
				analyzing = false;
			}
			
		}
		std::cout << ss.str() << std::endl;
		file << ss.str() << '\n';
		file.close();

		Socket->sendCommand = 4; 
		Socket->mCondVar.notify_one();
	}

}

void wchToCh(char* wCharFormatBuf, int* bufSize)
{
	int j = 0;
	for (int i = 3; i < *bufSize; ++i)
	{
		if (wCharFormatBuf[i] != '\0')
		{
			wCharFormatBuf[j++] = wCharFormatBuf[i];
		}
	}
	wCharFormatBuf[j++] = '\0';
	*bufSize = j;
}