#ifndef BANDWIDTHTESTDIALOG_H
#define BANDWIDTHTESTDIALOG_H

#include <QDialog>
#include "diagnostics/BandwidthTester.h"

namespace Ui {
class BandwidthTestDialog;
}

/**
 * @brief Dialog for configuring bandwidth test parameters
 *
 * This dialog allows users to configure all parameters for a bandwidth test:
 * - Target IP address (read-only, pre-filled)
 * - Server port (default: 5201 for iperf3)
 * - Test duration (1-60 seconds)
 * - Protocol (TCP or UDP)
 * - Direction (Download or Upload)
 * - Packet size (1-1024 KB)
 *
 * The dialog provides sensible defaults and tooltips to guide users.
 */
class BandwidthTestDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Test configuration parameters
     */
    struct TestConfig {
        QString targetIp;                   ///< Target device IP address
        quint16 port;                       ///< Server port number
        int durationSeconds;                ///< Test duration in seconds
        BandwidthTester::Protocol protocol; ///< Protocol (TCP or UDP)
        BandwidthTester::Direction direction; ///< Direction (Download or Upload)
        int packetSizeKB;                   ///< Packet size in kilobytes
    };

    /**
     * @brief Constructs a bandwidth test configuration dialog
     * @param targetIp The IP address of the device to test
     * @param parent Parent widget
     */
    explicit BandwidthTestDialog(const QString& targetIp, QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~BandwidthTestDialog();

    /**
     * @brief Gets the configured test parameters
     * @return TestConfig structure with all parameters
     */
    TestConfig getConfig() const;

    /**
     * @brief Sets the port number
     * @param port Port number (1-65535)
     */
    void setPort(int port);

    /**
     * @brief Sets the test duration
     * @param seconds Duration in seconds (1-60)
     */
    void setDuration(int seconds);

    /**
     * @brief Sets the protocol
     * @param protocol TCP or UDP
     */
    void setProtocol(BandwidthTester::Protocol protocol);

    /**
     * @brief Sets the test direction
     * @param direction Download or Upload
     */
    void setDirection(BandwidthTester::Direction direction);

    /**
     * @brief Sets the packet size
     * @param sizeKB Packet size in kilobytes (1-1024)
     */
    void setPacketSize(int sizeKB);

private:
    Ui::BandwidthTestDialog *ui;
};

#endif // BANDWIDTHTESTDIALOG_H
