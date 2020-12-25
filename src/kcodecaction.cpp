/*
    SPDX-FileCopyrightText: 2003 Jason Keirstead <jason@keirstead.org>
    SPDX-FileCopyrightText: 2006 Michel Hermier <michel.hermier@gmail.com>
    SPDX-FileCopyrightText: 2007 Nick Shaforostoff <shafff@ukr.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kcodecaction.h"
#include "kconfigwidgets_debug.h"

#include <KCharsets>
#include <KLocalizedString>

#include <QMenu>
#include <QVariant>
#include <QTextCodec>

// According to http://www.iana.org/assignments/ianacharset-mib
// the default/unknown mib value is 2.
#define MIB_DEFAULT 2

class Q_DECL_HIDDEN KCodecAction::Private
{
public:
    Private(KCodecAction *parent)
        : q(parent)
    {
    }

    void init(bool);

    void _k_subActionTriggered(QAction *);

    KCodecAction * const q;
    QAction *defaultAction = nullptr;
    QAction *currentSubAction = nullptr;
};

KCodecAction::KCodecAction(QObject *parent, bool showAutoOptions)
    : KSelectAction(parent)
    , d(new Private(this))
{
    d->init(showAutoOptions);
}

KCodecAction::KCodecAction(const QString &text, QObject *parent, bool showAutoOptions)
    : KSelectAction(text, parent)
    , d(new Private(this))
{
    d->init(showAutoOptions);
}

KCodecAction::KCodecAction(const QIcon &icon, const QString &text, QObject *parent, bool showAutoOptions)
    : KSelectAction(icon, text, parent)
    , d(new Private(this))
{
    d->init(showAutoOptions);
}

KCodecAction::~KCodecAction()
{
    delete d;
}

void KCodecAction::Private::init(bool showAutoOptions)
{
    q->setToolBarMode(MenuMode);
    defaultAction = q->addAction(i18nc("Encodings menu", "Default"));

    const auto lstEncodings = KCharsets::charsets()->encodingsByScript();
    for (const QStringList &encodingsForScript : lstEncodings) {
        KSelectAction *tmp = new KSelectAction(encodingsForScript.at(0), q);
        if (showAutoOptions) {
            KEncodingProber::ProberType scri = KEncodingProber::proberTypeForName(encodingsForScript.at(0));
            if (scri != KEncodingProber::None) {
                tmp->addAction(i18nc("Encodings menu", "Autodetect"))->setData(QVariant((uint)scri));
                tmp->menu()->addSeparator();
            }
        }
        for (int i = 1; i < encodingsForScript.size(); ++i) {
            tmp->addAction(encodingsForScript.at(i));
        }
        q->connect(tmp, SIGNAL(triggered(QAction*)), q, SLOT(_k_subActionTriggered(QAction*)));
        tmp->setCheckable(true);
        q->addAction(tmp);
    }
    q->setCurrentItem(0);

#if KCONFIGWIDGETS_BUILD_DEPRECATED_SINCE(5, 78)
    // forward deprecated signals to undeprecated, to be backward-compatible to unported subclasses
    QObject::connect(q, QOverload<QTextCodec *>::of(&KCodecAction::triggered),
                     q, &KCodecAction::codecTriggered);
    QObject::connect(q, QOverload<KEncodingProber::ProberType>::of(&KCodecAction::triggered),
                     q, &KCodecAction::encodingProberTriggered);
#endif
}

int KCodecAction::mibForName(const QString &codecName, bool *ok) const
{
    // FIXME logic is good but code is ugly

    bool success = false;
    int mib = MIB_DEFAULT;
    KCharsets *charsets = KCharsets::charsets();

    if (codecName == d->defaultAction->text()) {
        success = true;
    } else {
        QTextCodec *codec = charsets->codecForName(codecName, success);
        if (!success) {
            // Maybe we got a description name instead
            codec = charsets->codecForName(charsets->encodingForName(codecName), success);
        }

        if (codec) {
            mib = codec->mibEnum();
        }
    }

    if (ok) {
        *ok = success;
    }

    if (success) {
        return mib;
    }

    qCWarning(KCONFIG_WIDGETS_LOG) << "Invalid codec name: "  << codecName;
    return MIB_DEFAULT;
}

QTextCodec *KCodecAction::codecForMib(int mib) const
{
    if (mib == MIB_DEFAULT) {
        // FIXME offer to change the default codec
        return QTextCodec::codecForLocale();
    } else {
        return QTextCodec::codecForMib(mib);
    }
}

void KCodecAction::actionTriggered(QAction *action)
{
//we don't want to emit any signals from top-level items
//except for the default one
    if (action == d->defaultAction) {
#if KCONFIGWIDGETS_BUILD_DEPRECATED_SINCE(5, 78)
        emit triggered(KEncodingProber::Universal);
#else
        emit encodingProberTriggered(KEncodingProber::Universal);
#endif
        emit defaultItemTriggered();
    }
}

void KCodecAction::Private::_k_subActionTriggered(QAction *action)
{
    if (currentSubAction == action) {
        return;
    }
    currentSubAction = action;
    bool ok = false;
    int mib = q->mibForName(action->text(), &ok);
    if (ok) {
#if KWIDGETSADDONS_BUILD_DEPRECATED_SINCE(5, 78)
QT_WARNING_PUSH
QT_WARNING_DISABLE_CLANG("-Wdeprecated-declarations")
QT_WARNING_DISABLE_GCC("-Wdeprecated-declarations")
        // will also indirectly emit textTriggered, due to signal connection in KSelectAction
        emit q->triggered(action->text());
QT_WARNING_POP
#else
        emit q->textTriggered(action->text());
#endif
        QTextCodec *codec = q->codecForMib(mib);
#if KCONFIGWIDGETS_BUILD_DEPRECATED_SINCE(5, 78)
        // will also indirectly emit codecTriggered, due to signal connection in init()
        emit q->triggered(codec);
#else
        emit q->codecTriggered(codec);
#endif
    } else {
        if (!action->data().isNull()) {
            const auto encodingProberType = static_cast<KEncodingProber::ProberType>(action->data().toUInt());
#if KCONFIGWIDGETS_BUILD_DEPRECATED_SINCE(5, 78)
            // will also indirectly emit encodingProberTriggered, due to signal connection in init()
            emit q->triggered(encodingProberType);
#else
            emit q->encodingProberTriggered(encodingProberType);
#endif
        }
    }
}

QTextCodec *KCodecAction::currentCodec() const
{
    return codecForMib(currentCodecMib());
}

bool KCodecAction::setCurrentCodec(QTextCodec *codec)
{
    if (!codec) {
        return false;
    }

    for (int i = 0; i < actions().size(); ++i) {
        if (actions().at(i)->menu()) {
            for (int j = 0; j < actions().at(i)->menu()->actions().size(); ++j) {
                if (!j && !actions().at(i)->menu()->actions().at(j)->data().isNull()) {
                    continue;
                }
                if (codec == KCharsets::charsets()->codecForName(actions().at(i)->menu()->actions().at(j)->text())) {
                    d->currentSubAction = actions().at(i)->menu()->actions().at(j);
                    d->currentSubAction->trigger();
                    return true;
                }
            }
        }
    }
    return false;

}

QString KCodecAction::currentCodecName() const
{
    return d->currentSubAction->text();
}

bool KCodecAction::setCurrentCodec(const QString &codecName)
{
    return setCurrentCodec(KCharsets::charsets()->codecForName(codecName));
}

int KCodecAction::currentCodecMib() const
{
    return mibForName(currentCodecName());
}

bool KCodecAction::setCurrentCodec(int mib)
{
    if (mib == MIB_DEFAULT) {
        return setCurrentAction(d->defaultAction);
    } else {
        return setCurrentCodec(codecForMib(mib));
    }
}

KEncodingProber::ProberType KCodecAction::currentProberType() const
{
    return d->currentSubAction->data().isNull() ?
           KEncodingProber::None            :
           (KEncodingProber::ProberType)d->currentSubAction->data().toUInt();
}

bool KCodecAction::setCurrentProberType(KEncodingProber::ProberType scri)
{
    if (scri == KEncodingProber::Universal) {
        d->currentSubAction = d->defaultAction;
        d->currentSubAction->trigger();
        return true;
    }

    for (int i = 0; i < actions().size(); ++i) {
        if (actions().at(i)->menu()) {
            if (!actions().at(i)->menu()->actions().isEmpty()
                    && !actions().at(i)->menu()->actions().at(0)->data().isNull()
                    && actions().at(i)->menu()->actions().at(0)->data().toUInt() == (uint)scri
               ) {
                d->currentSubAction = actions().at(i)->menu()->actions().at(0);
                d->currentSubAction->trigger();
                return true;
            }
        }
    }
    return false;
}

#include "moc_kcodecaction.cpp"
