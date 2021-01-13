#include "datareceiver.h"
#include <QDebug>
#include <sstream>
#include <iomanip>
#include <QDateTime>

QTcpSocket m_client;
bool DataReceiver::isConnectedToServer = false;

quint16 DataReceiver::PORT = 1234;
QString DataReceiver::SERVER_IP_ADDRESS = "127.0.0.1";

DataReceiver::DataReceiver()
{
    connect(&m_client, &QIODevice::readyRead, this, &DataReceiver::receiveMore);
}

void DataReceiver::run()
{
    forever
    {
        // wait until data has been processed and ready for more
        m_receiveMutex.lock();
        m_receiveCheck.wait(&m_receiveMutex); // waits until waked
        if(exitThread)
        {
            break;
        }

        qint64 incomingBytes = m_client.bytesAvailable();
        m_client.read(tempData, incomingBytes);

        memcpy(data, tempData, incomingBytes);
        wchToCh(tempData, &incomingBytes);

        std::stringstream ss;
        if(strcmp("Identify", tempData) && strcmp("Accepted", tempData))
        {
            ss = analyzeData();
        }
        else
        {
            ss << tempData;
        }

        // Emits data that was received
        emit receiveReadySignal(ss.str().c_str());

        m_receiveMutex.unlock();
    }
}

void DataReceiver::receiveStop()
{
    exitThread = true;
    m_receiveCheck.wakeOne();
}

void DataReceiver::receiveMore()
{
    m_receiveCheck.wakeOne();
}

void DataReceiver::wchToCh(char *wCharFormatBuf, qint64 *bufSize)
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

std::stringstream DataReceiver::analyzeData()
{
    std::stringstream ss;
    int len;
    memcpy_s(&len, sizeof(int), &data[0], sizeof(int));

    int numberOfChannels;
    memcpy_s(&numberOfChannels, sizeof(int), &data[4], sizeof(int));

    int numberOfPoints;
    memcpy_s(&numberOfPoints, sizeof(int), &data[8], sizeof(int));

    bool analyzing = true;
    char channelBuf[30];
    char lastPtn[30];
    int bufLocation = 12;

    QDateTime cdt = QDateTime::currentDateTime();

    ss << "Measurement results at " << cdt.toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str() << '\n';
    while (analyzing)
    {
        strcpy_s(channelBuf, sizeof(channelBuf),&data[bufLocation]);

        if (!strcmp(channelBuf, "Furnace"))
        {
            ss <<&data[bufLocation] << ":\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1;
            strcpy_s(lastPtn, sizeof(lastPtn), "Furnace");

        }
        else if (!strcmp(channelBuf, "Inside temperature"))
        {
            double insideTemperature;
            memcpy_s(&insideTemperature, sizeof(double),&data[bufLocation + static_cast<int>(strlen(channelBuf))+ 1], sizeof(double));
            ss <<&data[bufLocation] << " = ";
            ss << std::setprecision(1) << std::fixed << insideTemperature << "°C\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1 + sizeof(double);
            numberOfPoints -= 1;
        }
        else if (!strcmp(channelBuf, "Gas inlet flow"))
        {
            double gasInletFlow;
            memcpy_s(&gasInletFlow, sizeof(double),&data[bufLocation + static_cast<int>(strlen(channelBuf))+ 1], sizeof(double));
            ss <<&data[bufLocation] << " = ";
            ss << std::setprecision(3) << std::fixed << gasInletFlow << "m³/s\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1 + static_cast<int>(sizeof(double));
            numberOfPoints -= 1;
        }
        else if (!strcmp(channelBuf, "Gas outlet flow"))
        {
            double gasOuletFlow;
            memcpy_s(&gasOuletFlow, static_cast<int>(sizeof(double)),&data[bufLocation + static_cast<int>(strlen(channelBuf))+ 1], static_cast<int>(sizeof(double)));
            ss <<&data[bufLocation] << " = ";
            ss << std::setprecision(3) << std::fixed << gasOuletFlow << "m³/s\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1 + static_cast<int>(sizeof(double));
            numberOfPoints -= 1;
        }
        else if (!strcmp(channelBuf, "Mixer"))
        {
            ss <<&data[bufLocation] << ":\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1;
            strcpy_s(lastPtn, sizeof(lastPtn), "Mixer");
        }
        else if (!strcmp(channelBuf, "NO input flow"))
        {
            double noInputFlow;
            memcpy_s(&noInputFlow, static_cast<int>(sizeof(double)),&data[bufLocation + static_cast<int>(strlen(channelBuf))+ 1], static_cast<int>(sizeof(double)));
            ss <<&data[bufLocation] << " = ";
            ss << std::setprecision(3) << std::fixed << noInputFlow << "m³/s\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1 + static_cast<int>(sizeof(double));
            numberOfPoints -= 1;
        }
        else if (!strcmp(channelBuf, "O2 input flow"))
        {
            double o2InputFlow;
            memcpy_s(&o2InputFlow, static_cast<int>(sizeof(double)),&data[bufLocation + static_cast<int>(strlen(channelBuf))+ 1], static_cast<int>(sizeof(double)));
            ss <<&data[bufLocation] << " = ";
            ss << std::setprecision(3) << std::fixed << o2InputFlow << "m³/s\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1 + static_cast<int>(sizeof(double));
            numberOfPoints -= 1;
        }
        else if (!strcmp(channelBuf, "N2 input flow"))
        {
            double n2InputFlow;
            memcpy_s(&n2InputFlow, static_cast<int>(sizeof(double)),&data[bufLocation + static_cast<int>(strlen(channelBuf))+ 1], static_cast<int>(sizeof(double)));
            ss <<&data[bufLocation] << " = ";
            ss << std::setprecision(3) << std::fixed << n2InputFlow << "m³/s\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1 + static_cast<int>(sizeof(double));
            numberOfPoints -= 1;
        }
        else if (!strcmp(channelBuf, "Output flow"))
        {
            double outputFlow;
            memcpy_s(&outputFlow, static_cast<int>(sizeof(double)),&data[bufLocation + static_cast<int>(strlen(channelBuf))+ 1], static_cast<int>(sizeof(double)));
            ss <<&data[bufLocation] << " = ";
            ss << std::setprecision(3) << std::fixed << outputFlow << "m³/s\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1 + static_cast<int>(sizeof(double));
            numberOfPoints -= 1;
        }
        else if (!strcmp(channelBuf, "Bubbler"))
        {
            ss <<&data[bufLocation] << ":\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1;
            strcpy_s(lastPtn, sizeof(lastPtn), "Bubbler");
        }
        else if (!strcmp(channelBuf, "Temperature"))
        {
            double temperature;
            memcpy_s(&temperature, static_cast<int>(sizeof(double)),&data[bufLocation + static_cast<int>(strlen(channelBuf))+ 1], static_cast<int>(sizeof(double)));
            ss <<&data[bufLocation] << " = ";
            ss << std::setprecision(1) << std::fixed << temperature << "°C\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1 + static_cast<int>(sizeof(double));
            numberOfPoints -= 1;
        }
        else if (!strcmp(channelBuf, "Level"))
        {
            int level;
            memcpy_s(&level, sizeof(int),&data[bufLocation + static_cast<int>(strlen(channelBuf))+ 1], sizeof(int));
            ss <<&data[bufLocation] << " = ";
            ss << level << "%\n"; // TODO: c++ cast
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1 + sizeof(int);
            numberOfPoints -= 1;
        }
        else if (!strcmp(channelBuf, "Cooler"))
        {
            ss <<&data[bufLocation] << ":\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1;
            strcpy_s(lastPtn, sizeof(lastPtn), "Cooler");
        }
        else if (!strcmp(channelBuf, "Input flow"))
        {
            double inputFlow;
            memcpy_s(&inputFlow, static_cast<int>(sizeof(double)),&data[bufLocation + static_cast<int>(strlen(channelBuf)) + 1], static_cast<int>(sizeof(double)));
            ss <<&data[bufLocation] << " = ";
            ss << std::setprecision(3) << std::fixed << inputFlow << "m³/s\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1 + static_cast<int>(sizeof(double));
            numberOfPoints -= 1;
            char msg[30];
            strcpy_s(msg, sizeof(msg),&data[bufLocation]);
            qDebug() << data[bufLocation];
        }
        else if (!strcmp(channelBuf, "Input temperature"))
        {
            double inputTemperature;
            memcpy_s(&inputTemperature, static_cast<int>(sizeof(double)),&data[bufLocation + static_cast<int>(strlen(channelBuf))+ 1], static_cast<int>(sizeof(double)));
            ss <<&data[bufLocation] << " = ";
            ss << std::setprecision(1) << std::fixed << inputTemperature << "°C\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1 + static_cast<int>(sizeof(double));
            numberOfPoints -= 1;
        }
        else if (!strcmp(channelBuf, "Output temperature"))
        {
            double inputTemperature;
            memcpy_s(&inputTemperature, static_cast<int>(sizeof(double)),&data[bufLocation + static_cast<int>(strlen(channelBuf))+ 1], static_cast<int>(sizeof(double)));
            ss <<&data[bufLocation] << " = ";
            ss << std::setprecision(1) << std::fixed << inputTemperature << "°C\n";
            bufLocation = bufLocation + static_cast<int>(strlen(channelBuf))+ 1 + static_cast<int>(sizeof(double));
            numberOfPoints -= 1;
        }
        if (numberOfPoints == 0) //
        {
            memcpy_s(&numberOfPoints, sizeof(int),&data[bufLocation], sizeof(int)); //get number of point in next ch
            bufLocation += sizeof(int);
        }
        if (bufLocation >= len)
        {
            analyzing = false;
        }

    }
    return ss;
}

bool DataReceiver::connectToServer()
{
    m_client.connectToHost(SERVER_IP_ADDRESS, PORT, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    isConnectedToServer = true;
    return true;
}

bool DataReceiver::disconnectClient()
{
    m_client.disconnectFromHost();
    isConnectedToServer = false;
    return true;
}

void DataReceiver::writeToClient(char *data, qint64 len)
{
    m_client.write(data, len);
}
