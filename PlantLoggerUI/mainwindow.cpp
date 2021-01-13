#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "datareceiver.h"
#include <QFileDialog>
#include <QMessageBox>

extern QTcpSocket m_client;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // If DLL client is connected then mainwindow HandleSocketConnectionSuccess is executed
    connect(&m_client, &QAbstractSocket::connected, this, &MainWindow::HandleSocketConnectionSuccess);

    // If DLL client emits SockerError while trying to connect then HandleSocketConnectionFailure is executed
    connect(&m_client,(&QAbstractSocket::error), this, &MainWindow::HandleSocketConnectionFailure);

    // If DLL client is disconnected then HandleSocketDisconnectionSuccess is executed
    connect(&m_client, &QAbstractSocket::disconnected, this, &MainWindow::HandleSocketDisconnectionSuccess);
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*  *   *   *   *   *   *   *   *   *
 *  *   *   *   *   *   *   *   *   *
 *  *   *   *   *   *   *   *   *   *
 *       ON BUTTON CLICKED          *
 *  *   *   *   *   *   *   *   *   *
 *  *   *   *   *   *   *   *   *   *
 *  *   *   *   *   *   *   *   *   */


void MainWindow::on_startButton_clicked()
{
    if(isOutputFileOpened == false)
    {
        QMessageBox msgBox;
        msgBox.setText("You have to open the output file!");
        msgBox.exec();
    }
    else
    {
        ui->startButton->setEnabled(false);
        ui->breakButton->setEnabled(true);

        workerThreadReceiver->writeToClient((char*)commands[1], commands[1][0]);
    }
}

void MainWindow::on_breakButton_clicked()
{
    ui->startButton->setEnabled(true);
    ui->breakButton->setEnabled(false);

    workerThreadReceiver->writeToClient((char*)commands[3], commands[3][0]);
}

void MainWindow::on_exitButton_clicked()
{
    if(file.isOpen())
    {
        file.close();
    }

    // Stop receiver thread
    stopReceiverThread();

    // disconnectClient already checks if we are connected or not
    DataReceiver::disconnectClient();

    // Close QT window
    QApplication::exit();
}

void MainWindow::on_connectButton_clicked()
{
    if(DataReceiver::isConnectedToServer == true)
    {
        ui->logBook->append("Already connected.");
    }
    else
    {
        ui->connectButton->setDisabled(true);
        ui->logBook->append("Connecting...");
        DataReceiver::connectToServer();
    }
}

void MainWindow::on_disconnectButton_clicked()
{
    stopReceiverThread();

    DataReceiver::disconnectClient();
}

void MainWindow::on_openButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a output file", "C:/Users/Jonathan/Documents/Visual Studio Code/IAS0410/PlantLoggerUI");

    file.setFileName(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        ui->logBook->append("Error opening file.");
    }
    else
    {
        ui->closeButton->setEnabled(true);
        ui->openButton->setEnabled(false);
        ui->logBook->append("Output file open.");
        out.setDevice(&file);
        isOutputFileOpened = true;
    }
}

void MainWindow::on_closeButton_clicked()
{
    if(file.isOpen())
    {
            file.close();
            ui->closeButton->setEnabled(false);
            ui->openButton->setEnabled(true);
            isOutputFileOpened = false;
    }
    ui->logBook->append("Output file closed");
}


/*  *   *   *   *   *   *   *   *   *
 *  *   *   *   *   *   *   *   *   *
 *  *   *   *   *   *   *   *   *   *
 *       Handle connection/         *
 *           disconnection          *
 *  *   *   *   *   *   *   *   *   *
 *  *   *   *   *   *   *   *   *   *
 *  *   *   *   *   *   *   *   *   */


void MainWindow::HandleSocketConnectionSuccess()
{
    ui->logBook->append("Connected.");
    ui->connectButton->setEnabled(false);
    ui->disconnectButton->setEnabled(true);
    ui->startButton->setEnabled(false);

    startReceiverThread();
}

void MainWindow::HandleSocketConnectionFailure()
{
    ui->logBook->append("Connecting failed.");
    ui->connectButton->setEnabled(false);
}

void MainWindow::HandleSocketDisconnectionSuccess()
{
    ui->logBook->append("Disconnected.");
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    DataReceiver::isConnectedToServer = false;
}


/*  *   *   *   *   *   *   *   *   *
 *  *   *   *   *   *   *   *   *   *
 *  *   *   *   *   *   *   *   *   *
 *        Thread functions          *
 *  *   *   *   *   *   *   *   *   *
 *  *   *   *   *   *   *   *   *   *
 *  *   *   *   *   *   *   *   *   */


void MainWindow::startReceiverThread()
{
    workerThreadReceiver = new DataReceiver();

    connect(workerThreadReceiver, &DataReceiver::receiveReadySignal, this, &MainWindow::showResults, Qt::QueuedConnection);
    connect(workerThreadReceiver, &QThread::finished, workerThreadReceiver, &QObject::deleteLater);
    connect(workerThreadReceiver, &DataReceiver::finished, this, &MainWindow::destroyReceiverThread);

    connect(this, &MainWindow::stopSignal, workerThreadReceiver, &DataReceiver::receiveStop, Qt::DirectConnection);

    workerThreadReceiver->start();

    qDebug() << "Receiver thread started";
}

void MainWindow::stopReceiverThread()
{
    emit stopSignal();
}

void MainWindow::destroyReceiverThread()
{
    qDebug() << "Receicer destroyed";
    ui->startButton->setEnabled(false);
    ui->breakButton->setEnabled(false);
}

void MainWindow::showResults(QString msg)
{
    if(msg == "Identify")
    {
        workerThreadReceiver->writeToClient((char*)(commands[0]), commands[0][0]);
    }
    else if(msg == "Accepted")
    {
        ui->startButton->setEnabled(true);
    }
    else
    {
        ui->logBook->append(msg);
        out << msg << '\n';
        workerThreadReceiver->writeToClient((char*)commands[4], commands[4][0]);
    }
}
