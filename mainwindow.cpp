#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBluetoothLocalDevice>
#include <QDebug>
#include <QLowEnergyController>

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    startDeviceDiscovery();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startDeviceDiscovery()
{
    localDevice = new QBluetoothLocalDevice;


    // Create a discovery agent and connect to its signals
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    discoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);

    connect(ui->scan, SIGNAL(clicked()), this, SLOT(startScan()));
    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));

    connect(discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo &)),
            this, SLOT(deviceDiscovered(const QBluetoothDeviceInfo &)));

    // Start a discovery
    //discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::ClassicMethod /*| QBluetoothDeviceDiscoveryAgent::LowEnergyMethod*/);

    connect(ui->list, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(itemActivated(QListWidgetItem*)));

    ui->pair->hide();
    ui->unpair->hide();

    connect(ui->pair, SIGNAL(clicked()), this, SLOT(requestPairing()));
    connect(ui->unpair, SIGNAL(clicked()), this, SLOT(requestUnPairing()));

    // add context menu for devices to be able to pair device
    ui->list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->list, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(displayPairingMenu(QPoint)));
    connect(localDevice, SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing))
        , this, SLOT(pairingDone(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));
    //...
}
#include <QList>
#include <QListWidgetItem>
#include <QTextBrowser>
#include <QMenu>
#include <QAction>
#include <QListWidgetItem>
#include <QColor>

void MainWindow::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    //qDebug() << "Found new device:" << device.name() << '(' << device.address().toString() << ')';
    ui->textBrowser->append(QString ("Found new device: %1 (%2) \n").arg(device.name()).arg(device.address().toString()));

    QString label = QString("%1 %2").arg(device.address().toString()).arg(device.name());
    QList<QListWidgetItem *> items = ui->list->findItems(label, Qt::MatchExactly);
    if (items.empty()) {
        QListWidgetItem *item = new QListWidgetItem(label);
        QBluetoothLocalDevice::Pairing pairingStatus = localDevice->pairingStatus(device.address());
        if (pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired )
            item->setForeground(QColor(Qt::green));
        else
            item->setForeground(QColor(Qt::black));

        ui->list->addItem(item);
    }
}

void MainWindow::startScan()
{
    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::ClassicMethod);
    ui->scan->setEnabled(false);
}

void MainWindow::scanFinished()
{
    ui->scan->setEnabled(true);
}

void MainWindow::displayPairingMenu(const QPoint &pos)
{
    if (ui->list->count() == 0)
        return;
    QMenu menu(this);
    QAction *pairAction = menu.addAction("Pair");
    QAction *removePairAction = menu.addAction("Remove Pairing");
    QAction *chosenAction = menu.exec(ui->list->viewport()->mapToGlobal(pos));
    QListWidgetItem *currentItem = ui->list->currentItem();

    QString text = currentItem->text();
    int index = text.indexOf(' ');
    if (index == -1)
        return;

    QBluetoothAddress address (text.left(index));
    if (chosenAction == pairAction) {
        localDevice->requestPairing(address, QBluetoothLocalDevice::Paired);
    } else if (chosenAction == removePairAction) {
        localDevice->requestPairing(address, QBluetoothLocalDevice::Unpaired);
    }
}

void MainWindow::pairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing)
{
    QList<QListWidgetItem *> items = ui->list->findItems(address.toString(), Qt::MatchContains);

    if (pairing == QBluetoothLocalDevice::Paired || pairing == QBluetoothLocalDevice::AuthorizedPaired ) {
        for (int var = 0; var < items.count(); ++var) {
            QListWidgetItem *item = items.at(var);
            item->setForeground(QColor(Qt::green));
        }
    } else {
        for (int var = 0; var < items.count(); ++var) {
            QListWidgetItem *item = items.at(var);
            item->setForeground(QColor(Qt::red));
        }
    }
}

void MainWindow::requestPairing()
{
    QListWidgetItem *currentItem = ui->list->currentItem();

    QString text = currentItem->text();
    int index = text.indexOf(' ');
    if (index == -1)
        return;

    QBluetoothAddress address (text.left(index));
    localDevice->requestPairing(address, QBluetoothLocalDevice::Paired);

}

void MainWindow::requestUnPairing()
{
    QListWidgetItem *currentItem = ui->list->currentItem();

    QString text = currentItem->text();
    int index = text.indexOf(' ');
    if (index == -1)
        return;

    QBluetoothAddress address (text.left(index));
    localDevice->requestPairing(address, QBluetoothLocalDevice::Unpaired);
}

void MainWindow::itemActivated(QListWidgetItem *item)
{

    QString text = item->text();

    int index = text.indexOf(' ');

    if (index == -1)
        return;

    QBluetoothAddress address(text.left(index));

    QBluetoothLocalDevice::Pairing pairingStatus = localDevice->pairingStatus(address);


    ui->unpair->setVisible(pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired);
    ui->pair->setVisible(pairingStatus != QBluetoothLocalDevice::Paired && pairingStatus != QBluetoothLocalDevice::AuthorizedPaired);
}
