#include "casper_app_window.h"
#include "ui_casper_app_window.h"
#include <QMacNativeWidget>
#include <QMacCocoaViewContainer>
#include <QDebug>
#include <QCloseEvent>

#include <casper_configure_dialog.h>
#include <casper_client.h>

#include <service_list.h>

class CasperAppWindow::PrivateAppContext {
public:
  PrivateAppContext() : mWindowPtr(new Ui::CasperAppWindow) {}
  ~PrivateAppContext() { delete mWindowPtr; }
  Ui::CasperAppWindow *mWindowPtr;

  Casper::ServiceList *mServiceListPtr;
};

void CasperAppWindow::UpdateServiceList() {
  ctx->mWindowPtr->m_service_list->clear();
  foreach (const Casper::Service &service,
           ctx->mServiceListPtr->GetCurrentServiceList()) {
    if (service.getName().isEmpty() || service.getName().isNull())
      continue;
    ctx->mWindowPtr->m_service_list->addItem(service.getName());
  }

  ctx->mWindowPtr->m_service_list->insertSeparator(
      ctx->mServiceListPtr->GetCurrentServiceList().count());
  ctx->mWindowPtr->m_service_list->addItem("+");
}

void CasperAppWindow::UpdateSystemDNS() {
  QString current_service = ctx->mWindowPtr->m_service_list->currentText();
  Casper::Client::Get()->UpdateDNS(
      Casper::ServiceList::Get()->GetService(current_service).getPrimary(),
      Casper::ServiceList::Get()->GetService(current_service).getSecondry());
  ctx->mWindowPtr->m_service_btn->setButtonActionState(
      UIButton::kButtonActionActive);
  // ctx->mWindowPtr->dns_status->setText("DNS ENABLED");
  //     statusBar()->showMessage(tr("ON"));
  ctx->mWindowPtr->m_service_btn->setChecked(true);
}

CasperAppWindow::CasperAppWindow(QWidget *parent)
    : QMainWindow(parent), ctx(new PrivateAppContext) {
  ctx->mWindowPtr->setupUi(this);
  setWindowFlags(windowFlags() ^ Qt::WindowMaximizeButtonHint);
  ctx->mWindowPtr->toolBar->setFloatable(false);
  ctx->mWindowPtr->toolBar->setMovable(false);
  ctx->mWindowPtr->toolBar->setContextMenuPolicy(Qt::NoContextMenu);
  ctx->mWindowPtr->toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
  ctx->mWindowPtr->toolBar->addAction(
      QIcon(":/casper/artwork/casper_ui_toolbar_settings.svg"), "Settings");

#ifdef Q_OS_WIN
  setMinimumSize(211, 250);
  setMaximumSize(211, 250);
  setWindowTitle("Casper DNS");
  setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
#endif
  ctx->mServiceListPtr = Casper::ServiceList::Get();
  ctx->mServiceListPtr->Init();

  connect(ctx->mServiceListPtr, &Casper::ServiceList::service_added, this,
          [this]() {});

  UpdateServiceList();

  ctx->mWindowPtr->m_service_list->setCurrentText(
      Casper::ServiceList::Get()->GetActiveService().getName());

  connect(ctx->mWindowPtr->m_service_btn, &UIButton::toggled, this,
          [this](bool checked) {
    if (checked) {
      UpdateSystemDNS();
    } else {
      ctx->mWindowPtr->m_service_btn->setButtonActionState(
          UIButton::kButtonActionInactive);

      Casper::Client::Get()->RollbackDNS();
      // ctx->mWindowPtr->dns_status->setText("DNS DISABLED");
      // statusBar()->showMessage(tr("OFF"));
    }
  });

  connect(ctx->mWindowPtr->m_service_list, SIGNAL(activated(QString)), this,
          SLOT(on_activated(QString)));
  connect(ctx->mWindowPtr->m_service_list, SIGNAL(highlighted(QString)), this,
          SLOT(on_highligted(QString)));

  connect(Casper::ServiceList::Get(), &Casper::ServiceList::service_added, this,
          [this](const QString &name) { UpdateServiceList(); });

  connect(Casper::ServiceList::Get(), &Casper::ServiceList::service_removed,
          this, [this](const QString &name) {
    for (int i = 0;
         i < Casper::ServiceList::Get()->GetCurrentServiceList().count(); i++) {
      if (ctx->mWindowPtr->m_service_list->itemText(i) == name) {
        ctx->mWindowPtr->m_service_list->removeItem(i);
        break;
      }
    }
  });

  connect(ctx->mWindowPtr->toolBar, &QToolBar::actionTriggered, this,
          [this](QAction *action) {
    if (!action)
      return;
    ConfigureDialog *dialog = new ConfigureDialog(this);
    dialog->show();
  });
}

CasperAppWindow::~CasperAppWindow() { delete ctx; }

void CasperAppWindow::on_activated(const QString &text) {
  if (text == "+") {
    if (Casper::ServiceList::Get()->GetActiveService().getName().isEmpty())
      qFatal("can't be empty");

    ctx->mWindowPtr->m_service_list->setCurrentText(
        Casper::ServiceList::Get()->GetActiveService().getName());

    // Casper::Client::Get()->RollbackDNS();
    ConfigureDialog *dialog = new ConfigureDialog(this);
    dialog->show();
    return;
  }
  qDebug() << Q_FUNC_INFO << "[ui] [activated]" << text;
  qDebug() << Q_FUNC_INFO << "[ui] [activated] [primary]"
           << Casper::ServiceList::Get()->GetService(text).getPrimary();

  ctx->mWindowPtr->m_service_btn->setChecked(false);
  ctx->mWindowPtr->m_service_btn->setChecked(true);

  Casper::ServiceList::Get()->SetActiveService(text);

  ctx->mWindowPtr->m_service_list->setCurrentText(
      Casper::ServiceList::Get()->GetActiveService().getName());
}

void CasperAppWindow::on_highligted(const QString &text) {
  statusBar()->showMessage(
      Casper::ServiceList::Get()->GetService(text).getDetails(), 1000 * 2);
}
