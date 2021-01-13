#ifndef DATARECEIVER_H
#define DATARECEIVER_H
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QtNetwork>

static const wchar_t commands[5][13] = { {26, '\0', 'c', 'o', 'u', 'r', 's', 'e', 'w', 'o', 'r', 'k', '\0'},
                                        {16, '\0', 'S', 't', 'a',  'r', 't', '\0' },
                                        {14, '\0', 'S', 't', 'o',  'p', '\0'},
                                        {16, '\0', 'B', 'r', 'e',  'a', 'k', '\0'},
                                        {16, '\0', 'R', 'e', 'a',  'd', 'y', '\0' } };

class DataReceiver : public QThread
{
    Q_OBJECT
public:
    DataReceiver();

    static bool isConnectedToServer;
    void writeToClient(char *data, qint64 len);
    static bool connectToServer();
    static bool disconnectClient();

protected:
    void run() override;

private:
    QMutex m_receiveMutex;
    QWaitCondition m_receiveCheck;
    bool exitThread{false};

    void wchToCh(char *wCharFormatBuf, qint64 *bufSize);
    std::stringstream analyzeData();

    char data[1000];
    char tempData[1000];

    static quint16 PORT;
    static QString SERVER_IP_ADDRESS;


signals:
    void receiveReadySignal(QString msg);
    void receiveSendSignal(QString msg);

public slots:
    void receiveStop();
    void receiveMore();
};

#endif // DATARECEIVER_H
