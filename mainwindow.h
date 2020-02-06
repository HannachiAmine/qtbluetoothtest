#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qbluetoothlocaldevice.h>

QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceDiscoveryAgent)
QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceInfo)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void startDeviceDiscovery();

    // In your local slot, read information about the found devices
    void deviceDiscovered(const QBluetoothDeviceInfo &device);

    void startScan();
    void scanFinished();

    void displayPairingMenu(const QPoint &pos);

    void pairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);

    void requestPairing();
    void requestUnPairing();

    void itemActivated(QListWidgetItem *item);

private:

    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothLocalDevice *localDevice;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
