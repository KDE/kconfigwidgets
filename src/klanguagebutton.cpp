/*
    SPDX-FileCopyrightText: 1999-2003 Hans Petter Bieker <bieker@kde.org>
    SPDX-FileCopyrightText: 2007 David Jarvie <djarvie@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "klanguagebutton.h"

#include <QComboBox>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QHBoxLayout>
#include <QLocale>
#include <QStylePainter>

#include <KConfig>
#include <KConfigGroup>

// Shows staticText instead of the current item when it is set.
class KLanguageComboBox : public QComboBox
{
public:
    using QComboBox::QComboBox;

    QString staticText;

protected:
    void paintEvent(QPaintEvent *event) override
    {
        if (staticText.isNull()) {
            QComboBox::paintEvent(event);
            return;
        }
        QStylePainter painter(this);
        QStyleOptionComboBox option;
        initStyleOption(&option);
        option.currentText = staticText;
        option.currentIcon = QIcon();
        painter.drawComplexControl(QStyle::CC_ComboBox, option);
        painter.drawControl(QStyle::CE_ComboBoxLabel, option);
    }
};

static void checkInsertPos(QComboBox *combo, const QString &str, int &index)
{
    if (index != -1) {
        return;
    }

    int a = 0;
    int b = combo->count();

    while (a < b) {
        int w = (a + b) / 2;
        int j = str.localeAwareCompare(combo->itemText(w));
        if (j > 0) {
            a = w + 1;
        } else {
            b = w;
        }
    }

    index = a; // it doesn't really matter ... a == b here.

    Q_ASSERT(a == b);
}

class KLanguageButtonPrivate
{
public:
    explicit KLanguageButtonPrivate(KLanguageButton *parent);
    void setCurrentItem(int index);
    void clear();

    KLanguageComboBox *combo = nullptr;
    QStringList ids;
    QString current;
    QString locale;
    bool showCodes : 1;
};

KLanguageButton::KLanguageButton(QWidget *parent)
    : QWidget(parent)
    , d(new KLanguageButtonPrivate(this))
{
}

KLanguageButton::KLanguageButton(const QString &text, QWidget *parent)
    : QWidget(parent)
    , d(new KLanguageButtonPrivate(this))
{
    setText(text);
}

KLanguageButtonPrivate::KLanguageButtonPrivate(KLanguageButton *parent)
    : combo(new KLanguageComboBox(parent))
    , locale(QLocale::system().name())
    , showCodes(false)
{
    QHBoxLayout *layout = new QHBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(combo);

    parent->setFocusProxy(combo);
    parent->setFocusPolicy(combo->focusPolicy());

    QObject::connect(combo, &QComboBox::currentIndexChanged, parent, [this](int index) {
        setCurrentItem(index);
    });
    QObject::connect(combo, &QComboBox::activated, parent, [this, parent] {
        Q_EMIT parent->activated(current);
    });
    QObject::connect(combo, &QComboBox::highlighted, parent, [this, parent](int index) {
        Q_EMIT parent->highlighted(combo->itemData(index).toString());
    });
}

KLanguageButton::~KLanguageButton() = default;

void KLanguageButton::setText(const QString &text)
{
    d->combo->staticText = text;
    d->combo->update();
}

void KLanguageButton::setLocale(const QString &locale)
{
    d->locale = locale;
}

void KLanguageButton::showLanguageCodes(bool show)
{
    d->showCodes = show;
}

static QString nameFromEntryFile(const QString &entryFile)
{
    const KConfig entry(entryFile, KConfig::SimpleConfig);
    const KConfigGroup group(&entry, QStringLiteral("KCM Locale"));
    return group.readEntry("Name", QString());
}

void KLanguageButton::insertLanguage(const QString &languageCode, const QString &name, int index)
{
    QString text;
    bool showCodes = d->showCodes;
    if (name.isEmpty()) {
        const QString entryFile =
            QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("locale/") + languageCode + QLatin1String("/kf6_entry.desktop"));
        if (QFile::exists(entryFile)) {
            text = nameFromEntryFile(entryFile);
        }

        if (text.isEmpty()) {
            text = languageCode;
            QLocale locale(languageCode);
            if (locale != QLocale::c()) {
                text = locale.nativeLanguageName();
                // For some languages the native name might be empty.
                // In this case use the non native language name as fallback.
                // See: QTBUG-51323
                text = text.isEmpty() ? QLocale::languageToString(locale.language()) : text;
            } else {
                showCodes = false;
            }
        }
    } else {
        text = name;
    }
    if (showCodes) {
        text += QLatin1String(" (") + languageCode + QLatin1Char(')');
    }

    checkInsertPos(d->combo, text, index);
    if (index < 0 || index > d->combo->count()) {
        index = d->combo->count();
    }
    d->combo->insertItem(index, text, languageCode);
    d->ids.append(languageCode);
}

void KLanguageButton::insertSeparator(int index)
{
    if (index < 0 || index > d->combo->count()) {
        index = d->combo->count();
    }
    d->combo->insertSeparator(index);
}

void KLanguageButton::loadAllLanguages()
{
    const QStringList localeDirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("locale"), QStandardPaths::LocateDirectory);
    for (const QString &localeDir : localeDirs) {
        const QStringList entries = QDir(localeDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        for (const QString &d : entries) {
            const QString entryFile = localeDir + QLatin1Char('/') + d + QStringLiteral("/kf6_entry.desktop");
            if (QFile::exists(entryFile)) {
                insertLanguage(d);
            }
        }
    }

    d->ids.removeDuplicates();
    setCurrentItem(d->locale);
}

int KLanguageButton::count() const
{
    return d->ids.count();
}

void KLanguageButton::clear()
{
    d->clear();
}

void KLanguageButtonPrivate::clear()
{
    ids.clear();
    combo->clear();
}

bool KLanguageButton::contains(const QString &languageCode) const
{
    return d->ids.contains(languageCode);
}

QString KLanguageButton::current() const
{
    return d->current.isEmpty() ? QStringLiteral("en") : d->current;
}

void KLanguageButton::setCurrentItem(const QString &languageCode)
{
    if (d->ids.isEmpty()) {
        return;
    }
    const int index = d->combo->findData(d->ids.contains(languageCode) ? languageCode : d->ids[0]);
    if (index >= 0) {
        d->combo->setCurrentIndex(index);
        d->setCurrentItem(index);
    }
}

void KLanguageButtonPrivate::setCurrentItem(int index)
{
    const QVariant data = combo->itemData(index);
    if (!data.isValid()) {
        return;
    }
    current = data.toString();
}

bool KLanguageButton::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTipChange) {
        d->combo->setToolTip(toolTip());
    }
    return QWidget::event(event);
}

#include "moc_klanguagebutton.cpp"
