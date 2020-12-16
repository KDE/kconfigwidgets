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
#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(5, 78)
    /**
     * Specific (proper) codec was selected
     *
     * Note that triggered(const QString &) is emitted too (as defined in KSelectAction).
     * @deprecated Since 5.78, use codecTriggered(QTextCodec *)
     */
    KCONFIGWIDGETS_DEPRECATED_VERSION(5, 78, "Use KCodecAction::codecTriggered(QTextCodec *)")
    void triggered(QTextCodec *codec);
#endif
    /**
     * Specific (proper) codec was selected
     *
     * Note that textTriggered(const QString &) is emitted too (as defined in KSelectAction).
     * @since 5.78
     */
    void codecTriggered(QTextCodec *codec);

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(5, 78)
    /**
     * Autodetection has been selected.
     * emits KEncodingProber::Universal if Default was selected.
     *
     * Applicable only if showAutoOptions in c'tor was true
     * @deprecated Since 5.78, use encodingProberTriggered(KEncodingProber::ProberType)
     */
    KCONFIGWIDGETS_DEPRECATED_VERSION(5, 78, "Use KCodecAction::encodingProberTriggered(KEncodingProber::ProberType)")
    void triggered(KEncodingProber::ProberType);
#endif

    /**
     * Autodetection has been selected.
     * emits KEncodingProber::Universal if Default was selected.
     *
     * Applicable only if showAutoOptions in c'tor was true
     * @since 5.78
     */
    void encodingProberTriggered(KEncodingProber::ProberType);

    /**
     * If @p showAutoOptions is @c true, then better handle the signal
     * encodingProberTriggered(KEncodingProber::ProberType)
     * (or triggered(KEncodingProber::ProberType), to support also KF < 5.78).
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
