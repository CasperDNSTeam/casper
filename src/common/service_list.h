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
#ifndef SERVICELIST_H
#define SERVICELIST_H

#include <QObject>
#include <QIcon>
#include <QMap>

namespace Casper {
class Service {
public:
  Service() {}
  ~Service() {}

  int getId() const;
  void setId(int value);

  QString getName() const;
  void setName(const QString &value);

  QString getDetails() const;
  void setDetails(const QString &value);

  QIcon getIcon() const;
  void setIcon(const QIcon &value);

  QList<QString> GetDNSServiceOrder() const;
  void SetDNSServiceOrder(const QList<QString> &value);

  QString getSecondry() const;
  void setSecondry(const QString &value);

  QString getPrimary() const;
  void setPrimary(const QString &value);

  QString getUrl() const;
  void setUrl(const QString &value);

  bool getActive() const;
  void setActive(bool value);

private:
  int id;
  QString name;
  QString details;
  QString primary;
  QString secondry;
  QString url;
  QIcon icon;
  bool active;
  QList<QString> dns_server_order;
};

class ServiceList : public QObject {
    Q_OBJECT
public:
  static ServiceList *Get();
  virtual ~ServiceList();

  void Init();

  QList<Service> GetCurrentServiceList();
  Service GetService(const QString &name);
  Service GetActiveService();

  void CreateService(const Service &service);
  void DeleteService(const QString &name);

  void SetActiveService(const QString &name);

  void UpdateService(Service service);
protected:
  static ServiceList *__self;
  explicit ServiceList(QObject *parent = 0);

signals:
  void service_added(const QString &);
  void service_removed(const QString &);
  void service_updated(const QString &);
};
}

#endif // SERVICELIST_H
