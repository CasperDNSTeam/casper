#ifndef CASPER_CONFIGURE_DIALOG_H
#define CASPER_CONFIGURE_DIALOG_H

#include <service_list.h>
#include <QDialog>

namespace Ui {
  class ConfigureDialog;
}

class ConfigureDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ConfigureDialog(QWidget *parent = 0);
  ~ConfigureDialog();

  void LoadServiceData(Casper::Service service);
  void LoadActiveService();
private:
  class ConfigureDialogContext;

  ConfigureDialogContext * const ctx;

};

#endif // CASPER_CONFIGURE_DIALOG_H
