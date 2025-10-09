#include <QtTest>
#include <QSignalSpy>
#include "diagnostics/DnsDiagnostics.h"
#include "utils/Logger.h"

class DnsDiagnosticsTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        Logger::setLogLevel(Logger::DEBUG);
        Logger::enableConsoleOutput(true);
    }

    void cleanupTestCase()
    {
        // No cleanup needed
    }

    // Test 1: Basic construction
    void testConstruction()
    {
        DnsDiagnostics dns;
        QVERIFY(!dns.isRunning());
        QCOMPARE(dns.queryTime(), 0LL);
        QVERIFY(dns.hostname().isEmpty());
        QVERIFY(dns.nameserver().isEmpty());
    }

    // Test 2: DnsRecord toString
    void testDnsRecordToString()
    {
        DnsDiagnostics::DnsRecord record;
        record.type = DnsDiagnostics::A;
        record.name = "google.com";
        record.value = "142.250.185.78";
        record.ttl = 300;
        record.priority = 0;

        QString str = record.toString();
        QVERIFY(str.contains("A:"));
        QVERIFY(str.contains("google.com"));
        QVERIFY(str.contains("142.250.185.78"));
        QVERIFY(str.contains("TTL: 300s"));

        // Test MX record with priority
        record.type = DnsDiagnostics::MX;
        record.value = "mail.google.com";
        record.priority = 10;

        str = record.toString();
        QVERIFY(str.contains("MX:"));
        QVERIFY(str.contains("Priority: 10"));
    }

    // Test 3: Record type to string conversion
    void testRecordTypeToString()
    {
        QCOMPARE(DnsDiagnostics::recordTypeToString(DnsDiagnostics::A), QString("A"));
        QCOMPARE(DnsDiagnostics::recordTypeToString(DnsDiagnostics::AAAA), QString("AAAA"));
        QCOMPARE(DnsDiagnostics::recordTypeToString(DnsDiagnostics::MX), QString("MX"));
        QCOMPARE(DnsDiagnostics::recordTypeToString(DnsDiagnostics::NS), QString("NS"));
        QCOMPARE(DnsDiagnostics::recordTypeToString(DnsDiagnostics::TXT), QString("TXT"));
        QCOMPARE(DnsDiagnostics::recordTypeToString(DnsDiagnostics::CNAME), QString("CNAME"));
        QCOMPARE(DnsDiagnostics::recordTypeToString(DnsDiagnostics::PTR), QString("PTR"));
        QCOMPARE(DnsDiagnostics::recordTypeToString(DnsDiagnostics::SOA), QString("SOA"));
        QCOMPARE(DnsDiagnostics::recordTypeToString(DnsDiagnostics::SRV), QString("SRV"));
        QCOMPARE(DnsDiagnostics::recordTypeToString(DnsDiagnostics::ANY), QString("ANY"));
    }

    // Test 4: Record type to QDnsLookup::Type conversion
    void testToQDnsLookupType()
    {
        QCOMPARE(DnsDiagnostics::toQDnsLookupType(DnsDiagnostics::A), QDnsLookup::A);
        QCOMPARE(DnsDiagnostics::toQDnsLookupType(DnsDiagnostics::AAAA), QDnsLookup::AAAA);
        QCOMPARE(DnsDiagnostics::toQDnsLookupType(DnsDiagnostics::MX), QDnsLookup::MX);
        QCOMPARE(DnsDiagnostics::toQDnsLookupType(DnsDiagnostics::NS), QDnsLookup::NS);
        QCOMPARE(DnsDiagnostics::toQDnsLookupType(DnsDiagnostics::TXT), QDnsLookup::TXT);
        QCOMPARE(DnsDiagnostics::toQDnsLookupType(DnsDiagnostics::CNAME), QDnsLookup::CNAME);
        QCOMPARE(DnsDiagnostics::toQDnsLookupType(DnsDiagnostics::PTR), QDnsLookup::PTR);
        QCOMPARE(DnsDiagnostics::toQDnsLookupType(DnsDiagnostics::SRV), QDnsLookup::SRV);
        // Note: SOA type may not be available in all Qt versions
    }

    // Test 5: Invalid inputs
    void testInvalidInputs()
    {
        DnsDiagnostics dns;

        // Empty hostname
        QVERIFY(!dns.queryRecords("", DnsDiagnostics::A));

        // Empty IP for reverse lookup
        QVERIFY(!dns.reverseLookup(""));

        // Invalid IP for reverse lookup
        QVERIFY(!dns.reverseLookup("not.an.ip.address"));
    }

    // Test 6: A record query (google.com)
    void testARecordQuery()
    {
        DnsDiagnostics dns;

        QSignalSpy completedSpy(&dns, &DnsDiagnostics::lookupCompleted);
        QSignalSpy errorSpy(&dns, &DnsDiagnostics::lookupError);

        QVERIFY(dns.queryRecords("google.com", DnsDiagnostics::A));
        QVERIFY(dns.isRunning());
        QCOMPARE(dns.hostname(), QString("google.com"));

        // Wait for completion (10 second timeout)
        QVERIFY(completedSpy.wait(10000) || errorSpy.wait(10000));

        if (completedSpy.count() > 0) {
            QVERIFY(!dns.isRunning());
            QVERIFY(dns.queryTime() > 0);

            QList<DnsDiagnostics::DnsRecord> records =
                completedSpy.first().first().value<QList<DnsDiagnostics::DnsRecord>>();

            QVERIFY(records.size() > 0);

            // Check first record
            QCOMPARE(records.first().type, DnsDiagnostics::A);
            QVERIFY(!records.first().value.isEmpty());

            qDebug() << "google.com A records:";
            for (const auto& record : records) {
                qDebug() << "  " << record.toString();
            }
        } else {
            qDebug() << "DNS query failed (network may be unavailable):"
                     << errorSpy.first().first().toString();
        }
    }

    // Test 7: Forward lookup
    void testForwardLookup()
    {
        DnsDiagnostics dns;

        QSignalSpy completedSpy(&dns, &DnsDiagnostics::lookupCompleted);
        QSignalSpy errorSpy(&dns, &DnsDiagnostics::lookupError);

        QVERIFY(dns.forwardLookup("www.google.com"));

        QVERIFY(completedSpy.wait(10000) || errorSpy.wait(10000));

        if (completedSpy.count() > 0) {
            QList<DnsDiagnostics::DnsRecord> records =
                completedSpy.first().first().value<QList<DnsDiagnostics::DnsRecord>>();

            QVERIFY(records.size() > 0);
            qDebug() << "www.google.com resolved to:" << records.first().value;
        }
    }

    // Test 8: Reverse lookup
    void testReverseLookup()
    {
        DnsDiagnostics dns;

        QSignalSpy completedSpy(&dns, &DnsDiagnostics::lookupCompleted);
        QSignalSpy errorSpy(&dns, &DnsDiagnostics::lookupError);

        // Google's public DNS
        QVERIFY(dns.reverseLookup("8.8.8.8"));

        QVERIFY(completedSpy.wait(10000) || errorSpy.wait(10000));

        if (completedSpy.count() > 0) {
            QList<DnsDiagnostics::DnsRecord> records =
                completedSpy.first().first().value<QList<DnsDiagnostics::DnsRecord>>();

            if (records.size() > 0) {
                QCOMPARE(records.first().type, DnsDiagnostics::PTR);
                qDebug() << "8.8.8.8 reverse lookup:" << records.first().value;
            }
        } else {
            qDebug() << "Reverse lookup failed (may not have PTR record)";
        }
    }

    // Test 9: MX record query
    void testMxRecordQuery()
    {
        DnsDiagnostics dns;

        QSignalSpy completedSpy(&dns, &DnsDiagnostics::lookupCompleted);
        QSignalSpy errorSpy(&dns, &DnsDiagnostics::lookupError);

        QVERIFY(dns.queryRecords("gmail.com", DnsDiagnostics::MX));

        QVERIFY(completedSpy.wait(10000) || errorSpy.wait(10000));

        if (completedSpy.count() > 0) {
            QList<DnsDiagnostics::DnsRecord> records =
                completedSpy.first().first().value<QList<DnsDiagnostics::DnsRecord>>();

            if (records.size() > 0) {
                qDebug() << "gmail.com MX records:";
                for (const auto& record : records) {
                    QCOMPARE(record.type, DnsDiagnostics::MX);
                    QVERIFY(record.priority > 0);
                    qDebug() << "  " << record.toString();
                }
            }
        }
    }

    // Test 10: Cancel lookup
    void testCancelLookup()
    {
        DnsDiagnostics dns;

        QSignalSpy completedSpy(&dns, &DnsDiagnostics::lookupCompleted);

        QVERIFY(dns.queryRecords("google.com", DnsDiagnostics::A));
        QVERIFY(dns.isRunning());

        // Cancel immediately
        dns.cancel();

        QVERIFY(!dns.isRunning());

        // Wait to ensure no signal is emitted
        QTest::qWait(1000);
        QCOMPARE(completedSpy.count(), 0);
    }

    // Test 11: Cannot start multiple lookups
    void testMultipleLookups()
    {
        DnsDiagnostics dns;

        QVERIFY(dns.queryRecords("google.com", DnsDiagnostics::A));
        QVERIFY(dns.isRunning());

        // Try to start another lookup
        QVERIFY(!dns.queryRecords("yahoo.com", DnsDiagnostics::A));

        dns.cancel();
    }

    // Test 12: Progress updates
    void testProgressUpdates()
    {
        DnsDiagnostics dns;

        QSignalSpy progressSpy(&dns, &DnsDiagnostics::progressUpdated);
        QSignalSpy completedSpy(&dns, &DnsDiagnostics::lookupCompleted);
        QSignalSpy errorSpy(&dns, &DnsDiagnostics::lookupError);

        QVERIFY(dns.queryRecords("google.com", DnsDiagnostics::A));

        QVERIFY(completedSpy.wait(10000) || errorSpy.wait(10000));

        // Should have received at least one progress update
        QVERIFY(progressSpy.count() > 0);

        QString progressMsg = progressSpy.first().first().toString();
        QVERIFY(!progressMsg.isEmpty());
        qDebug() << "Progress message:" << progressMsg;
    }

    // Test 13: Nonexistent domain
    void testNonexistentDomain()
    {
        DnsDiagnostics dns;

        QSignalSpy completedSpy(&dns, &DnsDiagnostics::lookupCompleted);
        QSignalSpy errorSpy(&dns, &DnsDiagnostics::lookupError);

        QVERIFY(dns.queryRecords("this-domain-definitely-does-not-exist-12345.com", DnsDiagnostics::A));

        QVERIFY(completedSpy.wait(10000) || errorSpy.wait(10000));

        // Should get an error for nonexistent domain
        if (errorSpy.count() > 0) {
            QString error = errorSpy.first().first().toString();
            qDebug() << "Expected error for nonexistent domain:" << error;
            QVERIFY(!error.isEmpty());
        }
    }

    // Test 14: Custom nameserver
    void testCustomNameserver()
    {
        DnsDiagnostics dns;

        QSignalSpy completedSpy(&dns, &DnsDiagnostics::lookupCompleted);
        QSignalSpy errorSpy(&dns, &DnsDiagnostics::lookupError);

        // Use Google's public DNS
        QVERIFY(dns.queryRecords("google.com", DnsDiagnostics::A, "8.8.8.8"));
        QCOMPARE(dns.nameserver(), QString("8.8.8.8"));

        QVERIFY(completedSpy.wait(10000) || errorSpy.wait(10000));

        if (completedSpy.count() > 0) {
            QList<DnsDiagnostics::DnsRecord> records =
                completedSpy.first().first().value<QList<DnsDiagnostics::DnsRecord>>();

            QVERIFY(records.size() > 0);
            qDebug() << "Query via 8.8.8.8 succeeded, found" << records.size() << "records";
        }
    }

    // Test 15: Destructor cancels running lookup
    void testDestructorCancels()
    {
        {
            DnsDiagnostics dns;
            QVERIFY(dns.queryRecords("google.com", DnsDiagnostics::A));
            QVERIFY(dns.isRunning());
            // dns goes out of scope and destructor is called
        }
        // If we reach here without crash, destructor properly cleaned up
        QVERIFY(true);
    }
};

QTEST_MAIN(DnsDiagnosticsTest)
#include "DnsDiagnosticsTest.moc"
