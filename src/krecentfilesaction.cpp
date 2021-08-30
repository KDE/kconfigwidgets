/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
    SPDX-FileCopyrightText: 1999 Simon Hausmann <hausmann@kde.org>
    SPDX-FileCopyrightText: 2000 Nicolas Hadacek <haadcek@kde.org>
    SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>
    SPDX-FileCopyrightText: 2000 Michael Koch <koch@kde.org>
    SPDX-FileCopyrightText: 2001 Holger Freyther <freyther@kde.org>
    SPDX-FileCopyrightText: 2002 Ellis Whitehead <ellis@kde.org>
    SPDX-FileCopyrightText: 2002 Joseph Wenninger <jowenn@kde.org>
    SPDX-FileCopyrightText: 2003 Andras Mantia <amantia@kde.org>
    SPDX-FileCopyrightText: 2005-2006 Hamish Rodda <rodda@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "krecentfilesaction.h"
#include "krecentfilesaction_p.h"

#include <QActionGroup>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QMenu>
#include <QScreen>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>

KRecentFilesAction::KRecentFilesAction(QObject *parent)
    : KSelectAction(parent)
    , d_ptr(new KRecentFilesActionPrivate(this))
{
    Q_D(KRecentFilesAction);
    d->init();
}

KRecentFilesAction::KRecentFilesAction(const QString &text, QObject *parent)
    : KSelectAction(parent)
    , d_ptr(new KRecentFilesActionPrivate(this))
{
    Q_D(KRecentFilesAction);
    d->init();

    // Want to keep the ampersands
    setText(text);
}

KRecentFilesAction::KRecentFilesAction(const QIcon &icon, const QString &text, QObject *parent)
    : KSelectAction(parent)
    , d_ptr(new KRecentFilesActionPrivate(this))
{
    Q_D(KRecentFilesAction);
    d->init();

    setIcon(icon);
    // Want to keep the ampersands
    setText(text);
}

void KRecentFilesActionPrivate::init()
{
    Q_Q(KRecentFilesAction);
    delete q->menu();
    q->setMenu(new QMenu());
    q->setToolBarMode(KSelectAction::MenuMode);
    m_noEntriesAction = q->menu()->addAction(i18n("No Entries"));
    m_noEntriesAction->setObjectName(QStringLiteral("no_entries"));
    m_noEntriesAction->setEnabled(false);
    clearSeparator = q->menu()->addSeparator();
    clearSeparator->setVisible(false);
    clearSeparator->setObjectName(QStringLiteral("separator"));
    clearAction = q->menu()->addAction(i18n("Clear List"), q, &KRecentFilesAction::clear);
    clearAction->setObjectName(QStringLiteral("clear_action"));
    clearAction->setVisible(false);
    q->setEnabled(false);
    q->connect(q, qOverload<QAction *>(&KSelectAction::triggered), q, [this](QAction *action) {
        urlSelected(action);
    });
}

KRecentFilesAction::~KRecentFilesAction() = default;

void KRecentFilesActionPrivate::urlSelected(QAction *action)
{
    Q_Q(KRecentFilesAction);
    Q_EMIT q->urlSelected(m_urls[action]);
}

int KRecentFilesAction::maxItems() const
{
    Q_D(const KRecentFilesAction);
    return d->m_maxItems;
}

void KRecentFilesAction::setMaxItems(int maxItems)
{
    Q_D(KRecentFilesAction);
    // set new maxItems
    d->m_maxItems = std::max(maxItems, 0);

    // remove all excess items
    while (selectableActionGroup()->actions().count() > d->m_maxItems) {
        delete removeAction(selectableActionGroup()->actions().last());
    }
}

static QString titleWithSensibleWidth(const QString &nameValue, const QString &value)
{
    // Calculate 3/4 of screen geometry, we do not want
    // action titles to be bigger than that
    // Since we do not know in which screen we are going to show
    // we choose the min of all the screens
    int maxWidthForTitles = INT_MAX;
    const auto screens = QGuiApplication::screens();
    for (QScreen *screen : screens) {
        maxWidthForTitles = qMin(maxWidthForTitles, screen->availableGeometry().width() * 3 / 4);
    }
    const QFontMetrics fontMetrics = QFontMetrics(QFont());

    QString title = nameValue + QLatin1String(" [") + value + QLatin1Char(']');
    const int nameWidth = fontMetrics.boundingRect(title).width();
    if (nameWidth > maxWidthForTitles) {
        // If it does not fit, try to cut only the whole path, though if the
        // name is too long (more than 3/4 of the whole text) we cut it a bit too
        const int nameValueMaxWidth = maxWidthForTitles * 3 / 4;
        QString cutNameValue;
        QString cutValue;
        if (nameWidth > nameValueMaxWidth) {
            cutNameValue = fontMetrics.elidedText(nameValue, Qt::ElideMiddle, nameValueMaxWidth);
            cutValue = fontMetrics.elidedText(value, Qt::ElideMiddle, maxWidthForTitles - nameValueMaxWidth);
        } else {
            cutNameValue = nameValue;
            cutValue = fontMetrics.elidedText(value, Qt::ElideMiddle, maxWidthForTitles - nameWidth);
        }
        title = cutNameValue + QLatin1String(" [") + cutValue + QLatin1Char(']');
    }
    return title;
}

void KRecentFilesAction::addUrl(const QUrl &_url, const QString &name)
{
    Q_D(KRecentFilesAction);

    // ensure we never add items if we want none
    if (d->m_maxItems == 0) {
        return;
    }

    /**
     * Create a deep copy here, because if _url is the parameter from
     * urlSelected() signal, we will delete it in the removeAction() call below.
     * but access it again in the addAction call... => crash
     */
    const QUrl url(_url);

    if (url.isLocalFile() && url.toLocalFile().startsWith(QDir::tempPath())) {
        return;
    }
    const QString tmpName = name.isEmpty() ? url.fileName() : name;
    const QString pathOrUrl(url.toDisplayString(QUrl::PreferLocalFile));

#ifdef Q_OS_WIN
    const QString file = url.isLocalFile() ? QDir::toNativeSeparators(pathOrUrl) : pathOrUrl;
#else
    const QString file = pathOrUrl;
#endif

    // remove file if already in list
    const auto lstActions = selectableActionGroup()->actions();
    for (QAction *action : lstActions) {
        const QString urlStr = d->m_urls[action].toDisplayString(QUrl::PreferLocalFile);
#ifdef Q_OS_WIN
        const QString tmpFileName = url.isLocalFile() ? QDir::toNativeSeparators(urlStr) : urlStr;
        if (tmpFileName.endsWith(file, Qt::CaseInsensitive))
#else
        if (urlStr.endsWith(file))
#endif
        {
            removeAction(action)->deleteLater();
            break;
        }
    }
    // remove oldest item if already maxitems in list
    Q_ASSERT(d->m_maxItems > 0);
    if (selectableActionGroup()->actions().count() == d->m_maxItems) {
        // remove oldest added item
        delete removeAction(selectableActionGroup()->actions().first());
    }

    d->m_noEntriesAction->setVisible(false);
    d->clearSeparator->setVisible(true);
    d->clearAction->setVisible(true);
    setEnabled(true);
    // add file to list
    const QString title = titleWithSensibleWidth(tmpName, file);
    QAction *action = new QAction(title, selectableActionGroup());
    addAction(action, url, tmpName);
}

void KRecentFilesAction::addAction(QAction *action, const QUrl &url, const QString &name)
{
    Q_D(KRecentFilesAction);

    menu()->insertAction(menu()->actions().value(0), action);
    d->m_shortNames.insert(action, name);
    d->m_urls.insert(action, url);
}

QAction *KRecentFilesAction::removeAction(QAction *action)
{
    Q_D(KRecentFilesAction);
    KSelectAction::removeAction(action);

    d->m_shortNames.remove(action);
    d->m_urls.remove(action);

    return action;
}

void KRecentFilesAction::removeUrl(const QUrl &url)
{
    Q_D(KRecentFilesAction);
    for (QMap<QAction *, QUrl>::ConstIterator it = d->m_urls.constBegin(); it != d->m_urls.constEnd(); ++it) {
        if (it.value() == url) {
            delete removeAction(it.key());
            return;
        }
    }
}

QList<QUrl> KRecentFilesAction::urls() const
{
    Q_D(const KRecentFilesAction);
    return d->m_urls.values();
}

void KRecentFilesAction::clear()
{
    clearEntries();
    Q_EMIT recentListCleared();
}

void KRecentFilesAction::clearEntries()
{
    Q_D(KRecentFilesAction);
    KSelectAction::clear();
    d->m_shortNames.clear();
    d->m_urls.clear();
    d->m_noEntriesAction->setVisible(true);
    d->clearSeparator->setVisible(false);
    d->clearAction->setVisible(false);
    setEnabled(false);
}

void KRecentFilesAction::loadEntries(const KConfigGroup &_config)
{
    Q_D(KRecentFilesAction);
    clearEntries();

    QString key;
    QString value;
    QString nameKey;
    QString nameValue;
    QString title;
    QUrl url;

    KConfigGroup cg = _config;
    if (cg.name().isEmpty()) {
        cg = KConfigGroup(cg.config(), "RecentFiles");
    }

    bool thereAreEntries = false;
    // read file list
    for (int i = 1; i <= d->m_maxItems; i++) {
        key = QStringLiteral("File%1").arg(i);
        value = cg.readPathEntry(key, QString());
        if (value.isEmpty()) {
            continue;
        }
        url = QUrl::fromUserInput(value);

        // Don't restore if file doesn't exist anymore
        if (url.isLocalFile() && !QFile::exists(url.toLocalFile())) {
            continue;
        }

        // Don't restore where the url is already known (eg. broken config)
        auto containsUrl = std::find(d->m_urls.cbegin(), d->m_urls.cend(), url) != d->m_urls.cend();
        if (containsUrl) {
            continue;
        }

#ifdef Q_OS_WIN
        // convert to backslashes
        if (url.isLocalFile()) {
            value = QDir::toNativeSeparators(value);
        }
#endif

        nameKey = QStringLiteral("Name%1").arg(i);
        nameValue = cg.readPathEntry(nameKey, url.fileName());
        title = titleWithSensibleWidth(nameValue, value);
        if (!value.isNull()) {
            thereAreEntries = true;
            addAction(new QAction(title, selectableActionGroup()), url, nameValue);
        }
    }
    if (thereAreEntries) {
        d->m_noEntriesAction->setVisible(false);
        d->clearSeparator->setVisible(true);
        d->clearAction->setVisible(true);
        setEnabled(true);
    }
}

void KRecentFilesAction::saveEntries(const KConfigGroup &_cg)
{
    Q_D(KRecentFilesAction);
    QString key;
    QString value;
    QStringList lst = items();

    KConfigGroup cg = _cg;
    if (cg.name().isEmpty()) {
        cg = KConfigGroup(cg.config(), "RecentFiles");
    }

    cg.deleteGroup();

    // write file list
    for (int i = 1; i <= selectableActionGroup()->actions().count(); i++) {
        key = QStringLiteral("File%1").arg(i);
        // i - 1 because we started from 1
        value = d->m_urls[selectableActionGroup()->actions()[i - 1]].toDisplayString(QUrl::PreferLocalFile);
        cg.writePathEntry(key, value);
        key = QStringLiteral("Name%1").arg(i);
        value = d->m_shortNames[selectableActionGroup()->actions()[i - 1]];
        cg.writePathEntry(key, value);
    }
}

#include "moc_krecentfilesaction.cpp"
