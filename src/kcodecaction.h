/*
    SPDX-FileCopyrightText: 2003 Jason Keirstead <jason@keirstead.org>
    SPDX-FileCopyrightText: 2003-2006 Michel Hermier <michel.hermier@gmail.com>
    SPDX-FileCopyrightText: 2007 Nick Shaforostoff <shafff@ukr.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KCODECACTION_H
#define KCODECACTION_H

#include <KEncodingProber>
#include <KSelectAction>
#include <kconfigwidgets_export.h>

/**
 *  @class KCodecAction kcodecaction.h KCodecAction
 *
 *  @short Action for selecting one of several QTextCodec.
 *
 *  This action shows up a submenu with a list of the available codecs on the system.
 */
class KCONFIGWIDGETS_EXPORT KCodecAction
    : public KSelectAction
{
    Q_OBJECT

    Q_PROPERTY(QString codecName READ currentCodecName WRITE setCurrentCodec)
    Q_PROPERTY(int codecMib READ currentCodecMib)

public:
    explicit KCodecAction(QObject *parent, bool showAutoOptions = false);

    KCodecAction(const QString &text, QObject *parent, bool showAutoOptions = false);

    KCodecAction(const QIcon &icon, const QString &text, QObject *parent, bool showAutoOptions = false);

    ~KCodecAction() override;

public:
    int mibForName(const QString &codecName, bool *ok = nullptr) const;
    QTextCodec *codecForMib(int mib) const;

    QTextCodec *currentCodec() const;
    bool setCurrentCodec(QTextCodec *codec);

    QString currentCodecName() const;
    bool setCurrentCodec(const QString &codecName);

    int currentCodecMib() const;
    bool setCurrentCodec(int mib);

    /**
     * Applicable only if showAutoOptions in c'tor was true
     *
     * @returns KEncodingProber::None if specific encoding is selected, not autodetection, otherwise... you know it!
     */
    KEncodingProber::ProberType currentProberType() const;
    /**
     * Applicable only if showAutoOptions in c'tor was true
     *
     * KEncodingProber::Universal means 'Default' item
     */
    bool setCurrentProberType(KEncodingProber::ProberType);

Q_SIGNALS:
    /**
     * Specific (proper) codec was selected
     *
     * Note that triggered(const QString&) is emitted too (as defined in KSelectAction)
     */
    void triggered(QTextCodec *codec);

    /**
     * Autodetection has been selected.
     * emits KEncodingProber::Universal if Default was selected.
     *
     * Applicable only if showAutoOptions in c'tor was true
     */
    void triggered(KEncodingProber::ProberType);

    /**
     * If showAutoOptions==true, then better handle triggered(KEncodingProber::ProberType) signal
     */
    void defaultItemTriggered();

protected Q_SLOTS:
    void actionTriggered(QAction *) override;

protected:
    using KSelectAction::triggered;

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_subActionTriggered(QAction *))
};

#endif
