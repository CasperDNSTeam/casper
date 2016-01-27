#include "ui_button.h"

#include <QPaintEvent>
#include <QPainter>
#include <QIcon>

class UIButton::private_button {
public:
  private_button() : m_current_action(kButtonActionInactive){
     m_icon_dict[kButtonActionInactive] =
        QIcon(":casper/artwork/casper_ui_off_button.png");
     m_icon_dict[kButtonActionActive] =
        QIcon(":casper/artwork/casper_ui_on_button.png");
  }

  ~private_button() {}

  ButtonActions m_current_action;
  QMap<ButtonActions, QIcon> m_icon_dict;
};

UIButton::UIButton(QWidget *parent)
    : QPushButton(parent), ctx(new private_button) {
}

UIButton::~UIButton() { delete ctx; }

void UIButton::setButtonActionState(UIButton::ButtonActions action) {
  ctx->m_current_action = action;
  update();
}

void UIButton::paintEvent(QPaintEvent *event) {
  QPainter context;
  context.begin(this);


  QRect rect = contentsRect();

  rect.setX((contentsRect().width() - 128) / 2);
  rect.setY((contentsRect().height() - 128) / 2);
  rect.setWidth(128.0);
  rect.setHeight(128.0);

  if (isChecked()) {
    //rect = rect.adjusted(1, 1, -1, -1);
  }

  context.save();
  context.setRenderHints(QPainter::HighQualityAntialiasing |
                     QPainter::SmoothPixmapTransform |
                     QPainter::Antialiasing
                     );

  context.drawPixmap(rect,
                 ctx->m_icon_dict[ctx->m_current_action].pixmap(256, 256,
                 isEnabled() ? QIcon::Normal : QIcon::Disabled,
                 isChecked() ? QIcon::On : QIcon::Off));
  context.restore();
  context.end();
}
