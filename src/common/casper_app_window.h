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

#ifndef CASPER_APP_WINDOW_H
#define CASPER_APP_WINDOW_H

#include <QMainWindow>

namespace Ui {
class CasperAppWindow;
}

class CasperAppWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CasperAppWindow(QWidget *parent = 0);
    ~CasperAppWindow();

    void UpdateServiceList();
    void UpdateSystemDNS();
public slots:
    void on_activated(const QString &text);
    void on_highligted(const QString &text);
private:
    class PrivateAppContext;
    PrivateAppContext * const ctx;
};

#endif // CASPER_APP_WINDOW_H
