/*
 * SPDX-FileCopyrightText: 2020 Piyush Aggarwal <piyushaggarwal002@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef WindowsMessagesNotifier_H
#define WindowsMessagesNotifier_H

#include <QSettings>
#include <QAbstractNativeEventFilter>
#include <QAbstractEventDispatcher>
#include <QDebug>

#include <windows.h>

class WindowsMessagesNotifier
    : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    WindowsMessagesNotifier();
    ~WindowsMessagesNotifier();
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE;
    void handleWMSettingChange();
    bool preferDarkMode();

Q_SIGNALS:
    void wm_colorSchemeChanged();

private:
    const QString m_subKey {QStringLiteral("Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize")};
    QSettings m_settings {QStringLiteral("HKEY_CURRENT_USER\\") + m_subKey, QSettings::NativeFormat};
    bool m_preferDarkMode = false;
};

#endif // WindowsMessagesNotifier_H
