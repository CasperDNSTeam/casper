#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>

#ifdef Q_OS_MAC
#include  <CoreFoundation/CoreFoundation.h>
#endif

#ifdef Q_OS_WIN
typedef bool Boolean;
#endif

namespace Casper {
typedef enum {
    kClientConnected,
    kClientAgentMessage,
    kClientAgentResponse,
    kClientDiconnected
} ConnectionStatusType;

class Client : public QObject
{
    Q_OBJECT
public:
    static Client *Get();
    virtual ~Client();

    virtual Boolean InitConnection();
    virtual Boolean InitHandShake();

    virtual Boolean GetAccess();

    virtual void UpdateDNS(const QString &_IN_primary,
                           const QString &_IN_secondry);
    virtual void RollbackDNS();
protected:
    static Client *__self;
    explicit Client(QObject *parent = 0);
signals:
   void ClientState(ConnectionStatusType);

private:
    class ClientContext;
    ClientContext * const ctx;
};
}

#endif // CLIENT_H
