/*
    This file is part of the KDE project
    SPDX-FileCopyrightText: 2013 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCOLORSCHEMEMANAGER_P_H
#define KCOLORSCHEMEMANAGER_P_H

#include <memory>

#include "kcolorschememodel.h"
#ifdef Q_OS_WIN
#include "windowsmessagesnotifier.h"
#endif

class KColorSchemeManagerPrivate
{
public:
    KColorSchemeManagerPrivate();

    std::unique_ptr<KColorSchemeModel> model;
    bool m_autosaveChanges = true;
    bool m_defaultSchemeSelected = true;

    static QIcon createPreview(const QString &path);
    void activateSchemeInternal(const QString &colorSchemePath);
    QString automaticColorSchemePath() const;
    QModelIndex indexForSchemeId(const QString &id) const;

#ifdef Q_OS_WIN
    static WindowsMessagesNotifier m_windowsMessagesNotifier;
    WindowsMessagesNotifier &getWindowsMessagesNotifier() const
    {
        return m_windowsMessagesNotifier;
    }
    const QString &getLightColorScheme() const
    {
        return m_lightColorScheme;
    }
    const QString &getDarkColorScheme() const
    {
        return m_darkColorScheme;
    }

private:
    QString m_lightColorScheme = QStringLiteral("Breeze");
    QString m_darkColorScheme = QStringLiteral("BreezeDark");
#endif
};

#endif
