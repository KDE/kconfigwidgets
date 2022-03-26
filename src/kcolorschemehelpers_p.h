/*
    SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCOLORSCHEME_P_H
#define KCOLORSCHEME_P_H

#include <KConfigGroup>
#include <KSharedConfig>

#include <QCoreApplication>
#include <QDir>
#include <QPalette>

#include <array>

#if KCONFIGWIDGETS_BUILD_DEPRECATED_SINCE(5, 94)
static KSharedConfigPtr defaultConfig()
{
    // cache the value we'll return, since usually it's going to be the same value
    static thread_local KSharedConfigPtr config;
    // Read from the application's color scheme file (as set by KColorSchemeManager).
    // If unset, this is equivalent to openConfig() and the system scheme is used.
    const auto colorSchemePath = qApp->property("KDE_COLOR_SCHEME_PATH").toString();
    if (!config || config->name() != colorSchemePath) {
        config = KSharedConfig::openConfig(colorSchemePath);
    }
    return config;
}
#endif

static KSharedConfigPtr openColorScheme(const QString &fileName)
{
    if (fileName.endsWith(QLatin1String(".colors"))) {
        const QString path =
            QDir::isAbsolutePath(fileName) ? fileName : QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("colors/") + fileName);
        if (QFileInfo::exists(path)) {
            return KSharedConfig::openConfig(path, KConfig::SimpleConfig);
        }
    }

    static thread_local KSharedConfigPtr defaultConfig;
    const auto colorSchemeManagerPath = qApp->property("KDE_COLOR_SCHEME_PATH").toString();
    if (!colorSchemeManagerPath.isEmpty()) {
        if (!defaultConfig || colorSchemeManagerPath != defaultConfig->name()) {
            defaultConfig = KSharedConfig::openConfig(colorSchemeManagerPath, KConfig::SimpleConfig);
        }
        return defaultConfig;
    }

    auto globalsGroup = KSharedConfig::openConfig()->group("General");
    if (!globalsGroup.hasKey("ColorSchemeFile")) {
        return KSharedConfig::openConfig(); // old Plasma, fall back to kdeglobals/old behavior
    }
    const QString colorScheme = KSharedConfig::openConfig()->group("General").readEntry("ColorSchemeFile");
    if (!defaultConfig || defaultConfig->name() != colorScheme) {
        // TODO should plasma save aboslute path or jsut name?
        defaultConfig = KSharedConfig::openConfig(QStringLiteral("colors/") + colorScheme, KConfig::SimpleConfig, QStandardPaths::GenericDataLocation);
    }
    return defaultConfig;
}

class StateEffects
{
public:
    explicit StateEffects(QPalette::ColorGroup state, const KSharedConfigPtr &);
    ~StateEffects()
    {
    }

    QBrush brush(const QBrush &background) const;
    QBrush brush(const QBrush &foreground, const QBrush &background) const;

private:
    enum EffectTypes {
        Intensity,
        Color,
        Contrast,
        NEffectTypes,
    };

    enum IntensityEffects {
        IntensityNoEffect,
        IntensityShade,
        IntensityDarken,
        IntensityLighten,
        NIntensityEffects,
    };

    enum ColorEffects {
        ColorNoEffect,
        ColorDesaturate,
        ColorFade,
        ColorTint,
        NColorEffects,
    };

    enum ContrastEffects {
        ContrastNoEffect,
        ContrastFade,
        ContrastTint,
        NContrastEffects,
    };

    int _effects[NEffectTypes];
    double _amount[NEffectTypes];
    QColor _color;
};

#endif
