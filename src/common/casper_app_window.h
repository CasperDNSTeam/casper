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
