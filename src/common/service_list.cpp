#include "service_list.h"

#include <QSettings>
#include <QDebug>

namespace Casper {
ServiceList *ServiceList::__self = 0;

ServiceList *ServiceList::Get() {
  if (__self)
    return __self;

  __self = new ServiceList();
  return __self;
}

ServiceList::ServiceList(QObject *parent) {}

ServiceList::~ServiceList() {}

void ServiceList::Init() {
  if (GetService("Google").getName().isEmpty()) {
    Service service;
    service.setName("Google");
    service.setUrl("http://www.google.com");
    service.setDetails("Google Fast DNS");
    service.setPrimary("8.8.8.8");
    service.setSecondry("8.8.4.1");
    service.setIcon(QIcon());
    service.setActive(false);
    CreateService(service);
  }

  if (GetService("OpenDNS").getName().isEmpty()) {
    Service service;
    service.setName("OpenDNS");
    service.setUrl("http://www.opendns.org");
    service.setDetails("Secure OpenDNS");
    service.setPrimary("208.67.222.222");
    service.setSecondry("208.67.220.220");
    service.setIcon(QIcon());
    service.setActive(true);
    CreateService(service);
  }

  if (GetService("Comodo").getName().isEmpty()) {
    Service service;
    service.setName("Comodo");
    service.setUrl("https://www.comodo.com/secure-dns/");
    service.setDetails("Comodo Secure DNS");
    service.setPrimary("8.26.56.26");
    service.setSecondry("8.20.247.20");
    service.setIcon(QIcon());
    service.setActive(false);
    CreateService(service);
  }

  if (GetService("Norton (A)").getName().isEmpty()) {
    Service service;
    service.setName("Norton (A)");
    service.setUrl("https://dns.norton.com/homeuser.html");
    service.setDetails("Malware and Phishing protections");
    service.setPrimary("199.85.126.10");
    service.setSecondry("199.85.127.10");
    service.setIcon(QIcon());
    service.setActive(false);
    CreateService(service);
  }

  if (GetService("Norton (B)").getName().isEmpty()) {
    Service service;
    service.setName("Norton (B)");
    service.setUrl("https://dns.norton.com/homeuser.html");
    service.setDetails("Security + Porn Prevent");
    service.setPrimary("199.85.126.20");
    service.setSecondry("199.85.127.20");
    service.setIcon(QIcon());
    service.setActive(false);
    CreateService(service);
  }

  if (GetService("Norton (C)").getName().isEmpty()) {
    Service service;
    service.setName("Norton (C)");
    service.setUrl("https://dns.norton.com/homeuser.html");
    service.setDetails("Security + Porn Prevent + Other");
    service.setPrimary("199.85.126.30");
    service.setSecondry("199.85.127.30");
    service.setIcon(QIcon());
    service.setActive(false);
    CreateService(service);
  }

  if (GetService("Dyn Internet Guide").getName().isEmpty()) {
    Service service;
    service.setName("Dyn Internet Guide");
    service.setUrl("https://www.dyn.com");
    service.setDetails("Typo Protection");
    service.setPrimary("216.146.35.35");
    service.setSecondry("216.146.36.36");
    service.setIcon(QIcon());
    service.setActive(false);
    CreateService(service);
  }
}

QList<Service> ServiceList::GetCurrentServiceList() {
  QSettings store("Casper", "casperdns", this);
  QList<Service> rv;

  foreach (const QString &group_name, store.childGroups()) {
    Service service;
    store.beginGroup(group_name);
    service.setName(store.value("name", "").toString());
    service.setUrl(store.value("url").toString());
    service.setDetails(store.value("details").toString());
    service.setPrimary(store.value("primary").toString());
    service.setSecondry(store.value("secondry").toString());
    store.endGroup();

    rv.append(service);
  }

  return rv;
}

Service ServiceList::GetService(const QString &name) {
  QSettings store("Casper", "casperdns", this);
  Service rv;

  foreach (const QString &group_name, store.childGroups()) {
    if (group_name != name)
      continue;

    Service service;
    store.beginGroup(group_name);
    service.setName(store.value("name", "").toString());
    service.setUrl(store.value("url").toString());
    service.setDetails(store.value("details").toString());
    service.setPrimary(store.value("primary").toString());
    service.setSecondry(store.value("secondry").toString());
    service.setActive(store.value("active").toBool());
    store.endGroup();

    return service;
  }

  return rv;
}

Service ServiceList::GetActiveService() {
  QSettings store("Casper", "casperdns", this);
  Service rv;
  foreach (const QString &group_name, store.childGroups()) {
    store.beginGroup(group_name);

    qDebug() << "[get_active_service] - check - " << group_name << ".... ["
             << store.value("active").toBool() << "]";

    if (store.value("active").toBool() != 1) {
      store.endGroup();
      continue;
    }

    Service service;
    service.setName(store.value("name", "").toString());
    service.setUrl(store.value("url").toString());
    service.setDetails(store.value("details").toString());
    service.setPrimary(store.value("primary").toString());
    service.setSecondry(store.value("secondry").toString());
    service.setActive(store.value("active").toBool());
    store.endGroup();

    return service;
  }

  return rv;
}

void ServiceList::CreateService(const Service &service) {
  QSettings store("Casper", "casperdns", this);
  store.beginGroup(service.getName());
  store.setValue("name", service.getName());
  store.setValue("url", service.getUrl());
  store.setValue("details", service.getDetails());
  store.setValue("icon", service.getIcon());
  store.setValue("primary", service.getPrimary());
  store.setValue("secondry", service.getSecondry());
  store.setValue("active", service.getActive());
  store.endGroup();
  store.sync();

  emit service_added(service.getName());
}

void ServiceList::DeleteService(const QString &name) {
  QSettings store("Casper", "casperdns", this);
  store.remove(name);
  store.sync();

  emit service_removed(name);
}

void ServiceList::UpdateService(Service service) {
  // actually updates the service
  CreateService(service);
  emit service_updated(service.getName());
}

void ServiceList::SetActiveService(const QString &name) {
  Service service = GetService(name);

  if (service.getName().isEmpty() || service.getName().isNull()) {
    qCritical() << Q_FUNC_INFO << "Fatal error emptry service - Input";
    return;
  }

  Service active_service = GetActiveService();

  if (active_service.getName().isEmpty() || active_service.getName().isNull()) {
    qCritical() << Q_FUNC_INFO << "Fatal error emptry service - Active";

    service.setActive(true);
    UpdateService(service);

    return;
  }

  active_service.setActive(false);
  UpdateService(active_service);

  service.setActive(true);
  UpdateService(service);
}

int Service::getId() const { return id; }

void Service::setId(int value) { id = value; }

QString Service::getName() const { return name; }

void Service::setName(const QString &value) { name = value; }

QString Service::getDetails() const { return details; }

void Service::setDetails(const QString &value) { details = value; }

QIcon Service::getIcon() const { return icon; }

void Service::setIcon(const QIcon &value) { icon = value; }

QList<QString> Service::GetDNSServiceOrder() const { return dns_server_order; }

void Service::SetDNSServiceOrder(const QList<QString> &value) {
  dns_server_order = value;
}

QString Service::getSecondry() const { return secondry; }

void Service::setSecondry(const QString &value) { secondry = value; }

QString Service::getPrimary() const { return primary; }

void Service::setPrimary(const QString &value) { primary = value; }

QString Service::getUrl() const { return url; }

void Service::setUrl(const QString &value) { url = value; }

bool Service::getActive() const { return active; }

void Service::setActive(bool value) { active = value; }
}
