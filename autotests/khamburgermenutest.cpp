/*
    SPDX-FileCopyrightText: 2023 Felix Ernst <felixernst@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QObject>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

#include "khamburgermenu.h"

class KHamburgerMenuTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void init();
    void visibilityTest();
    void menuContentsTest();

private:
    QScopedPointer<QMainWindow> m_mainWindow;
    QPointer<QMenuBar> m_menuBar;
    QPointer<QToolBar> m_toolBar;
};

void KHamburgerMenuTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
}

void KHamburgerMenuTest::init()
{
    m_mainWindow.reset(new QMainWindow());
    m_toolBar = new QToolBar(m_mainWindow.get());
    m_mainWindow->addToolBar(m_toolBar);
    m_menuBar = m_mainWindow->menuBar();
    m_menuBar->addAction(QStringLiteral("File"));
    m_menuBar->addAction(QStringLiteral("Help"));
    m_mainWindow->show();
}

/**
 * Tests whether the hamburger button actually hides when it is redundant.
 * Also makes sure that the "Menu" action, that can be added to context menus in situations in which no other menus are available,
 * only appears in such situations.
 * A special focus is set on making sure that the aboutToShowMenu() signal is emitted correctly to avoid premature population of menus.
 */
void KHamburgerMenuTest::visibilityTest()
{
    KHamburgerMenu *hamburgerMenuAction = new KHamburgerMenu(m_mainWindow.get());
    QSignalSpy aboutToShowMenuSignalsSpy(hamburgerMenuAction, &KHamburgerMenu::aboutToShowMenu);
    m_toolBar->addAction(hamburgerMenuAction);
    QToolButton *hamburgerToolButton = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(hamburgerMenuAction));
    QVERIFY(hamburgerToolButton);
    QTRY_VERIFY(hamburgerMenuAction->isVisible() && hamburgerToolButton->isVisible());

    /* 1. Test visibility interactions with menu bar */

    hamburgerMenuAction->setMenuBar(m_menuBar);
    QTRY_VERIFY2(!hamburgerMenuAction->isVisible() && !hamburgerToolButton->isVisible(), "KHamburgerMenu hides when there is a visible menu bar.");

    m_menuBar->hide();
    QTRY_VERIFY2(hamburgerMenuAction->isVisible() && hamburgerToolButton->isVisible(), "KHamburgerMenu re-appears when the menu bar is hidden.");

    /* 2. Test visibility interactions of the context menu action */

    QMenu *contextMenu = new QMenu(m_mainWindow.get());
    hamburgerMenuAction->addToMenu(contextMenu);
    QCOMPARE(contextMenu->actions().count(), 1);
    contextMenu->addAction("test");
    QMenu *hamburgerMenuMenu = new QMenu(m_mainWindow.get());
    hamburgerMenuMenu->addAction("testActionInsideTheHamburgerMenu");
    hamburgerMenuAction->setMenu(hamburgerMenuMenu);

    m_menuBar->show();
    hamburgerMenuAction->addToMenu(contextMenu);
    contextMenu->popup(QPoint());
    QVERIFY2(!contextMenu->actions().last()->isVisible(), "The context menu action should be invisible because the menu bar is visible.");

    m_menuBar->hide();
    hamburgerMenuAction->addToMenu(contextMenu);
    contextMenu->popup(QPoint());
    QVERIFY2(!contextMenu->actions().last()->isVisible(), "The context menu action should be invisible because KHamburgerMenu is visible in the toolbar.");

    m_menuBar->hide();
    m_toolBar->hide();
    hamburgerMenuAction->addToMenu(contextMenu);
    QCOMPARE(aboutToShowMenuSignalsSpy.count(), 0);
    contextMenu->popup(QPoint());
    QVERIFY2(contextMenu->actions().last()->isVisible(), "The context menu action should be visible because there is no other menu anywhere.");
    QVERIFY2(aboutToShowMenuSignalsSpy.count() > 0,
             "The hamburger menu within the context menu should already be populated at this point, "
             "so at least one aboutToShowMenu() signal should have been emitted.");

    const int count = aboutToShowMenuSignalsSpy.count();
    m_toolBar->show();
    hamburgerMenuAction->addToMenu(contextMenu);
    contextMenu->popup(QPoint());
    QVERIFY2(!contextMenu->actions().last()->isVisible(),
             "The context menu action should be hidden because the toolbar containing the hamburger menu is "
             "now visible.");
    QCOMPARE(aboutToShowMenuSignalsSpy.count(), count); // There is no reason why it would have changed.

    /* 3. Test visibility interactions when there are multiple buttons for one KHamburgerMenu */

    auto secondToolBar = new QToolBar(m_mainWindow.get());
    m_mainWindow->addToolBar(Qt::BottomToolBarArea, secondToolBar);
    secondToolBar->addAction(hamburgerMenuAction);
    QToolButton *secondHamburgerToolButton = qobject_cast<QToolButton *>(secondToolBar->widgetForAction(hamburgerMenuAction));
    QTRY_VERIFY(secondHamburgerToolButton->isVisible());

    m_menuBar->show();
    QTRY_VERIFY(!secondHamburgerToolButton->isVisible());

    m_menuBar->hide();
    QTRY_VERIFY(secondHamburgerToolButton->isVisible());

    m_toolBar->hide();
    hamburgerMenuAction->addToMenu(contextMenu);
    contextMenu->popup(QPoint());
    QVERIFY2(!contextMenu->actions().last()->isVisible(),
             "The context menu action should be hidden because the secondToolBar containing the hamburger menu "
             "action is now visible.");

    secondToolBar->hide();
    hamburgerMenuAction->addToMenu(contextMenu);
    QCOMPARE(aboutToShowMenuSignalsSpy.count(), count); // There is no reason why it would have changed.
    contextMenu->popup(QPoint());
    QVERIFY2(contextMenu->actions().last()->isVisible(), "The context menu action should be visible because there is no other menu anywhere.");
    QVERIFY(aboutToShowMenuSignalsSpy.count() > count); // It changed for the previous popup() because there actually was a menu in there that needed updating.
}

void openAndCloseToolButton(QToolButton *button)
{
    QTimer::singleShot(100, [button]() {
        button->menu()->close();
    }); // This seems to be the only way because code that isn't already set in motion through a timer doesn't get executed after the menu pops up.

    QTest::mouseClick(button, Qt::LeftButton);
}

/**
 * Tests whether the populating of the hamburger menu happens correctly based on the provided information.
 * A special focus is set on making sure that the aboutToShowMenu() signal is emitted correctly to avoid premature population of menus.
 * We don't want those menus to be populated until the aboutToShowMenu() has been emitted, so the populating code is not triggered until needed.
 */
void KHamburgerMenuTest::menuContentsTest()
{
    KHamburgerMenu *hamburgerMenuAction = new KHamburgerMenu(m_mainWindow.get());
    QSignalSpy aboutToShowMenuSignalsSpy(hamburgerMenuAction, &KHamburgerMenu::aboutToShowMenu);
    m_toolBar->addAction(hamburgerMenuAction);
    QToolButton *hamburgerToolButton = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(hamburgerMenuAction));
    QVERIFY(hamburgerToolButton);
    QVERIFY2(!hamburgerToolButton->menu() || hamburgerToolButton->menu()->isEmpty(),
             "The hamburger menu button is not supposed to have a menu until it is "
             "pressed the first time.");

    m_menuBar->hide();
    hamburgerMenuAction->setMenuBar(m_menuBar);
    openAndCloseToolButton(hamburgerToolButton);
    QCOMPARE(aboutToShowMenuSignalsSpy.count(), 1);
    // The hamburger menu now contains the two menu bar actions "File" and "Help".
    QVERIFY(hamburgerToolButton->menu());
    QVERIFY(!hamburgerToolButton->menu()->isEmpty());
    QCOMPARE(hamburgerToolButton->menu()->actions().count(), 2);

    QMenu *hamburgerMenuMenu = new QMenu(m_mainWindow.get());
    hamburgerMenuMenu->addAction("testActionInsideTheHamburgerMenu");
    hamburgerMenuAction->setMenu(hamburgerMenuMenu);
    QCOMPARE(hamburgerToolButton->menu()->actions().count(), 2); // The menu actions are not updated until the menu is opened again.

    openAndCloseToolButton(hamburgerToolButton);
    QCOMPARE(aboutToShowMenuSignalsSpy.count(), 2);
    QVERIFY(!hamburgerToolButton->menu()->isEmpty());
    QCOMPARE(hamburgerToolButton->menu()->actions().count(), 4); // The hamburger menu now contains
    // testActionInsideTheHamburgerMenu, the help menu, a separator and the menuBarAdvertisementsMenu.

    hamburgerMenuMenu->addAction("testActionInsideTheHamburgerMenu2");
    QCOMPARE(hamburgerToolButton->menu()->actions().count(), 4); // The menu actions are not updated until the menu is opened again.

    openAndCloseToolButton(hamburgerToolButton);
    QCOMPARE(aboutToShowMenuSignalsSpy.count(), 3);
    QVERIFY(!hamburgerToolButton->menu()->isEmpty());
    QCOMPARE(hamburgerToolButton->menu()->actions().count(), 5); // The hamburger menu now contains
    // testActionInsideTheHamburgerMenu, testActionInsideTheHamburgerMenu2, the help menu, a separator and the menuBarAdvertisementsMenu.

    hamburgerToolButton->menu()->actions().last()->menu()->aboutToShow();
    QCOMPARE(hamburgerToolButton->menu()->actions().last()->menu()->actions().count(), 2); // The "More" menu also known as the "menuBarAdvertisementMenu"
    // contains the a section header and the "File" menu.

    hamburgerMenuAction->setShowMenuBarAction(new QAction(QStringLiteral("Show Menubar"), m_mainWindow.get()));
    m_menuBar->removeAction(m_menuBar->actions().first()); // Remove "File" menu.
    openAndCloseToolButton(hamburgerToolButton);
    hamburgerToolButton->menu()->actions().last()->menu()->aboutToShow();
    QCOMPARE(hamburgerToolButton->menu()->actions().last()->menu()->actions().count(), 2); // The "More" menu also known as the "menuBarAdvertisementMenu"
    // contains the "Show Menubar"-action and an invisible section header.

    m_menuBar->addAction(new QAction(QStringLiteral("Edit"), m_mainWindow.get()));
    openAndCloseToolButton(hamburgerToolButton);
    hamburgerToolButton->menu()->actions().last()->menu()->aboutToShow();
    QCOMPARE(hamburgerToolButton->menu()->actions().last()->menu()->actions().count(),
             3); // The "More" menu also known as the "menuBarAdvertisementMenu" contains the "Show Menubar"-action, a section header and an "Edit" action.
}

QTEST_MAIN(KHamburgerMenuTest)

#include "khamburgermenutest.moc"
