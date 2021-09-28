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

    static QIcon createPreview(const QString &path);

#ifdef Q_OS_WIN
    static WindowsMessagesNotifier m_windowsMessagesNotifier;
    WindowsMessagesNotifier &getWindowsMessagesNotifier() {return m_windowsMessagesNotifier;}
    const QString& getLightColorScheme() {return m_lightColorScheme;}
    const QString& getDarkColorScheme() {return m_darkColorScheme;}

private:
    QString m_lightColorScheme = QStringLiteral("Breeze");
    QString m_darkColorScheme = QStringLiteral("BreezeDark");
#endif
};

#endif
