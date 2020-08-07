/*
    SPDX-FileCopyrightText: 2019 Milian Wolff <mail@milianw.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <QObject>
#include <QTest>
#include <QAbstractItemModel>

#include "kcolorscheme.h"
#include "kcolorschememanager.h"

class KColorSchemeTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void benchConstruction_data()
    {
        KColorSchemeManager manager;
        if (manager.model()->rowCount() <= 1) {
            QSKIP("no scheme files found, cannot run benchmark");
        }

        const auto anyScheme = manager.model()->index(1, 0).data(Qt::UserRole).toString();
        QVERIFY(QFile::exists(anyScheme));

        QTest::addColumn<QString>("file");

        QTest::newRow("default") << QString();
        QTest::newRow("explicit") << anyScheme;
    }

    void benchConstruction()
    {
        QFETCH(QString, file);
        qApp->setProperty("KDE_COLOR_SCHEME_PATH", file);

        QBENCHMARK {
            KColorScheme scheme(QPalette::Active);
        }
    }
};

QTEST_MAIN(KColorSchemeTest)

#include "kcolorschemetest.moc"
