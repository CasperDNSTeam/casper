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
