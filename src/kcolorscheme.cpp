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
    enum EffectTypes {
        Intensity,
        Color,
        Contrast,
        NEffectTypes
    };
    
    enum IntensityEffects {
        IntensityNoEffect,
        IntensityShade,
        IntensityDarken,
        IntensityLighten,
        NIntensityEffects
    };
    
    enum ColorEffects {
        ColorNoEffect,
        ColorDesaturate,
        ColorFade,
        ColorTint,
        NColorEffects
    };
    
    enum ContrastEffects {
        ContrastNoEffect,
        ContrastFade,
        ContrastTint,
        NContrastEffects        
    };

    int _effects[NEffectTypes];
    double _amount[NEffectTypes];
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

    for (auto &effect : _effects) {
        effect = 0;
    }

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
    int Focus[3];
    int Hover[3];
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

static const SetDefaultColors defaultHeaderColors = {
    { 227, 229, 231 }, // Background
    { 239, 240, 241 }, // Alternate
    {  35,  38,  41 }, // Normal
    { 112, 125, 138 }, // Inactive
    {  61, 174, 233 }, // Active
    {  41, 128, 185 }, // Link
    { 155,  89, 182 }, // Visited
    { 218,  68,  83 }, // Negative
    { 246, 116,   0 }, // Neutral
    {  39, 174,  96 }  // Positive
};

static const DecoDefaultColors defaultDecorationColors = {
    {  61, 174, 233 }, // Focus
    { 147, 206, 233 }, // Hover
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
        QBrush fg[KColorScheme::NForegroundRoles];
        QBrush bg[KColorScheme::NBackgroundRoles];
        QBrush deco[KColorScheme::NDecorationRoles];
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
    if (strcmp(group, "Colors:Header") == 0) { // For compatibility with color schemes made before ColorSet::Header was added
        // Default to Window colors before using Header default colors
        KConfigGroup windowCfg(config, "Colors:Window");
        _brushes.bg[KColorScheme::NormalBackground] = cfg.readEntry("BackgroundNormal", 
            windowCfg.readEntry("BackgroundNormal", SET_DEFAULT(NormalBackground)));
        _brushes.bg[KColorScheme::AlternateBackground] = cfg.readEntry("BackgroundAlternate", 
            windowCfg.readEntry("BackgroundAlternate", SET_DEFAULT(AlternateBackground)));
    } else {
        _brushes.bg[KColorScheme::NormalBackground] = cfg.readEntry("BackgroundNormal", SET_DEFAULT(NormalBackground));
        _brushes.bg[KColorScheme::AlternateBackground] = cfg.readEntry("BackgroundAlternate", SET_DEFAULT(AlternateBackground));
    }

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
    if (strcmp(group, "Colors:Header") == 0) { // For compatibility with color schemes made before ColorSet::Header was added
        // Default to Window colors before using Header default colors
        KConfigGroup windowCfg(config, "Colors:Window");
        _brushes.bg[KColorScheme::NormalBackground] = cfg.readEntry("BackgroundNormal", 
            windowCfg.readEntry("BackgroundNormal", SET_DEFAULT(NormalBackground)));
        _brushes.bg[KColorScheme::AlternateBackground] = cfg.readEntry("BackgroundAlternate", 
            windowCfg.readEntry("BackgroundAlternate", SET_DEFAULT(AlternateBackground)));
    } else {
        _brushes.bg[KColorScheme::NormalBackground] = cfg.readEntry("BackgroundNormal", SET_DEFAULT(NormalBackground));
        _brushes.bg[KColorScheme::AlternateBackground] = cfg.readEntry("BackgroundAlternate", SET_DEFAULT(AlternateBackground));
    }


    // adjustment
    _brushes.bg[KColorScheme::NormalBackground] = 
        KColorUtils::tint(_brushes.bg[KColorScheme::NormalBackground].color(), tint.color(), 0.4);
    _brushes.bg[KColorScheme::AlternateBackground] = 
        KColorUtils::tint(_brushes.bg[KColorScheme::AlternateBackground].color(), tint.color(), 0.4);

    // the rest
    init(config, state, group, defaults);
}

void KColorSchemePrivate::init(const KSharedConfigPtr &config,
                               QPalette::ColorGroup state,
                               const char *group,
                               const SetDefaultColors &defaults)
{
    KConfigGroup cfg(config, group);
    bool hasInactivePalette = false;
    if (state == QPalette::Inactive) {
        KConfigGroup inactiveGroup = KConfigGroup(&cfg, "Inactive");
        if (inactiveGroup.exists()) {
            cfg = inactiveGroup;
            hasInactivePalette = true;
        }
    }

    // loaded-from-config colors
    if (strcmp(group, "Colors:Header") == 0) { // For compatibility with color schemes made before ColorSet::Header was added
        // Default to Window colors before using Header default colors
        KConfigGroup windowCfg(config, "Colors:Window");
        _brushes.fg[KColorScheme::NormalText] = cfg.readEntry("ForegroundNormal", 
            windowCfg.readEntry("ForegroundNormal", SET_DEFAULT(NormalText)));
        _brushes.fg[KColorScheme::InactiveText] = cfg.readEntry("ForegroundInactive", 
            windowCfg.readEntry("ForegroundInactive", SET_DEFAULT(InactiveText)));
        _brushes.fg[KColorScheme::ActiveText] = cfg.readEntry("ForegroundActive", 
            windowCfg.readEntry("ForegroundActive", SET_DEFAULT(ActiveText)));
        _brushes.fg[KColorScheme::LinkText] = cfg.readEntry("ForegroundLink", 
            windowCfg.readEntry("ForegroundLink", SET_DEFAULT(LinkText)));
        _brushes.fg[KColorScheme::VisitedText] = cfg.readEntry("ForegroundVisited", 
            windowCfg.readEntry("ForegroundVisited", SET_DEFAULT(VisitedText)));
        _brushes.fg[KColorScheme::NegativeText] = cfg.readEntry("ForegroundNegative", 
            windowCfg.readEntry("ForegroundNegative", SET_DEFAULT(NegativeText)));
        _brushes.fg[KColorScheme::NeutralText] = cfg.readEntry("ForegroundNeutral", 
            windowCfg.readEntry("ForegroundNeutral", SET_DEFAULT(NeutralText)));
        _brushes.fg[KColorScheme::PositiveText] = cfg.readEntry("ForegroundPositive", 
            windowCfg.readEntry("ForegroundPositive", SET_DEFAULT(PositiveText)));

        _brushes.deco[KColorScheme::FocusColor] = cfg.readEntry("DecorationFocus", 
            windowCfg.readEntry("DecorationFocus", DECO_DEFAULT(Focus)));
        _brushes.deco[KColorScheme::HoverColor] = cfg.readEntry("DecorationHover", 
            windowCfg.readEntry("DecorationHover", DECO_DEFAULT(Hover)));
    } else {
        _brushes.fg[KColorScheme::NormalText] = cfg.readEntry("ForegroundNormal", SET_DEFAULT(NormalText));
        _brushes.fg[KColorScheme::InactiveText] = cfg.readEntry("ForegroundInactive", SET_DEFAULT(InactiveText));
        _brushes.fg[KColorScheme::ActiveText] = cfg.readEntry("ForegroundActive", SET_DEFAULT(ActiveText));
        _brushes.fg[KColorScheme::LinkText] = cfg.readEntry("ForegroundLink", SET_DEFAULT(LinkText));
        _brushes.fg[KColorScheme::VisitedText] = cfg.readEntry("ForegroundVisited", SET_DEFAULT(VisitedText));
        _brushes.fg[KColorScheme::NegativeText] = cfg.readEntry("ForegroundNegative", SET_DEFAULT(NegativeText));
        _brushes.fg[KColorScheme::NeutralText] = cfg.readEntry("ForegroundNeutral", SET_DEFAULT(NeutralText));
        _brushes.fg[KColorScheme::PositiveText] = cfg.readEntry("ForegroundPositive", SET_DEFAULT(PositiveText));

        _brushes.deco[KColorScheme::FocusColor] = cfg.readEntry("DecorationFocus", DECO_DEFAULT(Focus));
        _brushes.deco[KColorScheme::HoverColor] = cfg.readEntry("DecorationHover", DECO_DEFAULT(Hover));
    }
    _brushes.fg[KColorScheme::ActiveText] = cfg.readEntry("ForegroundActive", SET_DEFAULT(ActiveText));
    _brushes.fg[KColorScheme::LinkText] = cfg.readEntry("ForegroundLink", SET_DEFAULT(LinkText));
    _brushes.fg[KColorScheme::VisitedText] = cfg.readEntry("ForegroundVisited", SET_DEFAULT(VisitedText));
    _brushes.fg[KColorScheme::NegativeText] = cfg.readEntry("ForegroundNegative", SET_DEFAULT(NegativeText));
    _brushes.fg[KColorScheme::NeutralText] = cfg.readEntry("ForegroundNeutral", SET_DEFAULT(NeutralText));
    _brushes.fg[KColorScheme::PositiveText] = cfg.readEntry("ForegroundPositive", SET_DEFAULT(PositiveText));

    _brushes.deco[KColorScheme::FocusColor] = cfg.readEntry("DecorationFocus", DECO_DEFAULT(Focus));
    _brushes.deco[KColorScheme::HoverColor] = cfg.readEntry("DecorationHover", DECO_DEFAULT(Hover));

    // apply state adjustments
    if (state != QPalette::Active || (state == QPalette::Inactive && !hasInactivePalette)) {
        StateEffects effects(state, config);
        for (auto &fg : _brushes.fg) {
            fg = effects.brush(fg, _brushes.bg[KColorScheme::NormalBackground]);
        }
        for (auto &deco : _brushes.deco) {
            deco = effects.brush(deco, _brushes.bg[KColorScheme::NormalBackground]);
        }
        _brushes.bg[KColorScheme::NormalBackground] = effects.brush(_brushes.bg[KColorScheme::NormalBackground]);
        _brushes.bg[KColorScheme::AlternateBackground] = effects.brush(_brushes.bg[KColorScheme::AlternateBackground]);
    }

    // calculated backgrounds
    _brushes.bg[KColorScheme::ActiveBackground] =
        KColorUtils::tint(_brushes.bg[KColorScheme::NormalBackground].color(),
                          _brushes.fg[KColorScheme::ActiveText].color());
    _brushes.bg[KColorScheme::LinkBackground] =
        KColorUtils::tint(_brushes.bg[KColorScheme::NormalBackground].color(),
                          _brushes.fg[KColorScheme::LinkText].color());
    _brushes.bg[KColorScheme::VisitedBackground] =
        KColorUtils::tint(_brushes.bg[KColorScheme::NormalBackground].color(),
                          _brushes.fg[KColorScheme::VisitedText].color());
    _brushes.bg[KColorScheme::NegativeBackground] =
        KColorUtils::tint(_brushes.bg[KColorScheme::NormalBackground].color(),
                          _brushes.fg[KColorScheme::NegativeText].color());
    _brushes.bg[KColorScheme::NeutralBackground] =
        KColorUtils::tint(_brushes.bg[KColorScheme::NormalBackground].color(),
                          _brushes.fg[KColorScheme::NeutralText].color());
    _brushes.bg[KColorScheme::PositiveBackground] =
        KColorUtils::tint(_brushes.bg[KColorScheme::NormalBackground].color(),
                          _brushes.fg[KColorScheme::PositiveText].color());
}

QBrush KColorSchemePrivate::background(KColorScheme::BackgroundRole role) const
{
    if (role >= KColorScheme::NormalBackground && role < KColorScheme::NBackgroundRoles) {
        return _brushes.bg[role];
    } else {
        return _brushes.bg[KColorScheme::NormalBackground];
    }
}

QBrush KColorSchemePrivate::foreground(KColorScheme::ForegroundRole role) const
{
    if (role >= KColorScheme::NormalText && role < KColorScheme::NForegroundRoles) {
        return _brushes.fg[role];
    } else {
        return _brushes.fg[KColorScheme::NormalText];
    }
}

QBrush KColorSchemePrivate::decoration(KColorScheme::DecorationRole role) const
{
    if (role >= KColorScheme::FocusColor && role < KColorScheme::NDecorationRoles) {
        return _brushes.deco[role];
    } else {
        return _brushes.deco[KColorScheme::FocusColor];
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
    case Header:
        d = new KColorSchemePrivate(config, state, "Colors:Header", defaultHeaderColors);
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

    static const QPalette::ColorGroup states[QPalette::NColorGroups] = {
        QPalette::Active, QPalette::Inactive, QPalette::Disabled
    };

    // TT thinks tooltips shouldn't use active, so we use our active colors for all states
    KColorScheme schemeTooltip(QPalette::Active, KColorScheme::Tooltip, config);

    for (auto state : states) {
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
    d = new KStatefulBrushPrivate[QPalette::NColorGroups];
}

KStatefulBrush::KStatefulBrush(KColorScheme::ColorSet set, KColorScheme::ForegroundRole role,
                               KSharedConfigPtr config)
{
    d = new KStatefulBrushPrivate[QPalette::NColorGroups];
    d[QPalette::Active] = KColorScheme(QPalette::Active,   set, config).foreground(role);
    d[QPalette::Disabled] = KColorScheme(QPalette::Disabled, set, config).foreground(role);
    d[QPalette::Inactive] = KColorScheme(QPalette::Inactive, set, config).foreground(role);
}

KStatefulBrush::KStatefulBrush(KColorScheme::ColorSet set, KColorScheme::BackgroundRole role,
                               KSharedConfigPtr config)
{
    d = new KStatefulBrushPrivate[QPalette::NColorGroups];
    d[QPalette::Active] = KColorScheme(QPalette::Active,   set, config).background(role);
    d[QPalette::Disabled] = KColorScheme(QPalette::Disabled, set, config).background(role);
    d[QPalette::Inactive] = KColorScheme(QPalette::Inactive, set, config).background(role);
}

KStatefulBrush::KStatefulBrush(KColorScheme::ColorSet set, KColorScheme::DecorationRole role,
                               KSharedConfigPtr config)
{
    d = new KStatefulBrushPrivate[QPalette::NColorGroups];
    d[QPalette::Active] = KColorScheme(QPalette::Active,   set, config).decoration(role);
    d[QPalette::Disabled] = KColorScheme(QPalette::Disabled, set, config).decoration(role);
    d[QPalette::Inactive] = KColorScheme(QPalette::Inactive, set, config).decoration(role);
}

KStatefulBrush::KStatefulBrush(const QBrush &brush, KSharedConfigPtr config)
{
    if (!config) {
        config = defaultConfig();
    }
    d = new KStatefulBrushPrivate[QPalette::NColorGroups];
    d[QPalette::Active] = brush;
    d[QPalette::Disabled] = StateEffects(QPalette::Disabled, config).brush(brush);
    d[QPalette::Inactive] = StateEffects(QPalette::Inactive, config).brush(brush);
}

KStatefulBrush::KStatefulBrush(const QBrush &brush, const QBrush &background,
                               KSharedConfigPtr config)
{
    if (!config) {
        config = defaultConfig();
    }
    d = new KStatefulBrushPrivate[QPalette::NColorGroups];
    d[QPalette::Active] = brush;
    d[QPalette::Disabled] = StateEffects(QPalette::Disabled, config).brush(brush, background);
    d[QPalette::Inactive] = StateEffects(QPalette::Inactive, config).brush(brush, background);
}

KStatefulBrush::KStatefulBrush(const KStatefulBrush &other)
{
    d = new KStatefulBrushPrivate[QPalette::NColorGroups];
    d[QPalette::Active] = other.d[QPalette::Active];
    d[QPalette::Disabled] = other.d[QPalette::Disabled];
    d[QPalette::Inactive] = other.d[QPalette::Inactive];
}

KStatefulBrush::~KStatefulBrush()
{
    delete[] d;
}

KStatefulBrush &KStatefulBrush::operator=(const KStatefulBrush &other)
{
    d[QPalette::Active] = other.d[QPalette::Active];
    d[QPalette::Disabled] = other.d[QPalette::Disabled];
    d[QPalette::Inactive] = other.d[QPalette::Inactive];
    return *this;
}

QBrush KStatefulBrush::brush(QPalette::ColorGroup state) const
{
    if (state >= QPalette::Active && state < QPalette::NColorGroups) {
        return d[state];
    } else {
        return d[QPalette::Active];
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
