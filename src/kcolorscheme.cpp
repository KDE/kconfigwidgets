/*
    This file is part of the KDE project
    SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kcolorscheme.h"

#include <KColorUtils>

#include <QBrush>
#include <QColor>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QStandardPaths>
#if KCONFIGWIDGETS_BUILD_DEPRECATED_SINCE(5, 84)
#include <QWidget>
#endif

static QColor readColor(const QSettings *colorScheme, const QString &group, const QString &key, const QColor &defaultColor)
{
    const QString colorKey = group.isEmpty() ? key : group + QChar('/') + key;
    const auto colorList = colorScheme->value(colorKey).toStringList();
    const auto size = colorList.size();
    if (size < 3 || size > 4) {
        return defaultColor;
    }

    bool ok;
    std::array<int, 4> rgbComponents;
    rgbComponents[4] = 255;
    for (int i = 0; i < size; ++i) {
        rgbComponents[i] = colorList[i].toInt(&ok);
        if (!ok) {
            return defaultColor;
        }
    }
    return QColor(rgbComponents[0], rgbComponents[1], rgbComponents[2], rgbComponents[3]);
}

// BEGIN StateEffects
class StateEffects
{
public:
    explicit StateEffects(QPalette::ColorGroup state, QSettings * colorScheme);
    ~StateEffects()
    {
    } //{ delete chain; } not needed yet

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
    //     StateEffects *_chain; not needed yet
};

StateEffects::StateEffects(QPalette::ColorGroup state, QSettings *colorScheme)
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
    if (colorScheme->childGroups().contains(group)) {
        const bool enabledByDefault = (state == QPalette::Disabled);
        colorScheme->beginGroup(group);
        if (colorScheme->value("Enable", enabledByDefault).toBool()) {
            _effects[Intensity] = colorScheme->value(QStringLiteral("IntensityEffect"), enabledByDefault ? IntensityDarken : IntensityNoEffect).toInt();
            _effects[Color] = colorScheme->value(QStringLiteral("ColorEffect"), enabledByDefault ? ColorNoEffect : ColorDesaturate).toInt();
            _effects[Contrast] = colorScheme->value(QStringLiteral("ContrastEffect"), enabledByDefault ? ContrastFade : ContrastTint).toInt();
            _amount[Intensity] = colorScheme->value(QStringLiteral("IntensityAmount"), enabledByDefault ? 0.10 : 0.0).toDouble();
            _amount[Color] = colorScheme->value(QStringLiteral("ColorAmount"), enabledByDefault ? 0.0 : -0.9).toDouble();
            _amount[Contrast] = colorScheme->value(QStringLiteral("ContrastAmount"), enabledByDefault ? 0.65 : 0.25).toDouble();
            if (_effects[Color] > ColorNoEffect) {
                _color = readColor(colorScheme, QString(), QStringLiteral("Color"), enabledByDefault ? QColor(56, 56, 56) : QColor(112, 111, 110));
            }
        }
        colorScheme->endGroup();
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
// END StateEffects

// BEGIN default colors
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

// clang-format off
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
// clang-format off

QSettings *defaultColorScheme() {
    // cache the value we'll return, since usually it's going to be the same value
    // Read from the application's color scheme file (as set by KColorSchemeManager).
    // If unset, this is equivalent to openConfig() and the system scheme is used.
    const auto colorSchemePath = qApp->property("KDE_COLOR_SCHEME_PATH").toString();
    if (!colorSchemePath.isEmpty()) {
        return new QSettings(colorSchemePath, QSettings::IniFormat);
    }

    const auto files = QStandardPaths::locateAll(QStandardPaths::GenericConfigLocation, QStringLiteral("kdeglobals"), QStandardPaths::LocateFile);
    const QString key = QStringLiteral("ColorScheme");
    for (const auto &file : files) {
        QSettings settings(file, QSettings::IniFormat);
        if (settings.childKeys().contains(key)) {
            const auto path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("colors/") + settings.value(key).toString());
            return new QSettings(path, QSettings::IniFormat);
        }
    }
   return new QSettings;
}

//BEGIN KColorSchemePrivate
class KColorSchemePrivate : public QSharedData
{
public:
    explicit KColorSchemePrivate(QSettings* , QPalette::ColorGroup, const QString &, const SetDefaultColors &);
    explicit KColorSchemePrivate(QSettings* , QPalette::ColorGroup, const QString &, const SetDefaultColors &, const QBrush &);
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

    void init(QSettings *, QPalette::ColorGroup, const QString &, const SetDefaultColors &);
};

#define DEFAULT(c) QColor( c[0], c[1], c[2] )
#define  SET_DEFAULT(a) DEFAULT( defaults.a )
#define DECO_DEFAULT(a) DEFAULT( defaultDecorationColors.a )

KColorSchemePrivate::KColorSchemePrivate(QSettings* colorScheme,
        QPalette::ColorGroup state,
        const QString &group,
        const SetDefaultColors &defaults)
{
   _contrast = colorScheme->value(QStringLiteral("KDE/contrast"), 7).toInt() * 0.1;

    // the rest
    init(colorScheme, state, group, defaults);
}

KColorSchemePrivate::KColorSchemePrivate(QSettings* colorScheme,
        QPalette::ColorGroup state,
        const QString &group,
        const SetDefaultColors &defaults,
        const QBrush &tint)
    : KColorSchemePrivate(colorScheme, state, group, defaults)
{
    // adjustment
     _brushes.bg[KColorScheme::NormalBackground] = 
        KColorUtils::tint(_brushes.bg[KColorScheme::NormalBackground].color(), tint.color(), 0.4);
    _brushes.bg[KColorScheme::AlternateBackground] = 
        KColorUtils::tint(_brushes.bg[KColorScheme::AlternateBackground].color(), tint.color(), 0.4);

}

void KColorSchemePrivate::init(QSettings *colorScheme,
                               QPalette::ColorGroup state,
                               const QString &group,
                               const SetDefaultColors &defaults)
{
    bool hasInactivePalette = false;
    QString actualGroup = group;
    colorScheme->beginGroup(group);
    if (state == QPalette::Inactive && colorScheme->childGroups().contains(QStringLiteral("Inactive"))) {
        actualGroup = group + QChar('/') + QStringLiteral("Inactive");
        hasInactivePalette = true;
        colorScheme->endGroup();
    }

    // loaded-from-config colors
    const std::map<KColorScheme::ForegroundRole, QString> foregroundNames = {
        {KColorScheme::NormalText, QStringLiteral("ForegroundNormal")},
        {KColorScheme::InactiveText, QStringLiteral("ForegroundInactive")},
        {KColorScheme::ActiveText, QStringLiteral("ForegroundActive")},
        {KColorScheme::LinkText, QStringLiteral("ForegroundLink")},
        {KColorScheme::VisitedText, QStringLiteral("ForegroundVisited")},
        {KColorScheme::NegativeText, QStringLiteral("ForegroundNegative")},
        {KColorScheme::NeutralText, QStringLiteral("ForegroundNeutral")},
        {KColorScheme::PositiveText, QStringLiteral("ForegroundPositive")},
    };
    const std::map<KColorScheme::BackgroundRole, QString> backgroundNames = {  
        {KColorScheme::NormalBackground, QStringLiteral("BackgroundNormal")},
        {KColorScheme::AlternateBackground, QStringLiteral("BackgroundAlternate")},
    };
     const std::map<KColorScheme::DecorationRole, QString> decorationNames = {
        {KColorScheme::FocusColor, QStringLiteral("DecorationFocus")},
        {KColorScheme::HoverColor, QStringLiteral("DecorationHover")},
    };

    auto color = [colorScheme] (const QString &group, const QString &key, const QColor &defaultColor) {
        return readColor(colorScheme, group, key, defaultColor);
    };

    if (group.startsWith(QLatin1String("Colors:Header"))) { // For compatibility with color schemes made before ColorSet::Header was added
        // Default to Window colors before using Header default colors
        const QString windowGroup = QStringLiteral("Colors:Window");
        _brushes.fg[KColorScheme::NormalText] = color(actualGroup, foregroundNames.at(KColorScheme::NormalText),
            color(windowGroup, foregroundNames.at(KColorScheme::NormalText), SET_DEFAULT(NormalText)));
        _brushes.fg[KColorScheme::InactiveText] = color(actualGroup, foregroundNames.at(KColorScheme::InactiveText),
            color(windowGroup, foregroundNames.at(KColorScheme::InactiveText), SET_DEFAULT(InactiveText)));
        _brushes.fg[KColorScheme::ActiveText] = color(actualGroup, foregroundNames.at(KColorScheme::ActiveText),
            color(windowGroup, foregroundNames.at(KColorScheme::ActiveText), SET_DEFAULT(ActiveText)));
        _brushes.fg[KColorScheme::LinkText] = color(actualGroup, foregroundNames.at(KColorScheme::LinkText),
            color(windowGroup, foregroundNames.at(KColorScheme::LinkText), SET_DEFAULT(LinkText)));
        _brushes.fg[KColorScheme::VisitedText] = color(actualGroup, foregroundNames.at(KColorScheme::VisitedText),
            color(windowGroup, foregroundNames.at(KColorScheme::VisitedText), SET_DEFAULT(VisitedText)));
        _brushes.fg[KColorScheme::NegativeText] = color(actualGroup, foregroundNames.at(KColorScheme::NegativeText),
            color(windowGroup, foregroundNames.at(KColorScheme::NegativeText), SET_DEFAULT(NegativeText)));
        _brushes.fg[KColorScheme::NeutralText] = color(actualGroup, foregroundNames.at(KColorScheme::NeutralText),
            color(windowGroup, foregroundNames.at(KColorScheme::NeutralText), SET_DEFAULT(NeutralText)));
        _brushes.fg[KColorScheme::PositiveText] = color(actualGroup, foregroundNames.at(KColorScheme::PositiveText),
            color(windowGroup, foregroundNames.at(KColorScheme::PositiveText), SET_DEFAULT(PositiveText)));

        _brushes.bg[KColorScheme::NormalBackground] = color(actualGroup, backgroundNames.at(KColorScheme::NormalBackground),
            color(windowGroup, backgroundNames.at(KColorScheme::NormalBackground), SET_DEFAULT(NormalBackground)));
        _brushes.bg[KColorScheme::AlternateBackground] = color(actualGroup, backgroundNames.at(KColorScheme::AlternateBackground),
            color(windowGroup, backgroundNames.at(KColorScheme::AlternateBackground), SET_DEFAULT(AlternateBackground)));

        _brushes.bg[KColorScheme::FocusColor] = color(actualGroup, decorationNames.at(KColorScheme::FocusColor),
            color(windowGroup, decorationNames.at(KColorScheme::FocusColor), DECO_DEFAULT(Focus)));
        _brushes.bg[KColorScheme::HoverColor] = color(actualGroup, decorationNames.at(KColorScheme::HoverColor),
            color(windowGroup, decorationNames.at(KColorScheme::HoverColor), DECO_DEFAULT(Hover)));
    } else {
        _brushes.fg[KColorScheme::NormalText] = color(actualGroup, foregroundNames.at(KColorScheme::NormalText), SET_DEFAULT(NormalText));
        _brushes.fg[KColorScheme::InactiveText] = color(actualGroup, foregroundNames.at(KColorScheme::InactiveText), SET_DEFAULT(InactiveText));
        _brushes.fg[KColorScheme::ActiveText] = color(actualGroup, foregroundNames.at(KColorScheme::ActiveText), SET_DEFAULT(ActiveText));
        _brushes.fg[KColorScheme::LinkText] = color(actualGroup, foregroundNames.at(KColorScheme::LinkText), SET_DEFAULT(LinkText));
        _brushes.fg[KColorScheme::VisitedText] = color(actualGroup, foregroundNames.at(KColorScheme::VisitedText), SET_DEFAULT(VisitedText));
        _brushes.fg[KColorScheme::NegativeText] = color(actualGroup, foregroundNames.at(KColorScheme::NegativeText), SET_DEFAULT(NegativeText));
        _brushes.fg[KColorScheme::NeutralText] = color(actualGroup, foregroundNames.at(KColorScheme::NeutralText), SET_DEFAULT(NeutralText));
        _brushes.fg[KColorScheme::PositiveText] = color(actualGroup, foregroundNames.at(KColorScheme::PositiveText), SET_DEFAULT(PositiveText));

        _brushes.bg[KColorScheme::NormalBackground] = color(actualGroup, backgroundNames.at(KColorScheme::NormalBackground), SET_DEFAULT(NormalBackground));
        _brushes.bg[KColorScheme::AlternateBackground] = color(actualGroup, backgroundNames.at(KColorScheme::AlternateBackground), SET_DEFAULT(AlternateBackground));

        _brushes.bg[KColorScheme::FocusColor] = color(actualGroup, decorationNames.at(KColorScheme::FocusColor), DECO_DEFAULT(Focus));
        _brushes.bg[KColorScheme::HoverColor] = color(actualGroup, decorationNames.at(KColorScheme::HoverColor), DECO_DEFAULT(Hover));
    }

    // apply state adjustments
    if (state != QPalette::Active || (state == QPalette::Inactive && !hasInactivePalette)) {
        StateEffects effects(state, colorScheme);
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

KColorScheme::KColorScheme(QPalette::ColorGroup state, ColorSet set, const QString &fileName)
{
    QSettings *colorScheme;
    const QString colorSchemePath = QDir::isAbsolutePath(fileName) ? fileName : QStandardPaths::locate(QStandardPaths::GenericDataLocation, fileName);
    if (colorSchemePath.isEmpty() || !colorSchemePath.endsWith(QLatin1String(".colors"))) {
        colorScheme = defaultColorScheme();
    } else {
        colorScheme = new QSettings(colorSchemePath, QSettings::IniFormat);
    }

    switch (set) {
    case Window:
        d = new KColorSchemePrivate(colorScheme, state, QStringLiteral("Colors:Window"), defaultWindowColors);
        break;
    case Button:
        d = new KColorSchemePrivate(colorScheme, state, QStringLiteral("Colors:Button"), defaultButtonColors);
        break;
    case Selection: {
        const QString inactivEffectPrefix = QStringLiteral("ColorEffects:Inactive/");
        // NOTE: keep this in sync with kdebase/workspace/kcontrol/colors/colorscm.cpp
        bool inactiveSelectionEffect = colorScheme->value(inactivEffectPrefix + QStringLiteral("ChangeSelectionColor"),
            colorScheme->value(inactivEffectPrefix + QStringLiteral("Enable"), true)).toBool();
        // if enabled, inactiver/disabled uses Window colors instead, ala gtk
        // ...except tinted with the Selection:NormalBackground color so it looks more like selection
        if (state == QPalette::Active || (state == QPalette::Inactive && !inactiveSelectionEffect)) {
            d = new KColorSchemePrivate(colorScheme, state, QStringLiteral("Colors:Selection"), defaultSelectionColors);
        } else if (state == QPalette::Inactive)  {
            d = new KColorSchemePrivate(colorScheme, state, QStringLiteral("Colors:Window"), defaultWindowColors,
                                        KColorScheme(QPalette::Active, Selection, colorSchemePath).background());
        } else { // disabled (...and still want this branch when inactive+disabled exists)
            d = new KColorSchemePrivate(colorScheme, state, QStringLiteral("Colors:Window"), defaultWindowColors);
        }
    } break;
    case Tooltip:
        d = new KColorSchemePrivate(colorScheme, state, QStringLiteral("Colors:Tooltip"), defaultTooltipColors);
        break;
    case Complementary:
        d = new KColorSchemePrivate(colorScheme, state, QStringLiteral("Colors:Complementary"), defaultComplementaryColors);
        break;
    case Header:
        d = new KColorSchemePrivate(colorScheme, state, QStringLiteral("Colors:Header"), defaultHeaderColors);
        break;
    default:
        d = new KColorSchemePrivate(colorScheme, state, QStringLiteral("Colors:View"), defaultViewColors);
    }
}

// static
int KColorScheme::defaultContrast()
{
    return defaultColorScheme()->value(QStringLiteral("KDE/contrast"), 7).toInt();
}

qreal KColorScheme::colorSchemeContrast()
{
    return d->contrast();
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
    return shade(color, role, defaultContrast() * 0.1);
}

QColor KColorScheme::shade(const QColor &color, ShadeRole role, qreal contrast, qreal chromaAdjust)
{
    // nan -> 1.0
    contrast = (1.0 > contrast ? (-1.0 < contrast ? contrast : -1.0) : 1.0);
    qreal y = KColorUtils::luma(color);
    qreal yi = 1.0 - y;

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
                                    ColorSet set, const QString &fileName)
{
    palette.setBrush(QPalette::Active,   color, KColorScheme(QPalette::Active,   set, fileName).background(newRole));
    palette.setBrush(QPalette::Inactive, color, KColorScheme(QPalette::Inactive, set, fileName).background(newRole));
    palette.setBrush(QPalette::Disabled, color, KColorScheme(QPalette::Disabled, set, fileName).background(newRole));
}

void KColorScheme::adjustForeground(QPalette &palette, ForegroundRole newRole, QPalette::ColorRole color,
                                    ColorSet set, const QString &fileName)
{
    palette.setBrush(QPalette::Active,   color, KColorScheme(QPalette::Active,   set, fileName).foreground(newRole));
    palette.setBrush(QPalette::Inactive, color, KColorScheme(QPalette::Inactive, set, fileName).foreground(newRole));
    palette.setBrush(QPalette::Disabled, color, KColorScheme(QPalette::Disabled, set, fileName).foreground(newRole));
}

bool KColorScheme::isColorSetSupported(const QString &fileName, KColorScheme::ColorSet set)
{
    const QString colorSchemePath = QDir::isAbsolutePath(fileName) ? fileName : QStandardPaths::locate(QStandardPaths::GenericDataLocation, fileName);
    if (fileName.isEmpty() || !fileName.endsWith(QLatin1String(".colors"))) {
        return false;
    }
    QSettings colorScheme (colorSchemePath, QSettings::IniFormat);
    switch (set) {
        case View:
            return colorScheme.childGroups().contains("Colors:View");
        case Window:
            return colorScheme.childGroups().contains("Colors:Window");
        case Button:
            return colorScheme.childGroups().contains("Colors:Button");
        case Selection:
            return colorScheme.childGroups().contains("Colors:Selection");
        case Tooltip:
            return colorScheme.childGroups().contains("Colors:Tooltip");
        case Complementary:
            return colorScheme.childGroups().contains("Colors:Complementary");
        case Header:
            return colorScheme.childGroups().contains("Colors:Header");
    }

    return false;
}

QPalette KColorScheme::createApplicationPalette(const QString &fileName)
{
    QPalette palette;

    static const QPalette::ColorGroup states[QPalette::NColorGroups] = {
        QPalette::Active, QPalette::Inactive, QPalette::Disabled
    };

    // TT thinks tooltips shouldn't use active, so we use our active colors for all states
    KColorScheme schemeTooltip(QPalette::Active, KColorScheme::Tooltip, fileName);

    for (auto state : states) {
        KColorScheme schemeView(state, KColorScheme::View, fileName);
        KColorScheme schemeWindow(state, KColorScheme::Window, fileName);
        KColorScheme schemeButton(state, KColorScheme::Button, fileName);
        KColorScheme schemeSelection(state, KColorScheme::Selection, fileName);

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
                              const QString &fileName)
{
    d = new KStatefulBrushPrivate[QPalette::NColorGroups];
    d[QPalette::Active] = KColorScheme(QPalette::Active,   set, fileName).foreground(role);
    d[QPalette::Disabled] = KColorScheme(QPalette::Disabled, set, fileName).foreground(role);
    d[QPalette::Inactive] = KColorScheme(QPalette::Inactive, set, fileName).foreground(role);
}

KStatefulBrush::KStatefulBrush(KColorScheme::ColorSet set, KColorScheme::BackgroundRole role,
                               const QString &fileName)
{
    d = new KStatefulBrushPrivate[QPalette::NColorGroups];
    d[QPalette::Active] = KColorScheme(QPalette::Active,   set, fileName).background(role);
    d[QPalette::Disabled] = KColorScheme(QPalette::Disabled, set, fileName).background(role);
    d[QPalette::Inactive] = KColorScheme(QPalette::Inactive, set, fileName).background(role);
}

KStatefulBrush::KStatefulBrush(KColorScheme::ColorSet set, KColorScheme::DecorationRole role,
                              const QString &fileName)
{
    d = new KStatefulBrushPrivate[QPalette::NColorGroups];
    d[QPalette::Active] = KColorScheme(QPalette::Active,   set, fileName).decoration(role);
    d[QPalette::Disabled] = KColorScheme(QPalette::Disabled, set, fileName).decoration(role);
    d[QPalette::Inactive] = KColorScheme(QPalette::Inactive, set, fileName).decoration(role);
}

KStatefulBrush::KStatefulBrush(const QBrush &brush, const QString &fileName)
{
    QSettings *colorScheme;
    const QString colorSchemePath = QDir::isAbsolutePath(fileName) ? fileName : QStandardPaths::locate(QStandardPaths::GenericDataLocation, fileName);
    if (colorSchemePath.isEmpty() || !colorSchemePath.endsWith(QLatin1String(".colors"))) {
        colorScheme = defaultColorScheme();
    } else {
        colorScheme = new QSettings(colorSchemePath, QSettings::IniFormat);
    }
    d = new KStatefulBrushPrivate[QPalette::NColorGroups];
    d[QPalette::Active] = brush;
    d[QPalette::Disabled] = StateEffects(QPalette::Disabled, colorScheme).brush(brush);
    d[QPalette::Inactive] = StateEffects(QPalette::Inactive, colorScheme).brush(brush);
}

KStatefulBrush::KStatefulBrush(const QBrush &brush, const QBrush &background,
                                const QString &fileName)
{
    QSettings *colorScheme;
    const QString colorSchemePath = QDir::isAbsolutePath(fileName) ? fileName : QStandardPaths::locate(QStandardPaths::GenericDataLocation, fileName);
    if (colorSchemePath.isEmpty() || !colorSchemePath.endsWith(QLatin1String(".colors"))) {
        colorScheme = defaultColorScheme();
    } else {
        colorScheme = new QSettings(colorSchemePath, QSettings::IniFormat);
    }
    d = new KStatefulBrushPrivate[QPalette::NColorGroups];
    d[QPalette::Active] = brush;
    d[QPalette::Disabled] = StateEffects(QPalette::Disabled, colorScheme).brush(brush, background);
    d[QPalette::Inactive] = StateEffects(QPalette::Inactive, colorScheme).brush(brush, background);
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

#if KCONFIGWIDGETS_BUILD_DEPRECATED_SINCE(5, 84)
QBrush KStatefulBrush::brush(const QWidget *widget) const
{
    if (widget) {
        return brush(widget->palette());
    } else {
        return QBrush();
    }
}
#endif
//END KStatefulBrush
