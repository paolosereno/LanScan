#ifndef PORTSCANNER_H
#define PORTSCANNER_H

#include <QObject>
#include <QList>
#include <QString>

// Forward declarations
class TcpSocketManager;
class PortServiceMapper;

/**
 * @brief TCP port scanning service
 *
 * Provides flexible port scanning with support for quick scans
 * (common ports), full scans (1-65535), and custom port ranges.
 * Uses thread pool for parallel scanning operations.
 */
class PortScanner : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Port scan types
     */
    enum ScanType {
        QUICK_SCAN,    ///< Scan common ports only (~20 ports)
        FULL_SCAN,     ///< Scan all ports 1-65535
        CUSTOM_SCAN    ///< Scan user-defined port list/range
    };

    /**
     * @brief Result of a single port scan
     */
    struct PortScanResult {
        QString host;           ///< Target host
        int port;              ///< Port number
        QString state;         ///< Port state: "open", "closed", "filtered"
        QString service;       ///< Service name (e.g., "HTTP", "SSH")
        double responseTime;   ///< Response time in milliseconds

        PortScanResult()
            : port(0), state("unknown"), responseTime(0.0) {}
    };

    explicit PortScanner(QObject* parent = nullptr);
    ~PortScanner();

    /**
     * @brief Scan ports with predefined scan type
     * @param host Target IP address or hostname
     * @param type Type of scan (QUICK_SCAN, FULL_SCAN, CUSTOM_SCAN)
     */
    void scanPorts(const QString& host, ScanType type);

    /**
     * @brief Scan specific list of ports
     * @param host Target IP address or hostname
     * @param ports List of port numbers to scan
     */
    void scanPorts(const QString& host, const QList<int>& ports);

    /**
     * @brief Scan a range of ports
     * @param host Target IP address or hostname
     * @param startPort Starting port number (inclusive)
     * @param endPort Ending port number (inclusive)
     */
    void scanPortRange(const QString& host, int startPort, int endPort);

    /**
     * @brief Cancel ongoing scan operation
     */
    void cancelScan();

    /**
     * @brief Check if scan is currently running
     * @return True if scan is in progress
     */
    bool isScanning() const;

signals:
    /**
     * @brief Emitted when an open port is found
     * @param result Port scan result
     */
    void portFound(const PortScanResult& result);

    /**
     * @brief Emitted periodically to report scan progress
     * @param current Number of ports scanned
     * @param total Total number of ports to scan
     */
    void scanProgress(int current, int total);

    /**
     * @brief Emitted when scan completes
     * @param results List of all open ports found
     */
    void scanCompleted(const QList<PortScanResult>& results);

    /**
     * @brief Emitted when an error occurs
     * @param error Error message
     */
    void errorOccurred(const QString& error);

private:
    TcpSocketManager* socketManager;
    PortServiceMapper* serviceMapper;

    QString currentHost;
    QList<PortScanResult> scanResults;
    int totalPorts;
    int scannedPorts;
    bool scanning;

    /**
     * @brief Get list of common ports for quick scan
     * @return List of common port numbers
     */
    QList<int> getCommonPorts();

    /**
     * @brief Scan a single port
     * @param host Target host
     * @param port Port number
     * @param timeout Connection timeout in milliseconds
     * @return Port scan result
     */
    PortScanResult scanSinglePort(const QString& host, int port, int timeout = 1000);

    /**
     * @brief Execute the actual port scan
     * @param host Target host
     * @param ports List of ports to scan
     */
    void executeScan(const QString& host, const QList<int>& ports);

    /**
     * @brief Update scan progress
     */
    void updateProgress();
};

#endif // PORTSCANNER_H
