#ifndef CASPER_UIBUTTON_H
#define CASPER_UIBUTTON_H

#include <QObject>
#include <QPushButton>

class UIButton : public QPushButton
{
    Q_OBJECT
public:
    typedef enum {
    kButtonActionInactive,
    kButtonActionActive,
    } ButtonActions;

    explicit UIButton(QWidget * parent = 0);
    virtual ~UIButton();

    void setButtonActionState(ButtonActions action);

protected:
   virtual void paintEvent(QPaintEvent *event);

private:
    class private_button;
    private_button * const ctx;
};

#endif // CASPER_UIBUTTON_H
