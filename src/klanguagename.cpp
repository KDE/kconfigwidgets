/*
    SPDX-FileCopyrightText: 1999-2003 Hans Petter Bieker <bieker@kde.org>
    SPDX-FileCopyrightText: 2007 David Jarvie <software@astrojar.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
    auto languageExists = [&localeDir](const QString &language) {
        return QFile::exists(localeDir + '/' + language + "/kf5_entry.desktop");
    };
    std::copy_if(entries.begin(), entries.end(),
                std::back_inserter(systemLangList),
                languageExists);                
    return systemLangList;
}
