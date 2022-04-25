/*
 * SPDX-FileCopyrightText: 2020 Piyush Aggarwal <piyushaggarwal002@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "windowsmessagesnotifier.h"

WindowsMessagesNotifier::WindowsMessagesNotifier()
{
    m_preferDarkMode = !(m_settings.value(QStringLiteral("AppsUseLightTheme"), true).value<bool>());
}

WindowsMessagesNotifier::~WindowsMessagesNotifier() {}

bool WindowsMessagesNotifier::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    MSG *msg = static_cast< MSG * >( message );
    switch (msg->message)
    {
        case WM_SETTINGCHANGE: {
            WindowsMessagesNotifier::handleWMSettingChange();
            break;
        }
        default: {}
    }
    return false;
}

void WindowsMessagesNotifier::handleWMSettingChange()
{
    m_settings.sync();
    const bool preferDarkModeNow = !(m_settings.value(QStringLiteral("AppsUseLightTheme"), true).value<bool>());
    if (m_preferDarkMode != preferDarkModeNow) {
        m_preferDarkMode = preferDarkModeNow;
        Q_EMIT WindowsMessagesNotifier::wm_colorSchemeChanged();
    }
}

bool WindowsMessagesNotifier::preferDarkMode()
{
    return m_preferDarkMode;
}
