/*
    This file is part of the KDE project
    SPDX-FileCopyrightText: 2013 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kcolorschememanager.h"
#include "kcolorschememanager_p.h"

#include <KActionMenu>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <kcolorscheme.h>

#include <QActionGroup>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QPainter>
#include <QStandardPaths>
#include <QStyle>

constexpr int defaultSchemeRow = 0;

static void activateScheme(const QString &colorSchemePath)
{
    // hint for plasma-integration to synchronize the color scheme with the window manager/compositor
    // The property needs to be set before the palette change because is is checked upon the
    // ApplicationPaletteChange event.
    qApp->setProperty("KDE_COLOR_SCHEME_PATH", colorSchemePath);
    if (colorSchemePath.isEmpty()) {
        qApp->setPalette(KColorScheme::createApplicationPalette(KSharedConfig::Ptr(nullptr)));
    } else {
        qApp->setPalette(KColorScheme::createApplicationPalette(KSharedConfig::openConfig(colorSchemePath)));
    }
}

static QIcon createPreview(const QString &path)
{
    KSharedConfigPtr schemeConfig = KSharedConfig::openConfig(path, KConfig::SimpleConfig);
    QIcon result;

    KColorScheme activeWindow(QPalette::Active, KColorScheme::Window, schemeConfig);
    KColorScheme activeButton(QPalette::Active, KColorScheme::Button, schemeConfig);
    KColorScheme activeView(QPalette::Active, KColorScheme::View, schemeConfig);
    KColorScheme activeSelection(QPalette::Active, KColorScheme::Selection, schemeConfig);

    auto pixmap = [&](int size) {
        QPixmap pix(size, size);
        pix.fill(Qt::black);
        QPainter p;
        p.begin(&pix);
        const int itemSize = size / 2 - 1;
        p.fillRect(1, 1, itemSize, itemSize, activeWindow.background());
        p.fillRect(1 + itemSize, 1, itemSize, itemSize, activeButton.background());
        p.fillRect(1, 1 + itemSize, itemSize, itemSize, activeView.background());
        p.fillRect(1 + itemSize, 1 + itemSize, itemSize, itemSize, activeSelection.background());
        p.end();
        result.addPixmap(pix);
    };
    // 16x16
    pixmap(16);
    // 24x24
    pixmap(24);

    return result;
}

KColorSchemeManagerPrivate::KColorSchemeManagerPrivate()
    : model(new KColorSchemeModel())
{
}

KColorSchemeModel::KColorSchemeModel(QObject *parent)
    : QAbstractListModel(parent)
{
    init();
}

KColorSchemeModel::~KColorSchemeModel()
{
}

int KColorSchemeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_data.count();
}

QVariant KColorSchemeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (index.row() >= m_data.count())) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
        return m_data.at(index.row()).name;
    case Qt::DecorationRole: {
        auto &item = m_data[index.row()];
        if (item.preview.isNull()) {
            item.preview = createPreview(item.path);
        }
        return item.preview;
    }
    case Qt::UserRole:
        return m_data.at(index.row()).path;
    default:
        return QVariant();
    }
}

void KColorSchemeModel::init()
{
    beginResetModel();
    m_data.clear();

    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("color-schemes"), QStandardPaths::LocateDirectory);
    QStringList schemeFiles;
    for (const QString &dir : dirs) {
        const QStringList fileNames = QDir(dir).entryList(QStringList() << QStringLiteral("*.colors"));
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
        m_data.append(data);
    }
    std::sort(m_data.begin(), m_data.end(), [](const KColorSchemeModelData &first, const KColorSchemeModelData &second) {
        return first.name < second.name;
    });
    m_data.insert(defaultSchemeRow, {i18n("Default"), QString(), QIcon::fromTheme("edit-undo")});
    endResetModel();
}

KColorSchemeManager::KColorSchemeManager(QObject *parent)
    : QObject(parent)
    , d(new KColorSchemeManagerPrivate)
{
}

KColorSchemeManager::~KColorSchemeManager()
{
}

QAbstractItemModel *KColorSchemeManager::model() const
{
    return d->model.get();
}

QModelIndex KColorSchemeManager::indexForScheme(const QString &name) const
{
    // Empty string is mapped to "reset to the system scheme"
    if (name.isEmpty()) {
        return d->model->index(defaultSchemeRow);
    }
    for (int i = 1; i < d->model->rowCount(); ++i) {
        QModelIndex index = d->model->index(i);
        if (index.data().toString() == name) {
            return index;
        }
    }
    return QModelIndex();
}

KActionMenu *KColorSchemeManager::createSchemeSelectionMenu(const QIcon &icon, const QString &name, const QString &selectedSchemeName, QObject *parent)
{
    // Be careful here when connecting to signals. The menu can outlive the manager
    KActionMenu *menu = new KActionMenu(icon, name, parent);
    QActionGroup *group = new QActionGroup(menu);
    connect(group, &QActionGroup::triggered, qApp, [](QAction *action) {
        ::activateScheme(action->data().toString());
    });
    for (int i = 0; i < d->model->rowCount(); ++i) {
        QModelIndex index = d->model->index(i);
        QAction *action = new QAction(index.data(Qt::DisplayRole).toString(), menu);
        action->setData(index.data(Qt::UserRole));
        action->setActionGroup(group);
        action->setCheckable(true);
        if (index.data().toString() == selectedSchemeName) {
            action->setChecked(true);
        }
        menu->addAction(action);
    }
    if (!group->checkedAction()) {
        // If no (valid) color scheme has been selected we select the default one
        group->actions()[defaultSchemeRow]->setChecked(true);
    }
    group->actions()[defaultSchemeRow]->setIcon(QIcon::fromTheme("edit-undo"));
    connect(menu->menu(), &QMenu::aboutToShow, group, [group] {
        const auto actions = group->actions();
        for (QAction *action : actions) {
            if (action->icon().isNull()) {
                action->setIcon(createPreview(action->data().toString()));
            }
        }
    });

    return menu;
}

KActionMenu *KColorSchemeManager::createSchemeSelectionMenu(const QString &text, const QString &selectedSchemeName, QObject *parent)
{
    return createSchemeSelectionMenu(QIcon::fromTheme("preferences-desktop-color"), text, selectedSchemeName, parent);
}

KActionMenu *KColorSchemeManager::createSchemeSelectionMenu(const QString &selectedSchemeName, QObject *parent)
{
    return createSchemeSelectionMenu(QIcon::fromTheme("preferences-desktop-color"), i18n("Color Scheme"), selectedSchemeName, parent);
}

KActionMenu *KColorSchemeManager::createSchemeSelectionMenu(QObject *parent)
{
    return createSchemeSelectionMenu(QIcon::fromTheme("preferences-desktop-color"), i18n("Color Scheme"), QString(), parent);
}

void KColorSchemeManager::activateScheme(const QModelIndex &index)
{
    if (index.isValid() && index.model() == d->model.get()) {
        ::activateScheme(index.data(Qt::UserRole).toString());
    } else {
        ::activateScheme(QString());
    }
}
