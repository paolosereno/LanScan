#ifndef WAKEONLANSERVICE_H
#define WAKEONLANSERVICE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QUdpSocket>
#include <QRegularExpression>

class Device;

/**
 * @brief Magic packet builder for Wake-on-LAN
 *
 * Creates the magic packet structure: 6 bytes of 0xFF followed by
 * 16 repetitions of the target MAC address (total 102 bytes)
 */
class WakeOnLanPacket {
public:
    WakeOnLanPacket() = default;

    /**
     * @brief Create a magic packet for the given MAC address
     * @param macAddress MAC address in format XX:XX:XX:XX:XX:XX or XX-XX-XX-XX-XX-XX
     * @return Magic packet as QByteArray, or empty if MAC is invalid
     */
    static QByteArray createMagicPacket(const QString& macAddress);

    /**
     * @brief Validate MAC address format
     * @param macAddress MAC address string to validate
     * @return True if valid format (XX:XX:XX:XX:XX:XX or XX-XX-XX-XX-XX-XX)
     */
    static bool isValidMacAddress(const QString& macAddress);

private:
    /**
     * @brief Parse MAC address string to byte array
     * @param macAddress MAC address string (with or without separators)
     * @return MAC address as 6-byte array
     */
    static QByteArray parseMacAddress(const QString& macAddress);
};

/**
 * @brief Wake-on-LAN service for sending magic packets
 *
 * Sends UDP broadcast packets on port 9 (standard WoL port) to wake up
 * devices that support Wake-on-LAN functionality.
 */
class WakeOnLanService : public QObject {
    Q_OBJECT

public:
    explicit WakeOnLanService(QObject* parent = nullptr);
    ~WakeOnLanService();

    /**
     * @brief Send Wake-on-LAN packet to a specific MAC address
     * @param macAddress Target MAC address
     * @param broadcastAddress Broadcast address (default: 255.255.255.255)
     */
    void sendWakeOnLan(const QString& macAddress,
                      const QString& broadcastAddress = "255.255.255.255");

    /**
     * @brief Send Wake-on-LAN packet to a device
     * @param device Target device (uses MAC address from device)
     */
    void sendWakeOnLanToDevice(const Device& device);

signals:
    /**
     * @brief Emitted when magic packet is successfully sent
     * @param macAddress Target MAC address
     */
    void packetSent(const QString& macAddress);

    /**
     * @brief Emitted when an error occurs during sending
     * @param error Error description
     */
    void sendError(const QString& error);

private:
    static const int WOL_PORT = 9; // Standard Wake-on-LAN port

    /**
     * @brief Send magic packet via UDP broadcast
     * @param packet Magic packet data
     * @param broadcastAddress Broadcast IP address
     * @return True if sent successfully
     */
    bool sendMagicPacket(const QByteArray& packet, const QString& broadcastAddress);
};

#endif // WAKEONLANSERVICE_H
