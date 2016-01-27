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
