/*
    This file is part of the KDE project
    SPDX-FileCopyrightText: 2013 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kcolorschememodel.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <kcolorscheme.h>

#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QPainter>
#include <QStandardPaths>

#include "kcolorschememanager_p.h"

struct KColorSchemeModelData {
    QString name;
    QString path;
    QIcon preview;
};

struct KColorSchemeModelPrivate {
    mutable QVector<KColorSchemeModelData> m_data;
};

KColorSchemeModel::KColorSchemeModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new KColorSchemeModelPrivate)
{
    beginResetModel();
    d->m_data.clear();

    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("color-schemes"), QStandardPaths::LocateDirectory);
    QStringList schemeFiles;
    for (const QString &dir : dirs) {
        const QStringList fileNames = QDir(dir).entryList({QStringLiteral("*.colors")});
        for (const QString &file : fileNames) {
            const QString suffixedFileName = QLatin1String("color-schemes/") + file;
            if (!schemeFiles.contains(suffixedFileName)) {
                schemeFiles.append(suffixedFileName);
            }
        }
    }
    std::transform(schemeFiles.begin(), schemeFiles.end(), schemeFiles.begin(), [](const QString &item) {
        return QStandardPaths::locate(QStandardPaths::GenericDataLocation, item);
    });
    for (const QString &schemeFile : qAsConst(schemeFiles)) {
        KSharedConfigPtr config = KSharedConfig::openConfig(schemeFile, KConfig::SimpleConfig);
        KConfigGroup group(config, QStringLiteral("General"));
        const QString name = group.readEntry("Name", QFileInfo(schemeFile).baseName());
        const KColorSchemeModelData data = {name, schemeFile, QIcon()};
        d->m_data.append(data);
    }
    std::sort(d->m_data.begin(), d->m_data.end(), [](const KColorSchemeModelData &first, const KColorSchemeModelData &second) {
        return first.name < second.name;
    });
    d->m_data.insert(0, {i18n("Default"), QString(), QIcon::fromTheme("edit-undo")});
    endResetModel();
}

KColorSchemeModel::~KColorSchemeModel() = default;

int KColorSchemeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return d->m_data.count();
}

QVariant KColorSchemeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (index.row() >= d->m_data.count())) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
        return d->m_data.at(index.row()).name;
    case Qt::DecorationRole: {
        auto &item = d->m_data[index.row()];
        if (item.preview.isNull()) {
            item.preview = KColorSchemeManagerPrivate::createPreview(item.path);
        }
        return item.preview;
    }
    case Qt::UserRole:
        return d->m_data.at(index.row()).path;
    default:
        return QVariant();
    }
}
