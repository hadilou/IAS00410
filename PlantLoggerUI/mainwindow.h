#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "datareceiver.h"
#include <QFile>
#include <QTextStream>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void on_exitButton_clicked();
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void on_breakButton_clicked();
    void on_openButton_clicked();
    void on_closeButton_clicked();

    void startReceiverThread();
    void stopReceiverThread();
    void destroyReceiverThread();
    void showResults(QString);

private:
    Ui::MainWindow *ui;
    DataReceiver *workerThreadReceiver = nullptr;
    QFile file;
    QTextStream out;

    bool isOutputFileOpened{false};

public:

private:
    void HandleSocketConnectionSuccess();
    void HandleSocketConnectionFailure();
    void HandleSocketDisconnectionSuccess();

signals:
    void stopSignal();

};
#endif // MAINWINDOW_H
