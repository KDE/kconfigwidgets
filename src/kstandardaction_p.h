/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999, 2000 Kurt Granroth <granroth@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KSTANDARDACTION_PRIVATE_H
#define KSTANDARDACTION_PRIVATE_H

#include <QAction>
#include <QApplication>

#include <KLazyLocalizedString>
#include <KLocalizedString>
#include <KStandardShortcut>

namespace KStandardAction
{
class AutomaticAction : public QAction
{
    Q_OBJECT

public:
    AutomaticAction(const QIcon &icon, const QString &text, KStandardShortcut::StandardShortcut standardShortcut, const char *slot, QObject *parent);
public Q_SLOTS:
    inline void cut()
    {
        invokeEditSlot("cut");
    }
    inline void copy()
    {
        invokeEditSlot("copy");
    }
    inline void paste()
    {
        invokeEditSlot("paste");
    }
    inline void clear()
    {
        invokeEditSlot("clear");
    }
    inline void selectAll()
    {
        invokeEditSlot("selectAll");
    }

    void invokeEditSlot(const char *slot)
    {
        if (qApp->focusWidget()) {
            QMetaObject::invokeMethod(qApp->focusWidget(), slot);
        }
    }
};

}

#endif
