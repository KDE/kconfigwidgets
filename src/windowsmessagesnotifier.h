/*
 * SPDX-FileCopyrightText: 2020 Piyush Aggarwal <piyushaggarwal002@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef WindowsMessagesNotifier_H
#define WindowsMessagesNotifier_H

#include <QAbstractEventDispatcher>
#include <QAbstractNativeEventFilter>
#include <QDebug>
#include <QSettings>

#include <windows.h>

class WindowsMessagesNotifier : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    WindowsMessagesNotifier();
    ~WindowsMessagesNotifier();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *) override;
#else
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *) override;
#endif
    void handleWMSettingChange();
    bool preferDarkMode();

Q_SIGNALS:
    void wm_colorSchemeChanged();

private:
    const QString m_subKey{QStringLiteral("Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize")};
    QSettings m_settings{QStringLiteral("HKEY_CURRENT_USER\\") + m_subKey, QSettings::NativeFormat};
    bool m_preferDarkMode = false;
};

#endif // WindowsMessagesNotifier_H
