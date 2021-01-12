/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     Gary Wang <wzc782970009@gmail.com>
 *
 * Maintainer: Gary Wang <wangzichong@deepin.com>
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

#include "dfmsidebarmanager.h"

#include "interfaces/dfmsidebariteminterface.h"
#include "controllers/dfmsidebardefaultitemhandler.h"
#include "controllers/dfmsidebardeviceitemhandler.h"
#include "controllers/dfmsidebartagitemhandler.h"
#include "controllers/dfmsidebarbookmarkitemhandler.h"
#include "controllers/dfmsidebarvaultitemhandler.h"

DFM_BEGIN_NAMESPACE

class DFMSideBarManagerPrivate
{
public:
    explicit DFMSideBarManagerPrivate(DFMSideBarManager *qq);

    QMultiHash<const DFMSideBarManager::KeyType, DFMSideBarManager::SideBarInterfaceCreaterType> controllerCreatorHash;

    DFMSideBarManager *q_ptr;
};

DFMSideBarManagerPrivate::DFMSideBarManagerPrivate(DFMSideBarManager *qq)
    : q_ptr(qq)
{

}

DFMSideBarManager *DFMSideBarManager::instance()
{
    static DFMSideBarManager manager;

    return &manager;
}

bool DFMSideBarManager::isRegisted(const QString &scheme, const std::type_info &info) const
{
    Q_D(const DFMSideBarManager);

    const KeyType &type = KeyType(scheme);

    foreach (const SideBarInterfaceCreaterType &value, d->controllerCreatorHash.values(type)) {
        if (value.first == info.name())
            return true;
    }

    return false;
}

DFMSideBarItemInterface *DFMSideBarManager::createByIdentifier(const QString &identifier)
{
    Q_D(const DFMSideBarManager);

    KeyType theType(identifier);

    const QList<SideBarInterfaceCreaterType> creatorList = d->controllerCreatorHash.values(theType);

    if (!creatorList.isEmpty()) {
        DFMSideBarItemInterface *i = (creatorList.first().second)();
        return i;
    }

    return nullptr;
}

DFMSideBarManager::DFMSideBarManager(QObject *parent)
    : QObject(parent)
    , d_private(new DFMSideBarManagerPrivate(this))
{
    // register built-in
    dRegisterSideBarInterface<DFMSideBarItemInterface>(QStringLiteral(SIDEBAR_ID_INTERNAL_FALLBACK));
    dRegisterSideBarInterface<DFMSideBarDefaultItemHandler>(QStringLiteral(SIDEBAR_ID_DEFAULT));
    dRegisterSideBarInterface<DFMSideBarDeviceItemHandler>(QStringLiteral(SIDEBAR_ID_DEVICE));
    dRegisterSideBarInterface<DFMSideBarTagItemHandler>(QStringLiteral(SIDEBAR_ID_TAG));
    dRegisterSideBarInterface<DFMSideBarBookmarkItemHandler>(QStringLiteral(SIDEBAR_ID_BOOKMARK));
    dRegisterSideBarInterface<DFMSideBarVaultItemHandler>(QStringLiteral(SIDEBAR_ID_VAULT));
}

DFMSideBarManager::~DFMSideBarManager()
{

}

void DFMSideBarManager::insertToCreatorHash(const DFMSideBarManager::KeyType &type, const SideBarInterfaceCreaterType &creator)
{
    Q_D(DFMSideBarManager);

    d->controllerCreatorHash.insertMulti(type, creator);
}

DFM_END_NAMESPACE
