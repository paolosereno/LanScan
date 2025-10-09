#include <QtTest/QtTest>
#include "services/AlertService.h"
#include "models/Alert.h"

class AlertServiceTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Alert model tests
    void testAlertConstruction();
    void testAlertEquality();
    void testAlertToString();
    void testAlertTypeConversion();
    void testAlertSeverityColor();

    // AlertService tests
    void testServiceConstruction();
    void testCreateAlert();
    void testGetAlerts();
    void testGetUnacknowledgedAlerts();
    void testGetAlertsForDevice();
    void testGetAlertsBySeverity();
    void testGetAlertsByType();
    void testAcknowledgeAlert();
    void testAcknowledgeAllAlerts();
    void testClearAlerts();
    void testClearAlertsForDevice();
    void testMaxAlertsLimit();
    void testAlertCountSignals();

private:
    AlertService* service;
};

void AlertServiceTest::initTestCase()
{
    qDebug() << "Starting AlertService tests";
}

void AlertServiceTest::cleanupTestCase()
{
    qDebug() << "AlertService tests completed";
}

void AlertServiceTest::init()
{
    service = new AlertService(this);
}

void AlertServiceTest::cleanup()
{
    delete service;
    service = nullptr;
}

void AlertServiceTest::testAlertConstruction()
{
    Alert alert1;
    QVERIFY(!alert1.id().isEmpty());
    QCOMPARE(alert1.type(), AlertType::ThresholdExceeded);
    QCOMPARE(alert1.severity(), AlertSeverity::Info);
    QCOMPARE(alert1.isAcknowledged(), false);

    Alert alert2(AlertType::HighLatency, AlertSeverity::Warning, "192.168.1.1", "High latency");
    QVERIFY(!alert2.id().isEmpty());
    QCOMPARE(alert2.type(), AlertType::HighLatency);
    QCOMPARE(alert2.severity(), AlertSeverity::Warning);
    QCOMPARE(alert2.deviceId(), QString("192.168.1.1"));
    QCOMPARE(alert2.message(), QString("High latency"));
    QCOMPARE(alert2.isAcknowledged(), false);
}

void AlertServiceTest::testAlertEquality()
{
    Alert alert1(AlertType::HighLatency, AlertSeverity::Warning, "192.168.1.1", "Test");
    Alert alert2 = alert1;

    QCOMPARE(alert1, alert2);

    alert2.setAcknowledged(true);
    QVERIFY(alert1 != alert2);
}

void AlertServiceTest::testAlertToString()
{
    Alert alert(AlertType::PacketLoss, AlertSeverity::Critical, "192.168.1.1", "10% loss");
    QString str = alert.toString();

    QVERIFY(str.contains("Critical"));
    QVERIFY(str.contains("Packet Loss"));
    QVERIFY(str.contains("192.168.1.1"));
    QVERIFY(str.contains("10% loss"));
    QVERIFY(str.contains("Unacknowledged"));
}

void AlertServiceTest::testAlertTypeConversion()
{
    QCOMPARE(Alert::alertTypeToString(AlertType::HighLatency), QString("High Latency"));
    QCOMPARE(Alert::alertTypeToString(AlertType::PacketLoss), QString("Packet Loss"));
    QCOMPARE(Alert::alertTypeToString(AlertType::HighJitter), QString("High Jitter"));
    QCOMPARE(Alert::alertTypeToString(AlertType::DeviceOffline), QString("Device Offline"));
    QCOMPARE(Alert::alertTypeToString(AlertType::DeviceOnline), QString("Device Online"));
}

void AlertServiceTest::testAlertSeverityColor()
{
    QColor infoColor = Alert::severityColor(AlertSeverity::Info);
    QColor warningColor = Alert::severityColor(AlertSeverity::Warning);
    QColor criticalColor = Alert::severityColor(AlertSeverity::Critical);

    QVERIFY(infoColor.isValid());
    QVERIFY(warningColor.isValid());
    QVERIFY(criticalColor.isValid());
    QVERIFY(infoColor != warningColor);
    QVERIFY(warningColor != criticalColor);
}

void AlertServiceTest::testServiceConstruction()
{
    QVERIFY(service != nullptr);
    QCOMPARE(service->getAlertCount(), 0);
    QCOMPARE(service->getUnacknowledgedCount(), 0);
    QCOMPARE(service->getMaxAlerts(), 1000);
}

void AlertServiceTest::testCreateAlert()
{
    QSignalSpy spy(service, &AlertService::alertCreated);

    Alert alert = service->createAlert(
        AlertType::HighLatency,
        AlertSeverity::Warning,
        "192.168.1.1",
        "Latency is 150ms"
    );

    QVERIFY(!alert.id().isEmpty());
    QCOMPARE(alert.type(), AlertType::HighLatency);
    QCOMPARE(alert.severity(), AlertSeverity::Warning);
    QCOMPARE(alert.deviceId(), QString("192.168.1.1"));
    QCOMPARE(alert.message(), QString("Latency is 150ms"));

    QCOMPARE(service->getAlertCount(), 1);
    QCOMPARE(spy.count(), 1);
}

void AlertServiceTest::testGetAlerts()
{
    service->createAlert(AlertType::HighLatency, AlertSeverity::Info, "192.168.1.1", "Test 1");
    service->createAlert(AlertType::PacketLoss, AlertSeverity::Warning, "192.168.1.2", "Test 2");
    service->createAlert(AlertType::HighJitter, AlertSeverity::Critical, "192.168.1.3", "Test 3");

    QList<Alert> alerts = service->getAlerts();
    QCOMPARE(alerts.size(), 3);
}

void AlertServiceTest::testGetUnacknowledgedAlerts()
{
    Alert alert1 = service->createAlert(AlertType::HighLatency, AlertSeverity::Info, "192.168.1.1", "Test 1");
    service->createAlert(AlertType::PacketLoss, AlertSeverity::Warning, "192.168.1.2", "Test 2");

    QCOMPARE(service->getUnacknowledgedCount(), 2);

    service->acknowledgeAlert(alert1.id());

    QCOMPARE(service->getUnacknowledgedCount(), 1);

    QList<Alert> unacknowledged = service->getUnacknowledgedAlerts();
    QCOMPARE(unacknowledged.size(), 1);
    QCOMPARE(unacknowledged[0].deviceId(), QString("192.168.1.2"));
}

void AlertServiceTest::testGetAlertsForDevice()
{
    service->createAlert(AlertType::HighLatency, AlertSeverity::Info, "192.168.1.1", "Test 1");
    service->createAlert(AlertType::PacketLoss, AlertSeverity::Warning, "192.168.1.1", "Test 2");
    service->createAlert(AlertType::HighJitter, AlertSeverity::Critical, "192.168.1.2", "Test 3");

    QList<Alert> device1Alerts = service->getAlertsForDevice("192.168.1.1");
    QCOMPARE(device1Alerts.size(), 2);

    QList<Alert> device2Alerts = service->getAlertsForDevice("192.168.1.2");
    QCOMPARE(device2Alerts.size(), 1);

    QList<Alert> device3Alerts = service->getAlertsForDevice("192.168.1.3");
    QCOMPARE(device3Alerts.size(), 0);
}

void AlertServiceTest::testGetAlertsBySeverity()
{
    service->createAlert(AlertType::HighLatency, AlertSeverity::Info, "192.168.1.1", "Test 1");
    service->createAlert(AlertType::PacketLoss, AlertSeverity::Warning, "192.168.1.2", "Test 2");
    service->createAlert(AlertType::HighJitter, AlertSeverity::Warning, "192.168.1.3", "Test 3");
    service->createAlert(AlertType::DeviceOffline, AlertSeverity::Critical, "192.168.1.4", "Test 4");

    QList<Alert> infoAlerts = service->getAlertsBySeverity(AlertSeverity::Info);
    QCOMPARE(infoAlerts.size(), 1);

    QList<Alert> warningAlerts = service->getAlertsBySeverity(AlertSeverity::Warning);
    QCOMPARE(warningAlerts.size(), 2);

    QList<Alert> criticalAlerts = service->getAlertsBySeverity(AlertSeverity::Critical);
    QCOMPARE(criticalAlerts.size(), 1);
}

void AlertServiceTest::testGetAlertsByType()
{
    service->createAlert(AlertType::HighLatency, AlertSeverity::Info, "192.168.1.1", "Test 1");
    service->createAlert(AlertType::HighLatency, AlertSeverity::Warning, "192.168.1.2", "Test 2");
    service->createAlert(AlertType::PacketLoss, AlertSeverity::Critical, "192.168.1.3", "Test 3");

    QList<Alert> latencyAlerts = service->getAlertsByType(AlertType::HighLatency);
    QCOMPARE(latencyAlerts.size(), 2);

    QList<Alert> lossAlerts = service->getAlertsByType(AlertType::PacketLoss);
    QCOMPARE(lossAlerts.size(), 1);

    QList<Alert> jitterAlerts = service->getAlertsByType(AlertType::HighJitter);
    QCOMPARE(jitterAlerts.size(), 0);
}

void AlertServiceTest::testAcknowledgeAlert()
{
    QSignalSpy spy(service, &AlertService::alertAcknowledged);

    Alert alert = service->createAlert(AlertType::HighLatency, AlertSeverity::Info, "192.168.1.1", "Test");

    QCOMPARE(alert.isAcknowledged(), false);
    QCOMPARE(service->getUnacknowledgedCount(), 1);

    bool result = service->acknowledgeAlert(alert.id());
    QVERIFY(result);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(service->getUnacknowledgedCount(), 0);

    // Acknowledging again should return false
    result = service->acknowledgeAlert(alert.id());
    QVERIFY(!result);
}

void AlertServiceTest::testAcknowledgeAllAlerts()
{
    QSignalSpy spy(service, &AlertService::allAlertsAcknowledged);

    service->createAlert(AlertType::HighLatency, AlertSeverity::Info, "192.168.1.1", "Test 1");
    service->createAlert(AlertType::PacketLoss, AlertSeverity::Warning, "192.168.1.2", "Test 2");
    service->createAlert(AlertType::HighJitter, AlertSeverity::Critical, "192.168.1.3", "Test 3");

    QCOMPARE(service->getUnacknowledgedCount(), 3);

    service->acknowledgeAllAlerts();

    QCOMPARE(service->getUnacknowledgedCount(), 0);
    QCOMPARE(spy.count(), 1);
}

void AlertServiceTest::testClearAlerts()
{
    QSignalSpy spy(service, &AlertService::alertsCleared);

    service->createAlert(AlertType::HighLatency, AlertSeverity::Info, "192.168.1.1", "Test 1");
    service->createAlert(AlertType::PacketLoss, AlertSeverity::Warning, "192.168.1.2", "Test 2");

    QCOMPARE(service->getAlertCount(), 2);

    service->clearAlerts();

    QCOMPARE(service->getAlertCount(), 0);
    QCOMPARE(spy.count(), 1);
}

void AlertServiceTest::testClearAlertsForDevice()
{
    service->createAlert(AlertType::HighLatency, AlertSeverity::Info, "192.168.1.1", "Test 1");
    service->createAlert(AlertType::PacketLoss, AlertSeverity::Warning, "192.168.1.1", "Test 2");
    service->createAlert(AlertType::HighJitter, AlertSeverity::Critical, "192.168.1.2", "Test 3");

    QCOMPARE(service->getAlertCount(), 3);

    service->clearAlertsForDevice("192.168.1.1");

    QCOMPARE(service->getAlertCount(), 1);

    QList<Alert> remaining = service->getAlerts();
    QCOMPARE(remaining[0].deviceId(), QString("192.168.1.2"));
}

void AlertServiceTest::testMaxAlertsLimit()
{
    service->setMaxAlerts(10);
    QCOMPARE(service->getMaxAlerts(), 10);

    // Create 15 alerts
    for (int i = 0; i < 15; ++i) {
        service->createAlert(AlertType::HighLatency, AlertSeverity::Info,
                           QString("192.168.1.%1").arg(i), QString("Test %1").arg(i));
        QTest::qWait(10); // Small delay to ensure different timestamps
    }

    // Should only keep the most recent 10
    QCOMPARE(service->getAlertCount(), 10);
}

void AlertServiceTest::testAlertCountSignals()
{
    QSignalSpy spy(service, &AlertService::alertCountChanged);

    service->createAlert(AlertType::HighLatency, AlertSeverity::Info, "192.168.1.1", "Test");

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 1);  // Total count
    QCOMPARE(arguments.at(1).toInt(), 1);  // Unacknowledged count
}

QTEST_MAIN(AlertServiceTest)
#include "AlertServiceTest.moc"
