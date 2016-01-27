#include "casper_runtime.h"

#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QTimer>
#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>

#include <casper_client.h>

#include <casper_app_window.h>
#include <service_list.h>

#ifdef Q_OS_MAC
#include <casper_nativeapphandle.h>
#endif

namespace Casper {
class CDRunTime::CDRunTimeContext {
public:
  CDRunTimeContext() : m_user_invoked(1) {}
  ~CDRunTimeContext() {}

  CasperAppWindow *m_app_window;

  QSystemTrayIcon *m_tray_window;
  QMenu *m_system_tray_menu;
  Casper::Client *m_ipc_client;

  bool m_user_invoked;
};

CDRunTime::CDRunTime(QObject *parent, bool show_ui)
    : QObject(parent), ctx(new CDRunTimeContext) {
  ctx->m_user_invoked = show_ui;
  ctx->m_tray_window = new QSystemTrayIcon(this);
  ctx->m_tray_window->setIcon(QIcon(":/casper/artwork/casper.png"));
  ctx->m_system_tray_menu = new QMenu(0);

  Casper::ServiceList::Get()->Init();

  QAction *configure_action = new QAction(this);

  configure_action->setCheckable(false);
  configure_action->setText(tr("Show Casper Agent"));
  configure_action->setData("1");
  connect(configure_action, &QAction::triggered, this,
          [this](bool trigger) {

#ifdef Q_OS_MAC
      Casper::NativeAppHandle::show();
#endif

      ctx->m_app_window->show();
      ctx->m_app_window->raise();
  });

  QAction *help_action = new QAction(this);
  help_action->setText(tr("Open Casper Preferences..."));
  help_action->setData("2");

  QAction *quit_action = new QAction(this);
  quit_action->setText(tr("Exit Casper Agent"));
  connect(quit_action, &QAction::triggered, this, [this](bool toggled) {
    qDebug() << "[casper] [client] [system_tray]"
             << "Exit Requested";
    Casper::Client::Get()->RollbackDNS();

#ifdef Q_OS_MAC
    Casper::NativeAppHandle::quit();
#endif

    qApp->quit();
  });

  ctx->m_system_tray_menu->addAction(configure_action);
  ctx->m_system_tray_menu->addSeparator();
  ctx->m_system_tray_menu->addAction(quit_action);
  ctx->m_system_tray_menu->addSeparator();
  ctx->m_system_tray_menu->addAction(help_action);
  ctx->m_system_tray_menu->addSeparator();

  ctx->m_tray_window->setContextMenu(ctx->m_system_tray_menu);
  ctx->m_tray_window->show();

  connect(ctx->m_system_tray_menu, &QMenu::triggered, this,
          [this](QAction *action) {});

  connect(ctx->m_tray_window, &QSystemTrayIcon::activated, this,
          [this](QSystemTrayIcon::ActivationReason reason) {
    qDebug() << "[casper] [client] [ui] - Triggered";
#ifdef Q_OS_WIN
    ctx->m_app_window->show();
#endif
  });

  ctx->m_app_window = new CasperAppWindow();
  // ipc
  ctx->m_ipc_client = Casper::Client::Get();

  connect(Casper::Client::Get(), &Casper::Client::ClientState, this,
          [this](Casper::ConnectionStatusType reason) {
    if (reason == Casper::kClientConnected) {
      ctx->m_app_window->show();
      ctx->m_app_window->UpdateSystemDNS();
    }
  });

  Casper::Client::Get()->InitHandShake();

#ifdef Q_OS_WIN
  ctx->m_app_window->show();
#endif
}

CDRunTime::~CDRunTime() { delete ctx; }

void CDRunTime::ShowMainWindow()
{
  ctx->m_app_window->show();
}

void CDRunTime::PostNotification(const QString &message) {
  ctx->m_tray_window->showMessage("Casper DNS", message);
}
}
