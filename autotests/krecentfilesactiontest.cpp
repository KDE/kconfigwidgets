/* This file is part of the KDE libraries

    Copyright (c) 2015 Laurent Montel <montel@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "krecentfilesactiontest.h"
#include <krecentfilesaction.h>
#include <QMenu>
#include <qtest.h>



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
    foreach (const QAction *action, menu->actions()) {
        ret.append(action->objectName());
    }
    return ret;
}

QList<bool> KRecentFilesActionTest::extractActionEnableVisibleState(QMenu *menu)
{
    QList<bool> ret;
    foreach (const QAction *action, menu->actions()) {
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
    QCOMPARE(extractActionNames(recentAction.menu()), QStringList() << QLatin1String("no_entries") << QLatin1String("separator") << QLatin1String("clear_action"));
    QCOMPARE(extractActionEnableVisibleState(recentAction.menu()), QList<bool>()
             << false << true /*no_entries*/
             << false << false /*separator*/
             << false << false /*clear_action*/
             );
}

void KRecentFilesActionTest::shouldAddActionInTop()
{
    KRecentFilesAction recentAction(nullptr);
    recentAction.addUrl(QUrl(QStringLiteral("http://www.kde.org")));
    QList<QAction*> lstAction = recentAction.menu()->actions();
    QCOMPARE(lstAction.count(), 4);


    QCOMPARE(extractActionNames(recentAction.menu()), QStringList() << QString() << QLatin1String("no_entries") << QLatin1String("separator") << QLatin1String("clear_action"));
    QCOMPARE(extractActionEnableVisibleState(recentAction.menu()), QList<bool>()
             << true << true  /* new action*/
             << false << false /*no_entries*/
             << true << true /*separator*/
             << true << true /*clear_action*/
             );
}

void KRecentFilesActionTest::shouldClearMenu()
{
    KRecentFilesAction recentAction(nullptr);
    recentAction.addUrl(QUrl(QStringLiteral("http://www.kde.org")));
    QList<QAction*> lstAction = recentAction.menu()->actions();
    QCOMPARE(lstAction.count(), 4);
    recentAction.clear();

    lstAction = recentAction.menu()->actions();
    QCOMPARE(lstAction.count(), 3);


    QCOMPARE(extractActionNames(recentAction.menu()), QStringList() << QLatin1String("no_entries") << QLatin1String("separator") << QLatin1String("clear_action"));
    QCOMPARE(extractActionEnableVisibleState(recentAction.menu()), QList<bool>()
             << false << true /*no_entries*/
             << false << false /*separator*/
             << false << false /*clear_action*/
             );
}

QTEST_MAIN(KRecentFilesActionTest)
