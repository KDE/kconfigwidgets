/*
    Copyright (c) 2018 Harald Sitter <sitter@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QObject>
#include <QTest>

#include "klanguagename.h"

static void setEnvironment()
{
    qputenv("LANG", "C.UTF-8");
    qputenv("LANGUAGE", "en");
    qputenv("XDG_DATA_DIRS", qUtf8Printable(QFINDTESTDATA("kf5_entry_data")));
    // There is a distinct chance of the envionrment setup being to late and
    // causing flakey results based on the execution env.
    // Make sure we definitely default to english.
    QLocale::setDefault(QLocale::English);

    // NOTE
    // - fr has no translations
    // - es has no kf5_entry at all
    // - other languages under testing are complete
}

class KLanguageNameTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testListFound()
    {
        QVERIFY(KLanguageName::allLanguageCodes().count() > 0);
    }
    void testNameForCode()
    {
        // This is somewhat wrong, it should not say US.
        QCOMPARE(KLanguageName::nameForCode("en"), "US English");

        QCOMPARE(KLanguageName::nameForCode("de"), "German");
        QCOMPARE(KLanguageName::nameForCode("pt"), "Portuguese");
        QCOMPARE(KLanguageName::nameForCode("ca"), "Catalan");
    }

    void testNameForCodeInLocale()
    {
        // This is somewhat wrong, it should not say US.
        QCOMPARE(KLanguageName::nameForCodeInLocale("en", "de"), "US-Englisch");

        QCOMPARE(KLanguageName::nameForCodeInLocale("de", "de"), "Deutsch");
        QCOMPARE(KLanguageName::nameForCodeInLocale("pt", "de"), "Portugiesisch");
        QCOMPARE(KLanguageName::nameForCodeInLocale("ca", "de"), "Katalanisch");
    }

    void testNoTranslation()
    {
        // This has an entry file but no translation => QLocale.
        QCOMPARE(KLanguageName::nameForCode("fr"), "French");
        QCOMPARE(KLanguageName::nameForCodeInLocale("fr", "de"), "French");
        // When in the same language, use the native name.
        QCOMPARE(KLanguageName::nameForCodeInLocale("fr", "fr"), "français");
    }

    void testNoEntry()
    {
        // This has no entry file => QLocale.
        QCOMPARE(KLanguageName::nameForCode("es"), "Spanish");
        QCOMPARE(KLanguageName::nameForCodeInLocale("es", "de"), "Spanish");
        // When in the same language, use the native name.
        QCOMPARE(KLanguageName::nameForCodeInLocale("es", "es"), "español de España");
    }

    void testNoString()
    {
        // Qt doesn't have za support, we have no test fixture, so no string.
        QCOMPARE(KLanguageName::nameForCode("za"), QString());
    }
};

Q_COREAPP_STARTUP_FUNCTION(setEnvironment)

QTEST_GUILESS_MAIN(KLanguageNameTest)

#include "klanguagenametest.moc"
