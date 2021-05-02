/*
    This file is part of the KDE project
    SPDX-FileCopyrightText: 2021 Felix Ernst <fe.a.ernst@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "khamburgermenuhelpers_p.h"

#include "khamburgermenu.h"

#include <QEvent>
#include <QWidget>

ListenerContainer::ListenerContainer(KHamburgerMenuPrivate *hamburgerMenuPrivate)
    : QObject{hamburgerMenuPrivate},
    m_listeners{std::vector<std::unique_ptr<QObject>>(4)}
{   }

ListenerContainer::~ListenerContainer()
{   }

bool AddOrRemoveActionListener::eventFilter(QObject * /*watched*/, QEvent *event)
{
    if (event->type() == QEvent::ActionAdded || event->type() == QEvent::ActionRemoved) {
        static_cast<KHamburgerMenuPrivate *>(parent())->notifyMenuResetNeeded();
    }
    return false;
}

bool ButtonPressListener::eventFilter(QObject * /*watched*/, QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress) {
        auto hamburgerMenuPrivate = static_cast<KHamburgerMenuPrivate *>(parent());
        auto q = static_cast<KHamburgerMenu *>(hamburgerMenuPrivate->q_ptr);
        Q_EMIT q->aboutToShowMenu();
        hamburgerMenuPrivate->resetMenu();
    }
    return false;
}

bool VisibleActionsChangeListener::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Show || event->type() == QEvent::Hide) {
        if (!event->spontaneous()) {
            static_cast<KHamburgerMenuPrivate *>(parent())->notifyMenuResetNeeded();
        }
    } else if (event->type() == QEvent::ActionAdded || event->type() == QEvent::ActionRemoved) {
        Q_ASSERT_X(qobject_cast<QWidget *>(watched), "VisibileActionsChangeListener",
            "The watched QObject is expected to be a QWidget.");
        if (static_cast<QWidget *>(watched)->isVisible()) {
            static_cast<KHamburgerMenuPrivate *>(parent())->notifyMenuResetNeeded();
        }
    }
    return false;
}

bool VisibilityChangesListener::eventFilter(QObject * /*watched*/, QEvent *event)
{
    if (event->type() == QEvent::Show || event->type() == QEvent::Hide) {
        if (!event->spontaneous()) {
            static_cast<KHamburgerMenuPrivate *>(parent())->updateVisibility();
        }
    }
    return false;
}

bool isWidgetActuallyVisible(const QWidget *widget)
{
    Q_CHECK_PTR(widget);
    bool actuallyVisible = widget->isVisible();
    const QWidget *ancestorWidget = widget->parentWidget();
    while (actuallyVisible && ancestorWidget) {
        actuallyVisible = ancestorWidget->isVisible();
        ancestorWidget = ancestorWidget->parentWidget();
    }
    return actuallyVisible;
}

bool listContainsWidget(const std::forward_list<QPointer<const QWidget>> &list, const QWidget *widget)
{
    for (const auto &item : list) {
        if (widget == item) {
            return true;
        }
    }
    return false;
}
