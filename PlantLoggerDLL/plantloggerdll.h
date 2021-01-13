#ifndef PLANTLOGGERDLL_H
#define PLANTLOGGERDLL_H

#include "PlantLoggerDLL_global.h"
#include <QDebug>
#include <QtNetwork>

class PLANTLOGGERDLL_EXPORT PlantLoggerDLL
{
public:
    PlantLoggerDLL();
    ~PlantLoggerDLL();

    QTcpSocket *client;

    bool connectToServer();

    bool disconnectClient();

    bool isConnectedToServer{false};

    void wchToCh(char* wCharFormatBuf, qint64* bufSize);

    char data[256];

    void writeToClient(char *data, qint64 len);

};

#endif // PLANTLOGGERDLL_H
