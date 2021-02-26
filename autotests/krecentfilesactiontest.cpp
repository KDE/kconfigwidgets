/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2015 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "krecentfilesactiontest.h"
#include <QMenu>
#include <QTest>
#include <krecentfilesaction.h>

KRecentFilesActionTest::KRecentFilesActionTest(QObject *parent)
    : QObject(parent)
{
}

KRecentFilesActionTest::~KRecentFilesActionTest()
{
}

QStringList KRecentFilesActionTest::extractActionNames(QMenu *menu)
{
    QStringList ret;
    const auto lstActions = menu->actions();
    for (const QAction *action : lstActions) {
        ret.append(action->objectName());
    }
    return ret;
}

QList<bool> KRecentFilesActionTest::extractActionEnableVisibleState(QMenu *menu)
{
    QList<bool> ret;
    const auto lstActions = menu->actions();
    for (const QAction *action : lstActions) {
        ret.append(action->isEnabled());
        ret.append(action->isVisible());
    }
    return ret;
}

void KRecentFilesActionTest::shouldHaveDefaultValue()
{
    KRecentFilesAction recentAction(nullptr);
    QVERIFY(recentAction.urls().isEmpty());
    QVERIFY(recentAction.menu());
    QVERIFY(!recentAction.menu()->actions().isEmpty());
    QCOMPARE(recentAction.menu()->actions().count(), 3);
    QCOMPARE(extractActionNames(recentAction.menu()),
             QStringList() << QLatin1String("no_entries") << QLatin1String("separator") << QLatin1String("clear_action"));
    QCOMPARE(extractActionEnableVisibleState(recentAction.menu()),
             QList<bool>() << false << true /*no_entries*/
                           << false << false /*separator*/
                           << false << false /*clear_action*/
    );
}

void KRecentFilesActionTest::shouldAddActionInTop()
{
    KRecentFilesAction recentAction(nullptr);
    recentAction.addUrl(QUrl(QStringLiteral("http://www.kde.org")));
    QList<QAction *> lstAction = recentAction.menu()->actions();
    QCOMPARE(lstAction.count(), 4);

    QCOMPARE(extractActionNames(recentAction.menu()),
             QStringList() << QString() << QLatin1String("no_entries") << QLatin1String("separator") << QLatin1String("clear_action"));
    QCOMPARE(extractActionEnableVisibleState(recentAction.menu()),
             QList<bool>() << true << true /* new action*/
                           << false << false /*no_entries*/
                           << true << true /*separator*/
                           << true << true /*clear_action*/
    );
}

void KRecentFilesActionTest::shouldClearMenu()
{
    KRecentFilesAction recentAction(nullptr);
    recentAction.addUrl(QUrl(QStringLiteral("http://www.kde.org")));
    QList<QAction *> lstAction = recentAction.menu()->actions();
    QCOMPARE(lstAction.count(), 4);
    recentAction.clear();

    lstAction = recentAction.menu()->actions();
    QCOMPARE(lstAction.count(), 3);

    QCOMPARE(extractActionNames(recentAction.menu()),
             QStringList() << QLatin1String("no_entries") << QLatin1String("separator") << QLatin1String("clear_action"));
    QCOMPARE(extractActionEnableVisibleState(recentAction.menu()),
             QList<bool>() << false << true /*no_entries*/
                           << false << false /*separator*/
                           << false << false /*clear_action*/
    );
}

QTEST_MAIN(KRecentFilesActionTest)
