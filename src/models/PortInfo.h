#ifndef PORTINFO_H
#define PORTINFO_H

#include <QString>

class PortInfo
{
public:
    enum Protocol {
        TCP,
        UDP
    };

    enum State {
        Open,
        Closed,
        Filtered
    };

    PortInfo();
    PortInfo(int portNumber, Protocol protocol = TCP);

    // Getters
    int portNumber() const;
    Protocol protocol() const;
    QString protocolString() const;
    QString service() const;
    State state() const;
    QString stateString() const;

    // Alternative getters with "get" prefix
    int getPort() const;
    Protocol getProtocol() const;
    QString getService() const;
    State getState() const;

    // Setters
    void setPortNumber(int portNumber);
    void setProtocol(Protocol protocol);
    void setService(const QString& service);
    void setState(State state);

    // Utility methods
    static QString getDefaultService(int portNumber, Protocol protocol);

private:
    int m_portNumber;
    Protocol m_protocol;
    QString m_service;
    State m_state;
};

#endif // PORTINFO_H
