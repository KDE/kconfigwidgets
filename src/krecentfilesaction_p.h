/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
    SPDX-FileCopyrightText: 1999 Simon Hausmann <hausmann@kde.org>
    SPDX-FileCopyrightText: 2000 Nicolas Hadacek <haadcek@kde.org>
    SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>
    SPDX-FileCopyrightText: 2000 Michael Koch <koch@kde.org>
    SPDX-FileCopyrightText: 2001 Holger Freyther <freyther@kde.org>
    SPDX-FileCopyrightText: 2002 Ellis Whitehead <ellis@kde.org>
    SPDX-FileCopyrightText: 2002 Joseph Wenninger <jowenn@kde.org>
    SPDX-FileCopyrightText: 2003 Andras Mantia <amantia@kde.org>
    SPDX-FileCopyrightText: 2005-2006 Hamish Rodda <rodda@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KRECENTFILESACTION_P_H
#define KRECENTFILESACTION_P_H

#include "krecentfilesaction.h"

class KRecentFilesActionPrivate
{
    Q_DECLARE_PUBLIC(KRecentFilesAction)

public:
    explicit KRecentFilesActionPrivate(KRecentFilesAction *parent)
        : q_ptr(parent)
    {
    }

    virtual ~KRecentFilesActionPrivate()
    {
    }

    void init();

    void urlSelected(QAction *);

    int m_maxItems = 10;
    QMap<QAction *, QString> m_shortNames;
    QMap<QAction *, QUrl> m_urls;
    QAction *m_noEntriesAction = nullptr;
    QAction *clearSeparator = nullptr;
    QAction *clearAction = nullptr;

    KRecentFilesAction *const q_ptr;
};

#endif // KRECENTFILESACTION_P_H
