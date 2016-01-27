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
