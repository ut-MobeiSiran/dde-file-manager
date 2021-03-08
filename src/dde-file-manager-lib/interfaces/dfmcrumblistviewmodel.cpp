/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     Mike Chen <kegechen@gmail.com>
 *
 * Maintainer: Mike Chen <chenke_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "dfmcrumblistviewmodel.h"
#include "dtkwidget_global.h"

DWIDGET_USE_NAMESPACE

DFM_BEGIN_NAMESPACE

DFMCrumbListviewModel::DFMCrumbListviewModel(QObject *parent)
    :QStandardItemModel (parent)
{

}

DFMCrumbListviewModel::~DFMCrumbListviewModel()
{

}

void DFMCrumbListviewModel::removeAll()
{
    removeRows(0, rowCount());
}

QModelIndex DFMCrumbListviewModel::lastIndex()
{
    return index(rowCount()-1,0);
}

DFM_END_NAMESPACE
