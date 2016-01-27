#ifndef CASPER_CD_RUN_TIME_H
#define CASPER_CD_RUN_TIME_H

#include <QObject>

#include <QSystemTrayIcon>

namespace Casper {
class CDRunTime : public QObject {
  Q_OBJECT
public:
  explicit CDRunTime(QObject *parent = 0, bool show_ui = true);
  virtual ~CDRunTime();

  void ShowMainWindow();

  void PostNotification(const QString &message);

private:
  class CDRunTimeContext;
  CDRunTimeContext *const ctx;
};
}

#endif
