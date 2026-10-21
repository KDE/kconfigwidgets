/*
    SPDX-FileCopyrightText: 2026 Méven Car <meven@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "klanguagebutton.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QComboBox>
#include <QSignalSpy>
#include <QTest>

class KLanguageButtonTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testSortedInsertion();
    void testArrowKeysChangeLanguage();
    void testKeyboardSearchKeepsPopupOpen();
    void testPopupShowsCurrentLanguage();
};

static void fillLanguages(KLanguageButton &button)
{
    button.insertLanguage(QStringLiteral("fr"), QStringLiteral("French"));
    button.insertLanguage(QStringLiteral("de"), QStringLiteral("German"));
    button.insertLanguage(QStringLiteral("nl"), QStringLiteral("Dutch"));
    button.insertLanguage(QStringLiteral("da"), QStringLiteral("Danish"));
    button.insertLanguage(QStringLiteral("sr"), QStringLiteral("Serbian"));
}

void KLanguageButtonTest::testSortedInsertion()
{
    KLanguageButton button;
    fillLanguages(button);
    auto *combo = button.findChild<QComboBox *>();
    QVERIFY(combo);

    QStringList texts;
    for (int i = 0; i < combo->count(); ++i) {
        texts << combo->itemText(i);
    }
    QCOMPARE(texts,
             (QStringList{QStringLiteral("Danish"), QStringLiteral("Dutch"), QStringLiteral("French"), QStringLiteral("German"), QStringLiteral("Serbian")}));

    button.setCurrentItem(QStringLiteral("fr"));
    QCOMPARE(button.current(), QStringLiteral("fr"));
    QCOMPARE(combo->currentText(), QStringLiteral("French"));
}

void KLanguageButtonTest::testArrowKeysChangeLanguage()
{
    KLanguageButton button;
    fillLanguages(button);
    button.setCurrentItem(QStringLiteral("fr"));
    button.show();
    QVERIFY(QTest::qWaitForWindowExposed(&button));

    QSignalSpy activatedSpy(&button, &KLanguageButton::activated);
    QTest::keyClick(button.focusProxy(), Qt::Key_Down);
    QCOMPARE(button.current(), QStringLiteral("de"));
    QCOMPARE(activatedSpy.count(), 1);
    QCOMPARE(activatedSpy.at(0).at(0).toString(), QStringLiteral("de"));

    QTest::keyClick(button.focusProxy(), Qt::Key_Up);
    QTest::keyClick(button.focusProxy(), Qt::Key_Up);
    QCOMPARE(button.current(), QStringLiteral("nl"));
}

void KLanguageButtonTest::testKeyboardSearchKeepsPopupOpen()
{
    KLanguageButton button;
    fillLanguages(button);
    button.setCurrentItem(QStringLiteral("sr"));
    button.show();
    QVERIFY(QTest::qWaitForWindowExposed(&button));

    auto *combo = button.findChild<QComboBox *>();
    combo->showPopup();
    QAbstractItemView *view = combo->view();
    QTRY_VERIFY(view->isVisible());

    QSignalSpy activatedSpy(&button, &KLanguageButton::activated);
    QTest::keyClick(view, Qt::Key_D);
    QCOMPARE(view->currentIndex().data().toString(), QStringLiteral("Danish"));
    // A key pressed again after the search timed out goes to the next match.
    QTest::qWait(QApplication::keyboardInputInterval() + 100);
    QTest::keyClick(view, Qt::Key_D);
    QCOMPARE(view->currentIndex().data().toString(), QStringLiteral("Dutch"));
    QVERIFY(view->isVisible());
    QCOMPARE(activatedSpy.count(), 0);
    QCOMPARE(button.current(), QStringLiteral("sr"));

    QTest::keyClick(view, Qt::Key_Return);
    QCOMPARE(button.current(), QStringLiteral("nl"));
    QCOMPARE(activatedSpy.count(), 1);
}

void KLanguageButtonTest::testPopupShowsCurrentLanguage()
{
    KLanguageButton button;
    fillLanguages(button);
    button.setCurrentItem(QStringLiteral("sr"));
    button.show();
    QVERIFY(QTest::qWaitForWindowExposed(&button));

    auto *combo = button.findChild<QComboBox *>();
    combo->showPopup();
    QTRY_VERIFY(combo->view()->isVisible());
    QCOMPARE(combo->view()->currentIndex().data().toString(), QStringLiteral("Serbian"));
}

QTEST_MAIN(KLanguageButtonTest)

#include "klanguagebuttontest.moc"
