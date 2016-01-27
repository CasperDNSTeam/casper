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
