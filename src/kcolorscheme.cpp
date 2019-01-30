/* This file is part of the KDE project
 * Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "kcolorscheme.h"

#include <kconfig.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <kcolorutils.h>

#include <QColor>
#include <QBrush>
#include <QWidget>
#include <QCoreApplication>

//BEGIN StateEffects
class StateEffects
{
public:
    explicit StateEffects(QPalette::ColorGroup state, const KSharedConfigPtr &);
    ~StateEffects() {} //{ delete chain; } not needed yet

    QBrush brush(const QBrush &background) const;
    QBrush brush(const QBrush &foreground, const QBrush &background) const;

private:
    enum Effects {
        // Effects
        Intensity = 0,
        Color = 1,
        Contrast = 2,
        // Intensity
        IntensityNoEffect = 0,
        IntensityShade = 1,
        IntensityDarken = 2,
        IntensityLighten = 3,
        // Color
        ColorNoEffect = 0,
        ColorDesaturate = 1,
        ColorFade = 2,
        ColorTint = 3,
        // Contrast
        ContrastNoEffect = 0,
        ContrastFade = 1,
        ContrastTint = 2
    };

    int _effects[3];
    double _amount[3];
    QColor _color;
//     StateEffects *_chain; not needed yet
};

StateEffects::StateEffects(QPalette::ColorGroup state, const KSharedConfigPtr &config)
    : _color(0, 0, 0, 0) //, _chain(0) not needed yet
{
    QString group;
    if (state == QPalette::Disabled) {
        group = QStringLiteral("ColorEffects:Disabled");
    } else if (state == QPalette::Inactive) {
        group = QStringLiteral("ColorEffects:Inactive");
    }

    _effects[0] = 0;
    _effects[1] = 0;
    _effects[2] = 0;

    // NOTE: keep this in sync with kdebase/workspace/kcontrol/colors/colorscm.cpp
    if (! group.isEmpty()) {
        KConfigGroup cfg(config, group);
        const bool enabledByDefault = (state == QPalette::Disabled);
        if (cfg.readEntry("Enable", enabledByDefault)) {
            _effects[Intensity] = cfg.readEntry("IntensityEffect",
                                                (int)(state == QPalette::Disabled ?  IntensityDarken : IntensityNoEffect));
            _effects[Color]     = cfg.readEntry("ColorEffect",
                                                (int)(state == QPalette::Disabled ?  ColorNoEffect : ColorDesaturate));
            _effects[Contrast]  = cfg.readEntry("ContrastEffect",
                                                (int)(state == QPalette::Disabled ?  ContrastFade : ContrastTint));
            _amount[Intensity]  = cfg.readEntry("IntensityAmount", state == QPalette::Disabled ? 0.10 :  0.0);
            _amount[Color]      = cfg.readEntry("ColorAmount", state == QPalette::Disabled ?  0.0 : -0.9);
            _amount[Contrast]   = cfg.readEntry("ContrastAmount", state == QPalette::Disabled ? 0.65 :  0.25);
            if (_effects[Color] > ColorNoEffect) {
                _color = cfg.readEntry("Color", state == QPalette::Disabled ?  QColor(56, 56, 56) : QColor(112, 111, 110));
            }
        }
    }
}

QBrush StateEffects::brush(const QBrush &background) const
{
    QColor color = background.color(); // TODO - actually work on brushes
    switch (_effects[Intensity]) {
    case IntensityShade:
        color = KColorUtils::shade(color, _amount[Intensity]);
        break;
    case IntensityDarken:
        color = KColorUtils::darken(color, _amount[Intensity]);
        break;
    case IntensityLighten:
        color = KColorUtils::lighten(color, _amount[Intensity]);
        break;
    }
    switch (_effects[Color]) {
    case ColorDesaturate:
        color = KColorUtils::darken(color, 0.0, 1.0 - _amount[Color]);
        break;
    case ColorFade:
        color = KColorUtils::mix(color, _color, _amount[Color]);
        break;
    case ColorTint:
        color = KColorUtils::tint(color, _color, _amount[Color]);
        break;
    }
    return QBrush(color);
}

QBrush StateEffects::brush(const QBrush &foreground, const QBrush &background) const
{
    QColor color = foreground.color(); // TODO - actually work on brushes
    QColor bg = background.color();
    // Apply the foreground effects
    switch (_effects[Contrast]) {
    case ContrastFade:
        color = KColorUtils::mix(color, bg, _amount[Contrast]);
        break;
    case ContrastTint:
        color = KColorUtils::tint(color, bg, _amount[Contrast]);
        break;
    }
    // Now apply global effects
    return brush(color);
}
//END StateEffects

//BEGIN default colors
struct SetDefaultColors {
    int NormalBackground[3];
    int AlternateBackground[3];
    int NormalText[3];
    int InactiveText[3];
    int ActiveText[3];
    int LinkText[3];
    int VisitedText[3];
    int NegativeText[3];
    int NeutralText[3];
    int PositiveText[3];
};

struct DecoDefaultColors {
    int Hover[3];
    int Focus[3];
};

// these numbers come from the Breeze color scheme ([breeze]/colors/Breeze.colors)
static const SetDefaultColors defaultViewColors = {
    { 252, 252, 252 }, // Background
    { 239, 240, 241 }, // Alternate
    {  35,  38, 39  }, // Normal
    { 127, 140, 141 }, // Inactive
    {  61, 174, 233 }, // Active
    {  41, 128, 185 }, // Link
    { 127, 140, 141 }, // Visited
    { 218,  68,  83 }, // Negative
    { 246, 116,   0 }, // Neutral
    {  39, 174,  96 }  // Positive
};

static const SetDefaultColors defaultWindowColors = {
    { 239, 240, 241 }, // Background
    { 189, 195, 199 }, // Alternate
    {  35,  38, 39  }, // Normal
    { 127, 140, 141 }, // Inactive
    {  61, 174, 233 }, // Active
    {  41, 128, 185 }, // Link
    { 127, 140, 141 }, // Visited
    { 218,  68,  83 }, // Negative
    { 246, 116,   0 }, // Neutral
    {  39, 174,  96 }  // Positive
};

static const SetDefaultColors defaultButtonColors = {
    { 239, 240, 241 }, // Background
    { 189, 195, 199 }, // Alternate
    {  35,  38, 39  }, // Normal
    { 127, 140, 141 }, // Inactive
    {  61, 174, 233 }, // Active
    {  41, 128, 185 }, // Link
    { 127, 140, 141 }, // Visited
    { 218,  68,  83 }, // Negative
    { 246, 116,   0 }, // Neutral
    {  39, 174,  96 }  // Positive
};

static const SetDefaultColors defaultSelectionColors = {
    {  61, 174, 233 }, // Background
    {  29, 153, 243 }, // Alternate
    { 252, 252, 252 }, // Normal
    { 239, 240, 241 }, // Inactive
    { 252, 252, 252 }, // Active
    { 253, 188,  75 }, // Link
    { 189, 195, 199 }, // Visited
    { 218,  68,  83 }, // Negative
    { 246, 116,   0 }, // Neutral
    {  39, 174,  96 }  // Positive
};

static const SetDefaultColors defaultTooltipColors = {
    {  35,  38, 39  }, // Background
    {  77,  77,  77 }, // Alternate
    { 252, 252, 252 }, // Normal
    { 189, 195, 199 }, // Inactive
    {  61, 174, 233 }, // Active
    {  41, 128, 185 }, // Link
    { 127, 140, 141 }, // Visited
    { 218,  68,  83 }, // Negative
    { 246, 116,   0 }, // Neutral
    {  39, 174,  96 }  // Positive
};

static const SetDefaultColors defaultComplementaryColors = {
    {  49,  54,  59 }, // Background
    {  59,  64,  69 }, // Alternate
    { 239, 240, 241 }, // Normal
    { 175, 176, 179 }, // Inactive
    { 147, 206, 233 }, // Active
    {  61, 174, 230 }, // Link
    {  61, 174, 230 }, // Visited
    { 231,  76,  60 }, // Negative
    { 253, 188,  75 }, // Neutral
    {  46, 174, 230 }  // Positive
};

static const DecoDefaultColors defaultDecorationColors = {
    { 147, 206, 233 }, // Hover
    {  61, 174, 233 }, // Focus
};
//END default colors

KSharedConfigPtr defaultConfig() {
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

//BEGIN KColorSchemePrivate
class KColorSchemePrivate : public QSharedData
{
public:
    explicit KColorSchemePrivate(const KSharedConfigPtr &, QPalette::ColorGroup, const char *, const SetDefaultColors &);
    explicit KColorSchemePrivate(const KSharedConfigPtr &, QPalette::ColorGroup, const char *, const SetDefaultColors &, const QBrush &);
    ~KColorSchemePrivate() {}

    QBrush background(KColorScheme::BackgroundRole) const;
    QBrush foreground(KColorScheme::ForegroundRole) const;
    QBrush decoration(KColorScheme::DecorationRole) const;
    qreal contrast() const;
private:
    struct {
        QBrush fg[8], bg[8], deco[2];
    } _brushes;
    qreal _contrast;

    void init(const KSharedConfigPtr &, QPalette::ColorGroup, const char *, const SetDefaultColors &);
};

#define DEFAULT(c) QColor( c[0], c[1], c[2] )
#define  SET_DEFAULT(a) DEFAULT( defaults.a )
#define DECO_DEFAULT(a) DEFAULT( defaultDecorationColors.a )

KColorSchemePrivate::KColorSchemePrivate(const KSharedConfigPtr &config,
        QPalette::ColorGroup state,
        const char *group,
        const SetDefaultColors &defaults)
{
    KConfigGroup cfg(config, group);
    _contrast = KColorScheme::contrastF(config);

    // loaded-from-config colors (no adjustment)
    _brushes.bg[0] = cfg.readEntry("BackgroundNormal", SET_DEFAULT(NormalBackground));
    _brushes.bg[1] = cfg.readEntry("BackgroundAlternate", SET_DEFAULT(AlternateBackground));

    // the rest
    init(config, state, group, defaults);
}

KColorSchemePrivate::KColorSchemePrivate(const KSharedConfigPtr &config,
        QPalette::ColorGroup state,
        const char *group,
        const SetDefaultColors &defaults,
        const QBrush &tint)
{
    KConfigGroup cfg(config, group);
    _contrast = KColorScheme::contrastF(config);

    // loaded-from-config colors
    _brushes.bg[0] = cfg.readEntry("BackgroundNormal", SET_DEFAULT(NormalBackground));
    _brushes.bg[1] = cfg.readEntry("BackgroundAlternate", SET_DEFAULT(AlternateBackground));

    // adjustment
    _brushes.bg[0] = KColorUtils::tint(_brushes.bg[0].color(), tint.color(), 0.4);
    _brushes.bg[1] = KColorUtils::tint(_brushes.bg[1].color(), tint.color(), 0.4);

    // the rest
    init(config, state, group, defaults);
}

void KColorSchemePrivate::init(const KSharedConfigPtr &config,
                               QPalette::ColorGroup state,
                               const char *group,
                               const SetDefaultColors &defaults)
{
    KConfigGroup cfg(config, group);

    // loaded-from-config colors
    _brushes.fg[0] = cfg.readEntry("ForegroundNormal", SET_DEFAULT(NormalText));
    _brushes.fg[1] = cfg.readEntry("ForegroundInactive", SET_DEFAULT(InactiveText));
    _brushes.fg[2] = cfg.readEntry("ForegroundActive", SET_DEFAULT(ActiveText));
    _brushes.fg[3] = cfg.readEntry("ForegroundLink", SET_DEFAULT(LinkText));
    _brushes.fg[4] = cfg.readEntry("ForegroundVisited", SET_DEFAULT(VisitedText));
    _brushes.fg[5] = cfg.readEntry("ForegroundNegative", SET_DEFAULT(NegativeText));
    _brushes.fg[6] = cfg.readEntry("ForegroundNeutral", SET_DEFAULT(NeutralText));
    _brushes.fg[7] = cfg.readEntry("ForegroundPositive", SET_DEFAULT(PositiveText));

    _brushes.deco[0] = cfg.readEntry("DecorationHover", DECO_DEFAULT(Hover));
    _brushes.deco[1] = cfg.readEntry("DecorationFocus", DECO_DEFAULT(Focus));

    // apply state adjustments
    if (state != QPalette::Active) {
        StateEffects effects(state, config);
        for (int i = 0; i < 8; i++) {
            _brushes.fg[i] = effects.brush(_brushes.fg[i], _brushes.bg[0]);
        }
        _brushes.deco[0] = effects.brush(_brushes.deco[0], _brushes.bg[0]);
        _brushes.deco[1] = effects.brush(_brushes.deco[1], _brushes.bg[0]);
        _brushes.bg[0] = effects.brush(_brushes.bg[0]);
        _brushes.bg[1] = effects.brush(_brushes.bg[1]);
    }

    // calculated backgrounds
    _brushes.bg[2] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[2].color());
    _brushes.bg[3] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[3].color());
    _brushes.bg[4] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[4].color());
    _brushes.bg[5] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[5].color());
    _brushes.bg[6] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[6].color());
    _brushes.bg[7] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[7].color());
}

QBrush KColorSchemePrivate::background(KColorScheme::BackgroundRole role) const
{
    switch (role) {
    case KColorScheme::AlternateBackground:
        return _brushes.bg[1];
    case KColorScheme::ActiveBackground:
        return _brushes.bg[2];
    case KColorScheme::LinkBackground:
        return _brushes.bg[3];
    case KColorScheme::VisitedBackground:
        return _brushes.bg[4];
    case KColorScheme::NegativeBackground:
        return _brushes.bg[5];
    case KColorScheme::NeutralBackground:
        return _brushes.bg[6];
    case KColorScheme::PositiveBackground:
        return _brushes.bg[7];
    default:
        return _brushes.bg[0];
    }
}

QBrush KColorSchemePrivate::foreground(KColorScheme::ForegroundRole role) const
{
    switch (role) {
    case KColorScheme::InactiveText:
        return _brushes.fg[1];
    case KColorScheme::ActiveText:
        return _brushes.fg[2];
    case KColorScheme::LinkText:
        return _brushes.fg[3];
    case KColorScheme::VisitedText:
        return _brushes.fg[4];
    case KColorScheme::NegativeText:
        return _brushes.fg[5];
    case KColorScheme::NeutralText:
        return _brushes.fg[6];
    case KColorScheme::PositiveText:
        return _brushes.fg[7];
    default:
        return _brushes.fg[0];
    }
}

QBrush KColorSchemePrivate::decoration(KColorScheme::DecorationRole role) const
{
    switch (role) {
    case KColorScheme::FocusColor:
        return _brushes.deco[1];
    default:
        return _brushes.deco[0];
    }
}

qreal KColorSchemePrivate::contrast() const
{
    return _contrast;
}
//END KColorSchemePrivate

//BEGIN KColorScheme
KColorScheme::KColorScheme(const KColorScheme &other) : d(other.d)
{
}

KColorScheme &KColorScheme::operator=(const KColorScheme &other)
{
    d = other.d;
    return *this;
}

KColorScheme::~KColorScheme()
{
}

KColorScheme::KColorScheme(QPalette::ColorGroup state, ColorSet set, KSharedConfigPtr config)
{
    if (!config) {
        config = defaultConfig();
    }

    switch (set) {
    case Window:
        d = new KColorSchemePrivate(config, state, "Colors:Window", defaultWindowColors);
        break;
    case Button:
        d = new KColorSchemePrivate(config, state, "Colors:Button", defaultButtonColors);
        break;
    case Selection: {
        KConfigGroup group(config, "ColorEffects:Inactive");
        // NOTE: keep this in sync with kdebase/workspace/kcontrol/colors/colorscm.cpp
        bool inactiveSelectionEffect = group.readEntry("ChangeSelectionColor", group.readEntry("Enable", true));
        // if enabled, inactiver/disabled uses Window colors instead, ala gtk
        // ...except tinted with the Selection:NormalBackground color so it looks more like selection
        if (state == QPalette::Active || (state == QPalette::Inactive && !inactiveSelectionEffect)) {
            d = new KColorSchemePrivate(config, state, "Colors:Selection", defaultSelectionColors);
        } else if (state == QPalette::Inactive)
            d = new KColorSchemePrivate(config, state, "Colors:Window", defaultWindowColors,
                                        KColorScheme(QPalette::Active, Selection, config).background());
        else { // disabled (...and still want this branch when inactive+disabled exists)
            d = new KColorSchemePrivate(config, state, "Colors:Window", defaultWindowColors);
        }
    } break;
    case Tooltip:
        d = new KColorSchemePrivate(config, state, "Colors:Tooltip", defaultTooltipColors);
        break;
    case Complementary:
        d = new KColorSchemePrivate(config, state, "Colors:Complementary", defaultComplementaryColors);
        break;
    default:
        d = new KColorSchemePrivate(config, state, "Colors:View", defaultViewColors);
    }
}

// static
int KColorScheme::contrast()
{
    KConfigGroup g(KSharedConfig::openConfig(), "KDE");
    return g.readEntry("contrast", 7);
}

// static
qreal KColorScheme::contrastF(const KSharedConfigPtr &config)
{
    if (config) {
        KConfigGroup g(config, "KDE");
        return 0.1 * g.readEntry("contrast", 7);
    }
    return 0.1 * (qreal)contrast();
}

QBrush KColorScheme::background(BackgroundRole role) const
{
    return d->background(role);
}

QBrush KColorScheme::foreground(ForegroundRole role) const
{
    return d->foreground(role);
}

QBrush KColorScheme::decoration(DecorationRole role) const
{
    return d->decoration(role);
}

QColor KColorScheme::shade(ShadeRole role) const
{
    return shade(background().color(), role, d->contrast());
}

QColor KColorScheme::shade(const QColor &color, ShadeRole role)
{
    return shade(color, role, KColorScheme::contrastF());
}

QColor KColorScheme::shade(const QColor &color, ShadeRole role, qreal contrast, qreal chromaAdjust)
{
    // nan -> 1.0
    contrast = (1.0 > contrast ? (-1.0 < contrast ? contrast : -1.0) : 1.0);
    qreal y = KColorUtils::luma(color), yi = 1.0 - y;

    // handle very dark colors (base, mid, dark, shadow == midlight, light)
    if (y < 0.006) {
        switch (role) {
        case KColorScheme::LightShade:
            return KColorUtils::shade(color, 0.05 + 0.95 * contrast, chromaAdjust);
        case KColorScheme::MidShade:
            return KColorUtils::shade(color, 0.01 + 0.20 * contrast, chromaAdjust);
        case KColorScheme::DarkShade:
            return KColorUtils::shade(color, 0.02 + 0.40 * contrast, chromaAdjust);
        default:
            return KColorUtils::shade(color, 0.03 + 0.60 * contrast, chromaAdjust);
        }
    }

    // handle very light colors (base, midlight, light == mid, dark, shadow)
    if (y > 0.93) {
        switch (role) {
        case KColorScheme::MidlightShade:
            return KColorUtils::shade(color, -0.02 - 0.20 * contrast, chromaAdjust);
        case KColorScheme::DarkShade:
            return KColorUtils::shade(color, -0.06 - 0.60 * contrast, chromaAdjust);
        case KColorScheme::ShadowShade:
            return KColorUtils::shade(color, -0.10 - 0.90 * contrast, chromaAdjust);
        default:
            return KColorUtils::shade(color, -0.04 - 0.40 * contrast, chromaAdjust);
        }
    }

    // handle everything else
    qreal lightAmount = (0.05 + y * 0.55) * (0.25 + contrast * 0.75);
    qreal darkAmount = (- y) * (0.55 + contrast * 0.35);
    switch (role) {
    case KColorScheme::LightShade:
        return KColorUtils::shade(color, lightAmount, chromaAdjust);
    case KColorScheme::MidlightShade:
        return KColorUtils::shade(color, (0.15 + 0.35 * yi) * lightAmount, chromaAdjust);
    case KColorScheme::MidShade:
        return KColorUtils::shade(color, (0.35 + 0.15 * y) * darkAmount, chromaAdjust);
    case KColorScheme::DarkShade:
        return KColorUtils::shade(color, darkAmount, chromaAdjust);
    default:
        return KColorUtils::darken(KColorUtils::shade(color, darkAmount, chromaAdjust), 0.5 + 0.3 * y);
    }
}

void KColorScheme::adjustBackground(QPalette &palette, BackgroundRole newRole, QPalette::ColorRole color,
                                    ColorSet set, KSharedConfigPtr config)
{
    palette.setBrush(QPalette::Active,   color, KColorScheme(QPalette::Active,   set, config).background(newRole));
    palette.setBrush(QPalette::Inactive, color, KColorScheme(QPalette::Inactive, set, config).background(newRole));
    palette.setBrush(QPalette::Disabled, color, KColorScheme(QPalette::Disabled, set, config).background(newRole));
}

void KColorScheme::adjustForeground(QPalette &palette, ForegroundRole newRole, QPalette::ColorRole color,
                                    ColorSet set, KSharedConfigPtr config)
{
    palette.setBrush(QPalette::Active,   color, KColorScheme(QPalette::Active,   set, config).foreground(newRole));
    palette.setBrush(QPalette::Inactive, color, KColorScheme(QPalette::Inactive, set, config).foreground(newRole));
    palette.setBrush(QPalette::Disabled, color, KColorScheme(QPalette::Disabled, set, config).foreground(newRole));
}

QPalette KColorScheme::createApplicationPalette(const KSharedConfigPtr &config)
{
    QPalette palette;

    static const QPalette::ColorGroup states[3] = { QPalette::Active, QPalette::Inactive, QPalette::Disabled };

    // TT thinks tooltips shouldn't use active, so we use our active colors for all states
    KColorScheme schemeTooltip(QPalette::Active, KColorScheme::Tooltip, config);

    for (int i = 0; i < 3; i++) {
        QPalette::ColorGroup state = states[i];
        KColorScheme schemeView(state, KColorScheme::View, config);
        KColorScheme schemeWindow(state, KColorScheme::Window, config);
        KColorScheme schemeButton(state, KColorScheme::Button, config);
        KColorScheme schemeSelection(state, KColorScheme::Selection, config);

        palette.setBrush(state, QPalette::WindowText, schemeWindow.foreground());
        palette.setBrush(state, QPalette::Window, schemeWindow.background());
        palette.setBrush(state, QPalette::Base, schemeView.background());
        palette.setBrush(state, QPalette::Text, schemeView.foreground());
        palette.setBrush(state, QPalette::Button, schemeButton.background());
        palette.setBrush(state, QPalette::ButtonText, schemeButton.foreground());
        palette.setBrush(state, QPalette::Highlight, schemeSelection.background());
        palette.setBrush(state, QPalette::HighlightedText, schemeSelection.foreground());
        palette.setBrush(state, QPalette::ToolTipBase, schemeTooltip.background());
        palette.setBrush(state, QPalette::ToolTipText, schemeTooltip.foreground());

        palette.setColor(state, QPalette::Light, schemeWindow.shade(KColorScheme::LightShade));
        palette.setColor(state, QPalette::Midlight, schemeWindow.shade(KColorScheme::MidlightShade));
        palette.setColor(state, QPalette::Mid, schemeWindow.shade(KColorScheme::MidShade));
        palette.setColor(state, QPalette::Dark, schemeWindow.shade(KColorScheme::DarkShade));
        palette.setColor(state, QPalette::Shadow, schemeWindow.shade(KColorScheme::ShadowShade));

        palette.setBrush(state, QPalette::AlternateBase, schemeView.background(KColorScheme::AlternateBackground));
        palette.setBrush(state, QPalette::Link, schemeView.foreground(KColorScheme::LinkText));
        palette.setBrush(state, QPalette::LinkVisited, schemeView.foreground(KColorScheme::VisitedText));
    }

    return palette;
}

//END KColorScheme

//BEGIN KStatefulBrush
class KStatefulBrushPrivate : public QBrush // for now, just be a QBrush
{
public:
    KStatefulBrushPrivate() : QBrush() {}
    KStatefulBrushPrivate(const QBrush &brush) : QBrush(brush) {} // not explicit
};

KStatefulBrush::KStatefulBrush()
{
    d = new KStatefulBrushPrivate[3];
}

KStatefulBrush::KStatefulBrush(KColorScheme::ColorSet set, KColorScheme::ForegroundRole role,
                               KSharedConfigPtr config)
{
    d = new KStatefulBrushPrivate[3];
    d[0] = KColorScheme(QPalette::Active,   set, config).foreground(role);
    d[1] = KColorScheme(QPalette::Disabled, set, config).foreground(role);
    d[2] = KColorScheme(QPalette::Inactive, set, config).foreground(role);
}

KStatefulBrush::KStatefulBrush(KColorScheme::ColorSet set, KColorScheme::BackgroundRole role,
                               KSharedConfigPtr config)
{
    d = new KStatefulBrushPrivate[3];
    d[0] = KColorScheme(QPalette::Active,   set, config).background(role);
    d[1] = KColorScheme(QPalette::Disabled, set, config).background(role);
    d[2] = KColorScheme(QPalette::Inactive, set, config).background(role);
}

KStatefulBrush::KStatefulBrush(KColorScheme::ColorSet set, KColorScheme::DecorationRole role,
                               KSharedConfigPtr config)
{
    d = new KStatefulBrushPrivate[3];
    d[0] = KColorScheme(QPalette::Active,   set, config).decoration(role);
    d[1] = KColorScheme(QPalette::Disabled, set, config).decoration(role);
    d[2] = KColorScheme(QPalette::Inactive, set, config).decoration(role);
}

KStatefulBrush::KStatefulBrush(const QBrush &brush, KSharedConfigPtr config)
{
    if (!config) {
        config = defaultConfig();
    }
    d = new KStatefulBrushPrivate[3];
    d[0] = brush;
    d[1] = StateEffects(QPalette::Disabled, config).brush(brush);
    d[2] = StateEffects(QPalette::Inactive, config).brush(brush);
}

KStatefulBrush::KStatefulBrush(const QBrush &brush, const QBrush &background,
                               KSharedConfigPtr config)
{
    if (!config) {
        config = defaultConfig();
    }
    d = new KStatefulBrushPrivate[3];
    d[0] = brush;
    d[1] = StateEffects(QPalette::Disabled, config).brush(brush, background);
    d[2] = StateEffects(QPalette::Inactive, config).brush(brush, background);
}

KStatefulBrush::KStatefulBrush(const KStatefulBrush &other)
{
    d = new KStatefulBrushPrivate[3];
    d[0] = other.d[0];
    d[1] = other.d[1];
    d[2] = other.d[2];
}

KStatefulBrush::~KStatefulBrush()
{
    delete[] d;
}

KStatefulBrush &KStatefulBrush::operator=(const KStatefulBrush &other)
{
    d[0] = other.d[0];
    d[1] = other.d[1];
    d[2] = other.d[2];
    return *this;
}

QBrush KStatefulBrush::brush(QPalette::ColorGroup state) const
{
    switch (state) {
    case QPalette::Inactive:
        return d[2];
    case QPalette::Disabled:
        return d[1];
    default:
        return d[0];
    }
}

QBrush KStatefulBrush::brush(const QPalette &pal) const
{
    return brush(pal.currentColorGroup());
}

QBrush KStatefulBrush::brush(const QWidget *widget) const
{
    if (widget) {
        return brush(widget->palette());
    } else {
        return QBrush();
    }
}
//END KStatefulBrush

