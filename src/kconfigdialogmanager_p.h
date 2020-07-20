/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2003 Benjamin C Meyer (ben+kdelibs at meyerhome dot net)
 *  Copyright (C) 2003 Waldo Bastian <bastian@kde.org>
 *  Copyright (C) 2017 Friedrich W. H. Kossebau <kossebau@kde.org>
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

#ifndef KCONFIGDIALOGMANAGER_P_H
#define KCONFIGDIALOGMANAGER_P_H

#include <QHash>
#include <QSet>
#include <QString>

class QWidget;
class KConfigDialogManager;
class KCoreConfigSkeleton;

class KConfigDialogManagerPrivate
{
public:
    KConfigDialogManagerPrivate(KConfigDialogManager *q);

    void setDefaultsIndicatorsVisible(bool enabled);

    void onWidgetModified();
    void updateWidgetIndicator(const QString &configId, QWidget *widget);
    void updateAllWidgetIndicators();

public:
    KConfigDialogManager * const q;

    /**
    * KConfigSkeleton object used to store settings
     */
    KCoreConfigSkeleton *m_conf = nullptr;

    /**
    * Dialog being managed
     */
    QWidget *m_dialog = nullptr;

    QHash<QString, QWidget *> knownWidget;
    QHash<QString, QWidget *> buddyWidget;
    QSet<QWidget *> allExclusiveGroupBoxes;
    bool insideGroupBox : 1;
    bool trackChanges : 1;
    bool defaultsIndicatorsVisible : 1;
};

#endif // KCONFIGDIALOGMANAGER_P_H
