#include "casper_configure_dialog.h"
#include "ui_configuredialog.h"

#include <QListView>
#include <QStringListModel>
#include <QDebug>
#include <service_list.h>

class ConfigureDialog::ConfigureDialogContext {
public:
  ConfigureDialogContext() : ui(new Ui::ConfigureDialog) {}
  ~ConfigureDialogContext() { delete ui; }

  void RestForm();

  Ui::ConfigureDialog *ui;
  QStringListModel *m_model;
};

void ConfigureDialog::LoadServiceData(Casper::Service service) {
  ctx->ui->service->setText(service.getName());
  ctx->ui->details->setText(service.getDetails());
  ctx->ui->p_dns->setText(service.getPrimary());
  ctx->ui->s_dns->setText(service.getSecondry());
  ctx->ui->url->setText(service.getUrl());
}

void ConfigureDialog::LoadActiveService()
{
  ctx->ui->list->setCurrentIndex(ctx->m_model->index(0, 0));
  ctx->ui->list->selectionModel()->setCurrentIndex(ctx->m_model->index(0, 0),
                                                   QItemSelectionModel::Select);
  QString s_name = ctx->m_model->data(ctx->m_model->index(0, 0), 0).toString();
  Casper::Service s = Casper::ServiceList::Get()->GetService(s_name);
  LoadServiceData(s);
}

ConfigureDialog::ConfigureDialog(QWidget *parent)
    : QDialog(parent), ctx(new ConfigureDialogContext) {
  ctx->ui->setupUi(this);

  ctx->m_model = new QStringListModel(this);
  ctx->ui->list->setSelectionMode(QAbstractItemView::SingleSelection);

  QStringList data;
  foreach (const Casper::Service &service,
           Casper::ServiceList::Get()->GetCurrentServiceList()) {
    qDebug() << Q_FUNC_INFO << service.getName();
    if (service.getName().isEmpty())
      continue;
    data << service.getName();
  }
  ctx->m_model->setStringList(data);
  ctx->ui->list->setModel(ctx->m_model);

  connect(ctx->ui->list, &QListView::clicked, this, [this](QModelIndex index) {
    QString key = ctx->m_model->data(index, Qt::DisplayRole).toString();
    qDebug() << Q_FUNC_INFO << key;

    Casper::Service service = Casper::ServiceList::Get()->GetService(key);

    if (service.getName().isEmpty()) {
      return;
    }

    LoadServiceData(service);
  });

  connect(ctx->ui->add, &QPushButton::clicked, this,
          [this](bool) { ctx->RestForm(); });

  connect(ctx->ui->remove, &QPushButton::clicked, this, [this](bool) {
    QString name = ctx->ui->list->currentIndex().data().toString();
    qDebug() << Q_FUNC_INFO << name;
    Casper::Service service;
    service = Casper::ServiceList::Get()->GetService(name);

    if (service.getName().isEmpty())
      return;

    Casper::ServiceList::Get()->DeleteService(name);

    QStringList _data = ctx->m_model->stringList();
    if (_data.contains(service.getName())) {
      _data.removeAll(name);
      qDebug() << Q_FUNC_INFO << name;
      ctx->m_model->setStringList(_data);
    }
    LoadActiveService();
  });

  connect(ctx->ui->save, &QPushButton::clicked, this, [this](bool) {
    Casper::Service service;
    service.setName(ctx->ui->service->text());
    service.setDetails(ctx->ui->details->text());
    service.setPrimary(ctx->ui->p_dns->text());
    service.setSecondry(ctx->ui->s_dns->text());
    service.setUrl(ctx->ui->url->text());
    Casper::ServiceList::Get()->CreateService(service);

    QStringList _data = ctx->m_model->stringList();
    if (!_data.contains(service.getName())) {
      _data << service.getName();
      ctx->m_model->setStringList(_data);
    }
  });

  // load current active;
  LoadActiveService();
}

ConfigureDialog::~ConfigureDialog() { delete ctx; }

void ConfigureDialog::ConfigureDialogContext::RestForm() {
  ui->service->setText("");
  ui->details->setText("");
  ui->p_dns->setText("0.0.0.0");
  ui->s_dns->setText("0.0.0.0");
  ui->url->setText("");
}
