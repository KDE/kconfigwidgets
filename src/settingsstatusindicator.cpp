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

#include "settingsstatusindicator_p.h"

#include <QEvent>
#include <QIcon>
#include <QPainter>
#include <QRegularExpression>
#include <QVariant>

namespace  {
constexpr const int INDICATOR_ICON_SIZE = 16;
constexpr const int INDICATOR_SIZE = INDICATOR_ICON_SIZE + 8;

constexpr const char *originalPosProperty = "originalPos";
constexpr const char *originalWidthProperty = "originalWidth";

bool isWidgetAtParentEdge(QWidget *widget)
{
    const auto leftToRight = widget->isLeftToRight();
    if (leftToRight) {
        const auto right = widget->pos().x() + widget->width();
        const auto parentWidth = widget->parentWidget()->width();
        return right == parentWidth;
    } else {
        return widget->pos().x() == 0;
    }
}

int widgetForcedWidth(QWidget *widget)
{
    // We actually respect the minimum size hint to avoid the widget looking odd
    // worst case scenario the indicator isn't visible if the widget is ending at the very
    // edge of the window and is already too small to be resized, it's a very unlikely case
    // though, when reaching the edge it's often greedy in space anyway
    return qMax(widget->width() - INDICATOR_SIZE, widget->minimumSizeHint().width());
}

int widgetExpectedWidth(QWidget *widget)
{
    if (isWidgetAtParentEdge(widget)) {
        return widgetForcedWidth(widget);
    } else {
        return widget->width();
    }
}

}

SettingStatusIndicator::SettingStatusIndicator(QWidget *parent)
    : QToolButton(parent)
{
    setAutoRaise(true);
    setIconSize({INDICATOR_ICON_SIZE, INDICATOR_ICON_SIZE});
    setIcon(QIcon::fromTheme("edit-reset"));
    setFocusPolicy(Qt::NoFocus);
    setVisible(true);
}

SettingStatusIndicator::~SettingStatusIndicator()
{
    setTrackedWidget(nullptr); // This will neatly reset the width offset
}

void SettingStatusIndicator::setTrackedWidget(QWidget *widget)
{
    if (m_trackedWidget == widget) {
        return;
    }

    if (m_trackedWidget) {
        resetWidthOffset();
        m_trackedWidget->removeEventFilter(this);
    }

    m_trackedWidget = widget;
    if (!m_trackedWidget) {
        return;
    }

    forceWidthOffset();
    applyLayout();
    setVisible(m_trackedWidget->isVisible());
    m_trackedWidget->installEventFilter(this);
}

bool SettingStatusIndicator::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)
    if (event->type() == QEvent::Resize && !m_forcingSize) {
        if (forceWidthOffset()) {
            return true;
        }
    }

    switch (event->type()) {
    case QEvent::Resize:
    case QEvent::Move:
        applyLayout();
        break;
    case QEvent::Show:
        setVisible(true);
        break;
    case QEvent::Hide:
        setVisible(false);
        break;
    default:
        break;
    }

    return false;
}

bool SettingStatusIndicator::forceWidthOffset()
{
    // Force resizing only when necessary
    if (!isWidgetAtParentEdge(m_trackedWidget)) {
        return false;
    }

    m_forcingSize = true;
    m_trackedWidget->setProperty(originalWidthProperty, m_trackedWidget->width());
    m_trackedWidget->setProperty(originalPosProperty, m_trackedWidget->pos());
    m_trackedWidget->resize(widgetForcedWidth(m_trackedWidget), m_trackedWidget->height());
    if (!m_trackedWidget->isLeftToRight()) {
        m_trackedWidget->move(m_trackedWidget->pos() + QPoint(INDICATOR_SIZE, 0));
    }
    m_forcingSize = false;
    return true;
}

void SettingStatusIndicator::resetWidthOffset()
{
    const auto originalWidth = m_trackedWidget->property(originalWidthProperty);
    const auto originalPos = m_trackedWidget->property(originalPosProperty);
    if (originalWidth.isValid()) {
        Q_ASSERT(originalPos.isValid());
        m_forcingSize = true;

        auto size = m_trackedWidget->size();
        size.setWidth(originalWidth.toInt());
        m_trackedWidget->resize(size);
        m_trackedWidget->move(originalPos.toPoint());

        m_trackedWidget->setProperty(originalWidthProperty, QVariant());
        m_trackedWidget->setProperty(originalPosProperty, QVariant());

        m_forcingSize = false;
    }
}

void SettingStatusIndicator::applyLayout()
{
    resize(INDICATOR_SIZE, INDICATOR_SIZE);

    const auto leftToRight = m_trackedWidget->isLeftToRight();
    auto x = leftToRight ? m_trackedWidget->pos().x() + m_trackedWidget->width()
                         : m_trackedWidget->pos().x() - width();
    const auto y = m_trackedWidget->pos().y() + (m_trackedWidget->height() - height()) / 2.0f;

    // We didn't get resized which means we're not at the border of the window
    // then we try to vertically line up with other indicators
    const auto originalWidth = m_trackedWidget->property(originalWidthProperty);
    if (!originalWidth.isValid() || m_trackedWidget->width() == originalWidth.toInt()) {
        const auto re = QRegularExpression("^kcfg_");
        const auto children = m_trackedWidget->parentWidget()->findChildren<QWidget*>(re, Qt::FindDirectChildrenOnly);
        const auto xValues = [=] {
            auto result = QVector<int>();
            result.reserve(children.size());
            std::transform(children.cbegin(), children.cend(),
                           std::back_inserter(result),
                           [=](QWidget *w) {
                               const auto localX = leftToRight ? widgetExpectedWidth(w) : -width();
                               return w->pos().x() + localX;
                           });
            return result;
        }();
        if (leftToRight) {
            x = *std::max_element(xValues.cbegin(), xValues.cend());
        } else {
            x = *std::min_element(xValues.cbegin(), xValues.cend());
        }
    }

    move(x, y);
}
