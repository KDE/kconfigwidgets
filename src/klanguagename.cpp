/*
 * Copyright (c) 1999-2003 Hans Petter Bieker <bieker@kde.org>
 *           (c) 2007      David Jarvie <software@astrojar.org.uk>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "klanguagename.h"
#include "kconfigwidgets_debug.h"

#include <KConfig>
#include <KConfigGroup>

#include <QDir>

QString KLanguageName::nameForCode(const QString &code)
{
    const QStringList parts = QLocale().name().split(QLatin1Char('_'));
    return nameForCodeInLocale(code, parts.at(0));
}

QString KLanguageName::nameForCodeInLocale(const QString &code, const QString &outputCode)
{
    const QString realCode = code == QLatin1String("en") ? QStringLiteral("en_US") : code;
    const QString realOutputCode = outputCode == QLatin1String("en") ? QStringLiteral("en_US") : outputCode;

    const QString entryFile =
            QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                   QStringLiteral("locale") + QLatin1Char('/') + realCode + QStringLiteral("/kf5_entry.desktop"));
    if (!entryFile.isEmpty()) {
        KConfig entry(entryFile, KConfig::SimpleConfig);
        entry.setLocale(realOutputCode);
        const KConfigGroup group(&entry, "KCM Locale");
        const QString name = group.readEntry("Name");

        // KConfig doesn't have a way to say it didn't find the entry in
        // realOutputCode and returned the english version so we check for the
        // english version, if it's equal to the "non-english" version we'll use
        // otherwise we defer to QLocale.
        entry.setLocale("en_US");
        const QString englishName = group.readEntry("Name");
        if (name != englishName || realOutputCode == QLatin1String("en_US")) {
            return name;
        }
    }

    const QLocale locale(realCode);
    if (locale != QLocale::c()) {
        if (realCode == realOutputCode) {
            return locale.nativeLanguageName();
        }
        return QLocale::languageToString(locale.language());
    }

    return QString();
}

QStringList KLanguageName::allLanguageCodes()
{
    QStringList systemLangList;
    const QString localeDir = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                            QStringLiteral("locale"), QStandardPaths::LocateDirectory);
    const QStringList entries = QDir(localeDir).entryList(QDir::Dirs);
    std::copy_if(entries.begin(), entries.end(),
                std::back_inserter(systemLangList),
                [&localeDir](const QString &language) {
                    return QFile::exists(localeDir + '/' + language + "/kf5_entry.desktop");
                });
    return systemLangList;
}
