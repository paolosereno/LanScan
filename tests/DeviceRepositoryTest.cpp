#include <QtTest>
#include "database/DeviceRepository.h"
#include "database/DatabaseManager.h"
#include "models/Device.h"

class DeviceRepositoryTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testSaveAndFindById();
    void testFindByIp();
    void testFindAll();
    void testUpdate();
    void testRemove();
    void testExists();
    void testCache();

private:
    DatabaseManager* db;
    DeviceRepository* repo;
    QString testDbPath;
};

void DeviceRepositoryTest::initTestCase() {
    testDbPath = ":memory:"; // Use in-memory SQLite for testing
    db = DatabaseManager::instance();
}

void DeviceRepositoryTest::cleanupTestCase() {
    db->close();
}

void DeviceRepositoryTest::init() {
    QVERIFY(db->open(testDbPath));
    QVERIFY(db->createSchema());
    repo = new DeviceRepository(db);
}

void DeviceRepositoryTest::cleanup() {
    delete repo;
    db->close();
}

void DeviceRepositoryTest::testSaveAndFindById() {
    Device device;
    device.setId("device-1");
    device.setIp("192.168.1.100");
    device.setHostname("test-device");
    device.setMacAddress("AA:BB:CC:DD:EE:FF");
    device.setVendor("Test Vendor");
    device.setOnline(true);

    repo->save(device);

    Device found = repo->findById("device-1");
    QCOMPARE(found.getId(), QString("device-1"));
    QCOMPARE(found.getIp(), QString("192.168.1.100"));
    QCOMPARE(found.getHostname(), QString("test-device"));
    QCOMPARE(found.getMacAddress(), QString("AA:BB:CC:DD:EE:FF"));
    QCOMPARE(found.getVendor(), QString("Test Vendor"));
    QVERIFY(found.isOnline());
}

void DeviceRepositoryTest::testFindByIp() {
    Device device;
    device.setId("device-2");
    device.setIp("192.168.1.200");
    device.setHostname("ip-test-device");

    repo->save(device);

    Device found = repo->findByIp("192.168.1.200");
    QCOMPARE(found.getId(), QString("device-2"));
    QCOMPARE(found.getIp(), QString("192.168.1.200"));
}

void DeviceRepositoryTest::testFindAll() {
    Device device1;
    device1.setId("device-3");
    device1.setIp("192.168.1.1");

    Device device2;
    device2.setId("device-4");
    device2.setIp("192.168.1.2");

    repo->save(device1);
    repo->save(device2);

    QList<Device> all = repo->findAll();
    QCOMPARE(all.size(), 2);
}

void DeviceRepositoryTest::testUpdate() {
    Device device;
    device.setId("device-5");
    device.setIp("192.168.1.50");
    device.setHostname("original");

    repo->save(device);

    device.setHostname("updated");
    repo->update(device);

    Device found = repo->findById("device-5");
    QCOMPARE(found.getHostname(), QString("updated"));
}

void DeviceRepositoryTest::testRemove() {
    Device device;
    device.setId("device-6");
    device.setIp("192.168.1.60");

    repo->save(device);
    QVERIFY(repo->exists("device-6"));

    repo->remove("device-6");
    QVERIFY(!repo->exists("device-6"));
}

void DeviceRepositoryTest::testExists() {
    Device device;
    device.setId("device-7");
    device.setIp("192.168.1.70");

    QVERIFY(!repo->exists("device-7"));

    repo->save(device);
    QVERIFY(repo->exists("device-7"));
}

void DeviceRepositoryTest::testCache() {
    Device device;
    device.setId("device-8");
    device.setIp("192.168.1.80");

    repo->save(device);

    // First access should load from DB and cache
    Device found1 = repo->findById("device-8");
    QCOMPARE(found1.getId(), QString("device-8"));

    // Second access should hit cache
    Device found2 = repo->findById("device-8");
    QCOMPARE(found2.getId(), QString("device-8"));

    // Clear cache
    repo->clearCache();

    // Should still find from DB
    Device found3 = repo->findById("device-8");
    QCOMPARE(found3.getId(), QString("device-8"));
}

QTEST_MAIN(DeviceRepositoryTest)
#include "DeviceRepositoryTest.moc"
