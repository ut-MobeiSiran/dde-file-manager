/*
* Copyright (C) 2020 ~ 2020 Deepin Technology Co., Ltd.
*
* Author: Liu Zhangjian<liuzhangjian@uniontech.com>
*
* Maintainer: Liu Zhangjian<liuzhangjian@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "checkbox.h"

#include <QKeyEvent>

CheckBox::CheckBox(QWidget *parent)
    : QCheckBox (parent)
{

}

CheckBox::CheckBox(const QString &text, QWidget *parent)
    : QCheckBox (parent)
{
    this->setText(text);
}

void CheckBox::keyPressEvent(QKeyEvent *event)
{
    // 不要加event->key() == Qt::Key_Space，QCheckBox默认就是空格勾选
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return /*|| event->key() == Qt::Key_Space*/){
        if (this->isChecked()){
            this->setChecked(false);
            emit clicked(false);
        }else {
            this->setChecked(true);
            emit clicked(true);
        }
    }

    QCheckBox::keyPressEvent(event);
}
