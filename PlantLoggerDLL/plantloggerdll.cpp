#include "plantloggerdll.h"
#include <QIODevice>
#include <QAbstractSocket>

static quint16 PORT = 1234;
static QString SERVER_IP_ADDRESS = "127.0.0.1";

PlantLoggerDLL::PlantLoggerDLL()
{
    client = new QTcpSocket;
}

PlantLoggerDLL::~PlantLoggerDLL()
{
    delete [] client;
}

bool PlantLoggerDLL::connectToServer()
{
    if(!isConnectedToServer)
    {
        client->connectToHost(SERVER_IP_ADDRESS, PORT, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
        return true;
    }
    return false;
}

bool PlantLoggerDLL::disconnectClient()
{
    if(isConnectedToServer)
    {
        client->disconnectFromHost();
        return true;
    }
    return false;
}

void PlantLoggerDLL::wchToCh(char *wCharFormatBuf, qint64 *bufSize)
{
    qint64 j = 0;
    for (qint64 i = 3; i < *bufSize; ++i)
    {
        if (wCharFormatBuf[i] != '\0')
        {
            wCharFormatBuf[j++] = wCharFormatBuf[i];
        }
    }
    wCharFormatBuf[j++] = '\0';
    *bufSize = j;
}

void PlantLoggerDLL::writeToClient(char *data, qint64 len)
{
    client->write(data, len);
}
