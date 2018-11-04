/*
    Copyright 2007 Simon Hausmann <hausmann@kde.org>

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

#include "kstandardactiontest.h"

#include <QAction>
#include <QtTestWidgets>

#include "kstandardaction.h"

void tst_KStandardAction::shortcutForActionId()
{
    QList<QKeySequence> stdShortcut = KStandardShortcut::shortcut(KStandardShortcut::Cut);

    QAction *cut = KStandardAction::cut(nullptr);
    QList<QKeySequence> actShortcut = cut->shortcuts();
    QCOMPARE(cut->property("defaultShortcuts").value<QList<QKeySequence> >(), actShortcut);
    QVERIFY(stdShortcut == actShortcut);
    delete cut;

    cut = KStandardAction::create(KStandardAction::Cut, nullptr, nullptr, nullptr);
    actShortcut = cut->shortcuts();
    QVERIFY(stdShortcut == actShortcut);
    delete cut;
}

class Receiver : public QObject
{
    Q_OBJECT
public:
    Receiver() : triggered(false) {}

    bool triggered;
    QUrl lastUrl;

public Q_SLOTS:
    void onTriggered()
    {
        triggered = true;
    }

    void onUrlSelected(const QUrl &url)
    {
        lastUrl = url;
    }
};

void tst_KStandardAction::testCreateNewStyle()
{
    Receiver receiver;
    QAction *action1 = KStandardAction::create(KStandardAction::Next, &receiver, &Receiver::onTriggered, &receiver);
    QVERIFY(!receiver.triggered);
    action1->trigger();
    QVERIFY(receiver.triggered);

    // check that it works with lambdas as well
    bool triggered = false;
    auto onTriggered = [&] { triggered = true; };
    QAction *action2 = KStandardAction::create(KStandardAction::Copy, &receiver, onTriggered, &receiver);
    QVERIFY(!triggered);
    action2->trigger();
    QVERIFY(triggered);

    // check ConfigureToolbars
    triggered = false;
    QAction* action3 = KStandardAction::create(KStandardAction::ConfigureToolbars, &receiver, onTriggered, &receiver);
    QVERIFY(!triggered);
    action3->trigger(); // a queued connection should be used here
    QVERIFY(!triggered);
    QCoreApplication::processEvents();
    QVERIFY(triggered);


    QUrl expectedUrl = QUrl(QStringLiteral("file:///foo/bar"));
    KRecentFilesAction *recent = KStandardAction::openRecent(&receiver, &Receiver::onUrlSelected, &receiver);
    QCOMPARE(receiver.lastUrl, QUrl());
    recent->urlSelected(expectedUrl);
    QCOMPARE(receiver.lastUrl, expectedUrl);

    // same again with lambda
    QUrl url;
    KRecentFilesAction *recent2 = KStandardAction::openRecent(&receiver, [&](const QUrl &u) { url = u; }, &receiver);
    QCOMPARE(url, QUrl());
    recent2->urlSelected(expectedUrl);
    QCOMPARE(url, expectedUrl);


    // make sure the asserts don't trigger (action has the correct type)
    KToggleAction *toggle1 = KStandardAction::showMenubar(&receiver, &Receiver::onTriggered, &receiver);
    QVERIFY(toggle1);
    KToggleAction *toggle2 = KStandardAction::showStatusbar(&receiver, &Receiver::onTriggered, &receiver);
    QVERIFY(toggle2);
    KToggleFullScreenAction *toggle3 = KStandardAction::fullScreen(&receiver, &Receiver::onTriggered, new QWidget, &receiver);
    QVERIFY(toggle3);
}

void tst_KStandardAction::testCreateOldStyle()
{
    Receiver receiver;
    QAction *action1 = KStandardAction::create(KStandardAction::Next, &receiver, SLOT(onTriggered()), &receiver);
    QVERIFY(!receiver.triggered);
    action1->trigger();
    QVERIFY(receiver.triggered);

    // check ConfigureToolbars
    receiver.triggered = false;
    QAction* action3 = KStandardAction::create(KStandardAction::ConfigureToolbars, &receiver, SLOT(onTriggered()), &receiver);
    QVERIFY(!receiver.triggered);
    action3->trigger(); // a queued connection should be used here
    QVERIFY(!receiver.triggered);
    QCoreApplication::processEvents();
    QVERIFY(receiver.triggered);


    QUrl expectedUrl = QUrl(QStringLiteral("file:///foo/bar"));
    KRecentFilesAction *recent = KStandardAction::openRecent(&receiver, SLOT(onUrlSelected(QUrl)), &receiver);
    QCOMPARE(receiver.lastUrl, QUrl());
    recent->urlSelected(expectedUrl);
    QCOMPARE(receiver.lastUrl, expectedUrl);

    // make sure the asserts don't trigger (action has the correct type)
    KToggleAction *toggle1 = KStandardAction::showMenubar(&receiver, SLOT(onTriggered()), &receiver);
    QVERIFY(toggle1);
    KToggleAction *toggle2 = KStandardAction::showStatusbar(&receiver, SLOT(onTriggered()), &receiver);
    QVERIFY(toggle2);
    KToggleFullScreenAction *toggle3 = KStandardAction::fullScreen(&receiver, SLOT(onTriggered()), new QWidget, &receiver);
    QVERIFY(toggle3);
}



QTEST_MAIN(tst_KStandardAction)

#include "kstandardactiontest.moc"
