#ifndef IPSCANNER_H
#define IPSCANNER_H

#include <QObject>
#include <QThreadPool>
#include <QAtomicInt>
#include "models/Device.h"
#include "interfaces/IScanStrategy.h"

class IpScanner : public QObject
{
    Q_OBJECT

public:
    explicit IpScanner(QObject *parent = nullptr);
    ~IpScanner();

    void setScanStrategy(IScanStrategy* strategy);
    void startScan(const QString& cidr);
    void stopScan();

    bool isScanning() const;
    int getProgress() const;
    int getTotalHosts() const;

signals:
    void deviceDiscovered(const Device& device);
    void scanProgress(int current, int total);
    void scanStarted(int totalHosts);
    void scanFinished(int devicesFound);
    void scanError(const QString& error);

private slots:
    void onHostScanned(const Device& device);
    void onScanComplete();

private:
    IScanStrategy* m_strategy;
    QThreadPool* m_threadPool;
    QAtomicInt m_isScanning;
    QAtomicInt m_scannedCount;
    int m_totalHosts;
    int m_devicesFound;

    void resetCounters();
};

#endif // IPSCANNER_H
