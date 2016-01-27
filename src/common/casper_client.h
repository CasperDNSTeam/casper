/** Casper DNS Manager For Windows and Mac.
* Copyright (C) 2015 CasperTeam <casperdnsteam@gmail.com>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/
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
