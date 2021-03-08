/*
 * Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     xushitong <xushitong@uniontech.com>
 *
 * Maintainer: xushitong <xushitong@uniontech.com>
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
#ifndef TRASHFILEINFO_P_H
#define TRASHFILEINFO_P_H

#include "private/dabstractfileinfo_p.h"
#include "trashfileinfo.h"
#

class TrashFileInfoPrivate : public DAbstractFileInfoPrivate
{
public:
    TrashFileInfoPrivate(const DUrl &url, TrashFileInfo *qq)
        : DAbstractFileInfoPrivate(url, qq, true)
    {
        columnCompact = false;
    }

    QString desktopIconName;
    QString displayName;
    QString originalFilePath;
    QString displayDeletionDate;
    QDateTime deletionDate;
    QStringList tagNameList;

    void updateInfo();
    void inheritParentTrashInfo();
};


#endif // TRASHFILEINFO_P_H
