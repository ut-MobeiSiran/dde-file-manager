/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *               2019 ~ 2019 Chris Xiong
 *
 * Author:     Chris Xiong<chirs241097@gmail.com>
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

#include "dfmrootfileinfo.h"
#include "shutil/fileutils.h"
#include "app/define.h"
#include "utils/singleton.h"
#include "controllers/pathmanager.h"

#include <dgiofile.h>
#include <dgiofileinfo.h>
#include <dgiomount.h>
#include <dgiovolume.h>
#include <dgiovolumemanager.h>
#include <ddiskmanager.h>
#include <dblockdevice.h>
#include <ddiskdevice.h>

#include <QStandardPaths>
#include <QStorageInfo>

class DFMRootFileInfoPrivate
{
public:
    QStandardPaths::StandardLocation stdloc;
    QSharedPointer<DBlockDevice> blk;
    QExplicitlySharedDataPointer<DGioMount> gmnt;
    QExplicitlySharedDataPointer<DGioFileInfo> gfsi;
    QString backer_url;
    QByteArrayList mps;
    qulonglong size;
    QString label;
    QString fs;
    DFMRootFileInfo *q_ptr;
    Q_DECLARE_PUBLIC(DFMRootFileInfo)
};

DFMRootFileInfo::DFMRootFileInfo(const DUrl &url) :
    DAbstractFileInfo(url),
    d_ptr(new DFMRootFileInfoPrivate)
{
    if (suffix() == SUFFIX_USRDIR) {
        QStandardPaths::StandardLocation loc = QStandardPaths::StandardLocation::HomeLocation;
        if (baseName() == "desktop") {
            loc = QStandardPaths::StandardLocation::DesktopLocation;
        } else if (baseName() == "videos") {
            loc = QStandardPaths::StandardLocation::MoviesLocation;
        } else if (baseName() == "music") {
            loc = QStandardPaths::StandardLocation::MusicLocation;
        } else if (baseName() == "pictures") {
            loc = QStandardPaths::StandardLocation::PicturesLocation;
        } else if (baseName() == "documents") {
            loc = QStandardPaths::StandardLocation::DocumentsLocation;
        } else if (baseName() == "downloads") {
            loc = QStandardPaths::StandardLocation::DownloadLocation;
        }

        d_ptr->stdloc = loc;
        if (QStandardPaths::writableLocation(loc) == QStandardPaths::writableLocation(QStandardPaths::StandardLocation::HomeLocation)) {
            d_ptr->backer_url = "";
        }

        FileUtils::mkpath(DUrl::fromLocalFile(QStandardPaths::writableLocation(loc)));

        d_ptr->backer_url = QStandardPaths::writableLocation(loc);
    } else if (suffix() == SUFFIX_GVFSMP) {
        QString mpp = QUrl::fromPercentEncoding(fileUrl().path().mid(1).chopped(QString("." SUFFIX_GVFSMP).length()).toUtf8());
        QExplicitlySharedDataPointer<DGioMount> mp(DGioMount::createFromPath(mpp));
        if (mp->getRootFile()->createFileInfo()->fileType() == DGioFileType::FILE_TYPE_DIRECTORY) {
            QString mpurl = mp->getRootFile()->path();
            d_ptr->backer_url = mpurl;
            d_ptr->gmnt = mp;
            d_ptr->gfsi = mp->getRootFile()->createFileSystemInfo();
        }
    } else if (suffix() == SUFFIX_UDISKS) {
        QString udiskspath = "/org/freedesktop/UDisks2/block_devices" + url.path().chopped(QString("." SUFFIX_UDISKS).length());
        QSharedPointer<DBlockDevice> blk(DDiskManager::createBlockDevice(udiskspath));
        if (blk->path().length() != 0) {
            d_ptr->backer_url = udiskspath;
            d_ptr->blk = blk;
            d_ptr->blk->setWatchChanges(true);
            checkCache();
            QObject::connect(d_ptr->blk.data(), &DBlockDevice::idLabelChanged, [this] {this->checkCache();});
            QObject::connect(d_ptr->blk.data(), &DBlockDevice::mountPointsChanged, [this] {this->checkCache();});
            QObject::connect(d_ptr->blk.data(), &DBlockDevice::sizeChanged, [this] {this->checkCache();});
            QObject::connect(d_ptr->blk.data(), &DBlockDevice::idTypeChanged, [this] {this->checkCache();});
        }
    }
}

bool DFMRootFileInfo::exists() const
{
    Q_D(const DFMRootFileInfo);
    if (suffix() == SUFFIX_USRDIR) {
        return d->backer_url.length() != 0;
    } else if (suffix() == SUFFIX_GVFSMP) {
        return d->gmnt->getRootFile()->createFileInfo()->fileType() == DGioFileType::FILE_TYPE_DIRECTORY;
    } else if (suffix() == SUFFIX_UDISKS) {
        return d->blk->path().length() != 0;
    }
}

QString DFMRootFileInfo::suffix() const
{
    QRegularExpression re(".*\\.(.*)$");
    auto rem = re.match(fileName());
    if (!rem.hasMatch()) {
        return "";
    }
    return rem.captured(1);
}

QString DFMRootFileInfo::fileDisplayName() const
{
    Q_D(const DFMRootFileInfo);

    static QMap<QString, const char*> i18nMap {
        {"data", "Data Disk"}
    };
    const QString ddeI18nSym = QStringLiteral("_dde_");

    if (suffix() == SUFFIX_USRDIR) {
        return QStandardPaths::displayName(d->stdloc);
    } else if (suffix() == SUFFIX_GVFSMP) {
        return d->gmnt->name();
    } else if (suffix() == SUFFIX_UDISKS) {
        if (d->label.startsWith(ddeI18nSym)) {
            QString i18nKey = d->label.mid(ddeI18nSym.size(), d->label.size() - ddeI18nSym.size());
            return qApp->translate("DeepinStorage", i18nMap.value(i18nKey, i18nKey.toUtf8().constData()));
        }

        if (d->mps.size() == 1 && d->mps.front() == QString("/"))
            return QCoreApplication::translate("PathManager", "System Disk");
        if (d->label.length() == 0)
            return QCoreApplication::translate("DeepinStorage", "%1 Volume").arg(FileUtils::formatSize(d->size));
        return d->label;
    }
    return baseName();
}

bool DFMRootFileInfo::canRename() const
{
    Q_D(const DFMRootFileInfo);
    if (suffix() != SUFFIX_UDISKS || !d->blk) {
        return false;
    }
    if (d->blk->readOnly() || d->mps.size() > 0) {
        return false;
    }
    return true;
}

bool DFMRootFileInfo::canShare() const
{
    return false;
}

bool DFMRootFileInfo::canFetch() const
{
    return false;
}

bool DFMRootFileInfo::isReadable() const
{
    return true;
}

bool DFMRootFileInfo::isWritable() const
{
    return false;
}

bool DFMRootFileInfo::isExecutable() const
{
    return false;
}

bool DFMRootFileInfo::isHidden() const
{
    return false;
}

bool DFMRootFileInfo::isRelative() const
{
    return false;
}

bool DFMRootFileInfo::isAbsolute() const
{
    return true;
}

bool DFMRootFileInfo::isShared() const
{
    return false;
}

bool DFMRootFileInfo::isTaged() const
{
    return false;
}

bool DFMRootFileInfo::isWritableShared() const
{
    return false;
}

bool DFMRootFileInfo::isAllowGuestShared() const
{
    return false;
}

bool DFMRootFileInfo::isFile() const
{
    return true;
}

bool DFMRootFileInfo::isDir() const
{
    return false;
}

DAbstractFileInfo::FileType DFMRootFileInfo::fileType() const
{
    Q_D(const DFMRootFileInfo);

    ItemType ret;

    if (suffix() == SUFFIX_USRDIR) {
        ret = ItemType::UserDirectory;
    } else if (suffix() == SUFFIX_GVFSMP) {
        ret = ItemType::GvfsMount;
    } else if (d->mps.size() == 1 && d->mps.front() == QString("/")) {
        ret = ItemType::UDisksRoot;
    } else if (d->label == "_dde_data") {
        ret = ItemType::UDisksData;
    } else {
        ret = ItemType::UDisksNormal;
    }
    return static_cast<FileType>(ret);
}

int DFMRootFileInfo::filesCount() const
{
    return 0;
}

QString DFMRootFileInfo::iconName() const
{
    Q_D(const DFMRootFileInfo);
    if (suffix() == SUFFIX_USRDIR) {
        return systemPathManager->getSystemPathIconNameByPath(redirectedFileUrl().path());
    } else if (suffix() == SUFFIX_GVFSMP) {
        return d->gmnt->themedIconNames().front();
    } else if (suffix() == SUFFIX_UDISKS) {
        if (d->blk) {
            QScopedPointer<DDiskDevice> drv(DDiskManager::createDiskDevice(d->blk->drive()));
            if (drv->mediaCompatibility().join(" ").contains("optical")) {
                return "media-optical";
            }
            if (drv->mediaRemovable()) {
                return "drive-removable-media";
            }
            for (auto mp : d->blk->mountPoints()) {
                if (QString(mp) == "/") {
                    return "drive-harddisk-root";
                }
            }
        }
        return "drive-harddisk";
    }
    return "";
}

QVector<MenuAction> DFMRootFileInfo::menuActionList(DAbstractFileInfo::MenuType type) const
{
    Q_D(const DFMRootFileInfo);
    QVector<MenuAction> ret;
    if (suffix() == SUFFIX_USRDIR) {
        ret.push_back(MenuAction::OpenInNewWindow);
        ret.push_back(MenuAction::OpenInNewTab);
    } else {
        ret.push_back(MenuAction::OpenDiskInNewWindow);
        ret.push_back(MenuAction::OpenDiskInNewTab);
    }

    ret.push_back(MenuAction::Separator);

    QScopedPointer<DDiskDevice> drv(DDiskManager::createDiskDevice(d->blk ? d->blk->drive() : ""));
    if (suffix() == SUFFIX_GVFSMP || (suffix() == SUFFIX_UDISKS && d->blk && d->blk->mountPoints().size() > 0 && !d->blk->hintSystem())) {
        //!!TODO (needs clarification): Unmount, Eject, SafelyRemove?
        ret.push_back(MenuAction::Unmount);
    }

    if (suffix() == SUFFIX_UDISKS && d->blk && d->blk->mountPoints().empty()) {
        ret.push_back(MenuAction::Mount);
        if (!d->blk->readOnly()) {
            ret.push_back(MenuAction::Rename);
            ret.push_back(MenuAction::FormatDevice);
        }
        if (drv->optical() && drv->media().contains(QRegularExpression("_r(w|e)"))) {
            ret.push_back(MenuAction::OpticalBlank);
        }
    }

    if (suffix() == SUFFIX_GVFSMP) {
        QString scheme = QUrl(d->gmnt->getRootFile()->uri()).scheme();
        if (QSet<QString>({SMB_SCHEME, FTP_SCHEME, SFTP_SCHEME, DAV_SCHEME}).contains(scheme)) {
            ret.push_back(MenuAction::ForgetPassword);
        }
    }

    ret.push_back(MenuAction::Separator);

    ret.push_back(MenuAction::Property);
    return ret;
}

bool DFMRootFileInfo::canRedirectionFileUrl() const
{
    return true;
}

DUrl DFMRootFileInfo::redirectedFileUrl() const
{
    Q_D(const DFMRootFileInfo);
    if (suffix() == SUFFIX_USRDIR) {
        return DUrl::fromLocalFile(d->backer_url);
    } else if (suffix() == SUFFIX_GVFSMP) {
        return DUrl::fromLocalFile(d->backer_url);
    } else if (suffix() == SUFFIX_UDISKS) {
        QScopedPointer<DDiskDevice> drv(DDiskManager::createDiskDevice(d->blk->drive()));
        if (d->blk->mountPoints().size()) {
            if (drv->optical()) {
                return DUrl::fromBurnFile(QString(d->blk->device()) + "/" + BURN_SEG_ONDISC + "/");
            }
            return DUrl::fromLocalFile(d->blk->mountPoints().first());
        }
    }
    return DUrl();
}

bool DFMRootFileInfo::canDrop() const
{
    return false;
}

Qt::DropActions DFMRootFileInfo::supportedDragActions() const
{
    return Qt::DropAction::IgnoreAction;
}

Qt::DropActions DFMRootFileInfo::supportedDropActions() const
{
    return Qt::DropAction::IgnoreAction;
}

QVariantHash DFMRootFileInfo::extraProperties() const
{
    Q_D(const DFMRootFileInfo);
    QVariantHash ret;
    if (suffix() == SUFFIX_GVFSMP) {
        ret["fsUsed"] = d->gfsi->fsUsedBytes();
        ret["fsSize"] = d->gfsi->fsTotalBytes();
        ret["fsType"] = d->gfsi->fsType();
        ret["rooturi"] = d->gmnt->getRootFile()->uri();
    } else if (suffix() == SUFFIX_UDISKS) {
        if (d->mps.empty()) {
            ret["fsUsed"] = quint64(d->size + 1);
        } else {
            QStorageInfo si(d->mps.front());
            ret["fsUsed"] = quint64(si.bytesTotal() - si.bytesFree());
        }
        ret["fsSize"] = quint64(d->size);
        ret["fsType"] = d->fs;
        ret["udisksblk"] = d->blk->path();
    }
    return ret;
}

void DFMRootFileInfo::checkCache()
{
    Q_D(DFMRootFileInfo);
    if (!d->blk) {
        return;
    }
    d->mps = d->blk->mountPoints();
    d->size = d->blk->size();
    d->label = d->blk->idLabel();
    d->fs = d->blk->idType();
}
