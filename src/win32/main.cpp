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

#include <config.h>

// qt
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QDebug>

#ifdef Q_OS_WIN

#include <windows.h>
#include <objbase.h>
#include <atlbase.h>
#include <iostream>
#include <wbemidl.h>
#include <comutil.h>

#include <QStyleFactory>
#include <QStyle>
#include <QSysInfo>

#include <Windows.h>
#include <tchar.h>
#endif

#include <TlHelp32.h>

#include <casper_runtime.h>

bool HasAppInstance(const wchar_t *processName)
{
    bool exists = false;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    int instance_count = 0;
    if (Process32First(snapshot, &entry)) {
        while (Process32Next(snapshot, &entry)) {
            QString _qstr = QString::fromUtf8(entry.szExeFile);
            //qDebug() << Q_FUNC_INFO << _qstr;
            if (!wcsicmp(_qstr.toStdWString().c_str(), processName)) {
                //exists = true;
               instance_count++;
            }
        }
    }

    CloseHandle(snapshot);
    return (instance_count > 1);
}

Q_DECL_EXPORT int main(int argc, char *argv[]) {
    if (HasAppInstance(QString("casperdns.exe").toStdWString().c_str())) {
       return 0;
    }

    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);


   QApplication::setStyle(QStyleFactory::create("Fusion"));

   bool show_app_ui = 1;
    if (argc > 1)
       show_app_ui = 0;

    Casper::CDRunTime runtime(0, show_app_ui);
    return app.exec();
}
