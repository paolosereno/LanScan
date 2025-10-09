#include "services/WakeOnLanService.h"
#include "models/Device.h"
#include "utils/Logger.h"
#include <QHostAddress>
#include <QAbstractSocket>

// ============================================================================
// WakeOnLanPacket Implementation
// ============================================================================

QByteArray WakeOnLanPacket::createMagicPacket(const QString& macAddress) {
    if (!isValidMacAddress(macAddress)) {
        Logger::error("Invalid MAC address format: " + macAddress);
        return QByteArray();
    }

    QByteArray mac = parseMacAddress(macAddress);

    if (mac.size() != 6) {
        Logger::error("Failed to parse MAC address: " + macAddress);
        return QByteArray();
    }

    // Magic packet structure:
    // 6 bytes of 0xFF followed by 16 repetitions of target MAC address
    QByteArray packet;
    packet.reserve(102); // 6 + (16 * 6) = 102 bytes

    // Add header (6 bytes of 0xFF)
    for (int i = 0; i < 6; i++) {
        packet.append(static_cast<char>(0xFF));
    }

    // Add MAC address 16 times
    for (int i = 0; i < 16; i++) {
        packet.append(mac);
    }

    Logger::debug(QString("Created magic packet for %1 (%2 bytes)")
                 .arg(macAddress)
                 .arg(packet.size()));

    return packet;
}

bool WakeOnLanPacket::isValidMacAddress(const QString& macAddress) {
    if (macAddress.isEmpty()) {
        return false;
    }

    // MAC address regex: XX:XX:XX:XX:XX:XX or XX-XX-XX-XX-XX-XX
    // Supports uppercase and lowercase hex digits
    QRegularExpression regex(
        "^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$"
    );

    return regex.match(macAddress).hasMatch();
}

QByteArray WakeOnLanPacket::parseMacAddress(const QString& macAddress) {
    QByteArray mac;

    // Remove separators (: or -)
    QString cleaned = macAddress;
    cleaned.remove(':');
    cleaned.remove('-');
    cleaned.remove(' ');

    // Validate cleaned string length
    if (cleaned.length() != 12) {
        Logger::error(QString("Invalid MAC address length after cleaning: %1 (expected 12, got %2)")
                     .arg(macAddress)
                     .arg(cleaned.length()));
        return QByteArray();
    }

    // Convert hex string to bytes
    for (int i = 0; i < cleaned.length(); i += 2) {
        QString hexByte = cleaned.mid(i, 2);
        bool ok;
        quint8 byte = static_cast<quint8>(hexByte.toUInt(&ok, 16));
        if (!ok) {
            Logger::error(QString("Failed to parse hex byte: %1 in MAC %2")
                         .arg(hexByte)
                         .arg(macAddress));
            return QByteArray();
        }
        mac.append(static_cast<char>(byte));
    }

    return mac;
}

// ============================================================================
// WakeOnLanService Implementation
// ============================================================================

WakeOnLanService::WakeOnLanService(QObject* parent)
    : QObject(parent)
{
    Logger::info("WakeOnLanService initialized");
}

WakeOnLanService::~WakeOnLanService() {
    Logger::debug("WakeOnLanService destroyed");
}

void WakeOnLanService::sendWakeOnLan(const QString& macAddress,
                                     const QString& broadcastAddress) {
    Logger::info(QString("Attempting to send WoL packet to %1 via %2")
                .arg(macAddress)
                .arg(broadcastAddress));

    // Create magic packet
    QByteArray packet = WakeOnLanPacket::createMagicPacket(macAddress);

    if (packet.isEmpty()) {
        QString error = tr("Invalid MAC address: %1").arg(macAddress);
        Logger::error(error);
        emit sendError(error);
        return;
    }

    // Send packet
    if (sendMagicPacket(packet, broadcastAddress)) {
        Logger::info(QString("WoL packet sent successfully to %1").arg(macAddress));
        emit packetSent(macAddress);
    } else {
        QString error = tr("Failed to send WoL packet to %1").arg(macAddress);
        Logger::error(error);
        emit sendError(error);
    }
}

void WakeOnLanService::sendWakeOnLanToDevice(const Device& device) {
    if (device.macAddress().isEmpty()) {
        QString error = tr("Device has no MAC address: %1").arg(device.getIp());
        Logger::error(error);
        emit sendError(error);
        return;
    }

    Logger::info(QString("Sending WoL to device: %1 (%2 / %3)")
                .arg(device.hostname().isEmpty() ? "Unknown" : device.hostname())
                .arg(device.getIp())
                .arg(device.macAddress()));

    // Use default broadcast address
    sendWakeOnLan(device.macAddress(), "255.255.255.255");
}

bool WakeOnLanService::sendMagicPacket(const QByteArray& packet,
                                       const QString& broadcastAddress) {
    // Create a temporary UDP socket for sending
    QUdpSocket udpSocket;

    // Enable broadcast mode
    if (!udpSocket.bind(QHostAddress::AnyIPv4, 0, QAbstractSocket::ShareAddress)) {
        Logger::error(QString("Failed to bind UDP socket: %1")
                     .arg(udpSocket.errorString()));
        return false;
    }

    // Note: Qt 6 doesn't have BroadcastOption enum anymore
    // Broadcasting is enabled automatically when sending to broadcast address

    // Parse broadcast address
    QHostAddress broadcast(broadcastAddress);
    if (broadcast.isNull()) {
        Logger::error(QString("Invalid broadcast address: %1").arg(broadcastAddress));
        return false;
    }

    // Send the magic packet
    qint64 bytesSent = udpSocket.writeDatagram(
        packet,
        broadcast,
        WOL_PORT
    );

    if (bytesSent < 0) {
        Logger::error(QString("Failed to send datagram: %1")
                     .arg(udpSocket.errorString()));
        return false;
    }

    if (bytesSent != packet.size()) {
        Logger::error(QString("Partial packet sent: %1/%2 bytes")
                     .arg(bytesSent)
                     .arg(packet.size()));
        return false;
    }

    Logger::debug(QString("Sent %1 bytes to %2:%3")
                 .arg(bytesSent)
                 .arg(broadcastAddress)
                 .arg(WOL_PORT));

    // Wait for the datagram to be sent
    udpSocket.waitForBytesWritten(1000);

    return true;
}
