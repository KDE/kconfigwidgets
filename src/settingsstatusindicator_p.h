/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2020 Kevin Ottens <kevin.ottens@enioka.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef SETTINGSSTATUSINDICATOR_P_H
#define SETTINGSSTATUSINDICATOR_P_H

#include <QToolButton>
#include <QPointer>

class SettingStatusIndicator : public QToolButton
{
    Q_OBJECT
public:
    explicit SettingStatusIndicator(QWidget *parent);
    ~SettingStatusIndicator() override;

    void setTrackedWidget(QWidget *widget);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    bool forceWidthOffset();
    void resetWidthOffset();
    void applyLayout();

    QPointer<QWidget> m_trackedWidget;
    bool m_forcingSize = false;
};

#endif // SETTINGSSTATUSINDICATOR_P_H
