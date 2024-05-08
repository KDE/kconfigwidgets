/*
    SPDX-FileCopyrightText: 2024 Christoph Cullmann <cullmann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kstylemanager.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QApplication>
#include <QStyle>
#include <QStyleFactory>

#include <private/qguiapplication_p.h>
#include <qpa/qplatformtheme.h>

void KStyleManager::initStyle()
{
    // do nothing if we have the proper platform theme already
    if (QGuiApplicationPrivate::platformTheme() && QGuiApplicationPrivate::platformTheme()->name() == QLatin1String("kde")) {
        return;
    }

    // get config, with fallback to kdeglobals
    const auto config = KSharedConfig::openConfig();

    // enforce the style configured by the user, with kdeglobals fallback
    // if not set or the style is not there, use Breeze
    QString styleToUse = KConfigGroup(config, QStringLiteral("General")).readEntry("widgetStyle", QString());
    if (styleToUse.isEmpty() || !QApplication::setStyle(styleToUse)) {
        styleToUse = QStringLiteral("breeze");
        QApplication::setStyle(styleToUse);
    }
}
