/*
    SPDX-FileCopyrightText: 2021 Waqar Ahmed <waqar.17a@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "kcommandbarmodel_p.h"
#include "kcommandbar.h" // For ActionGroup

#include <KLocalizedString>

#include <QAction>

KCommandBarModel::KCommandBarModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void KCommandBarModel::refresh(const QVector<KCommandBar::ActionGroup> &actionGroups)
{
    int totalActions = std::accumulate(actionGroups.begin(), actionGroups.end(), 0, [](int a, const KCommandBar::ActionGroup &ag) {
        return a + ag.actions.count();
    });

    QVector<Item> temp_rows;
    temp_rows.reserve(totalActions);
    int actionGroupIdx = 0;
    for (const auto &ag : actionGroups) {
        const auto &agActions = ag.actions;
        const QString name = ag.name;
        std::transform(agActions.begin(), agActions.end(), std::back_inserter(temp_rows), [name](QAction *action) {
            return Item{name, action, -1};
        });
        actionGroupIdx++;
    }

    /**
     * For each action in last triggered actions,
     *  - Find it in the actions
     *  - Use the score variable to set its score
     *
     * Items in m_lastTriggered are stored in descending order
     * by their usage i.e., the first item in the vector is the most
     * recently invoked action.
     *
     * Here we traverse them in reverse order, i.e., from least recent to
     * most recent and then assign a score to them in a way that most recent
     * ends up having the highest score. Thus when proxy model does the sorting
     * later, most recent item will end up on the top
     */
    int score = 0;
    std::for_each(m_lastTriggered.crbegin(), m_lastTriggered.crend(), [&score, &temp_rows](const QString &act) {
        auto it = std::find_if(temp_rows.begin(), temp_rows.end(), [act](const KCommandBarModel::Item &i) {
            return i.action->text() == act;
        });
        if (it != temp_rows.end()) {
            it->score = score++;
        }
    });

    beginResetModel();
    m_rows = std::move(temp_rows);
    endResetModel();
}

QVariant KCommandBarModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto &entry = m_rows[index.row()];
    const int col = index.column();

    switch (role) {
    case Qt::DisplayRole:
        if (col == 0) {
            return QString(entry.groupName + QStringLiteral(": ") + KLocalizedString::removeAcceleratorMarker(entry.action->text()));
        } else {
            return entry.action->shortcut().toString();
        }
    case Qt::DecorationRole:
        if (col == 0) {
            return entry.action->icon();
        }
        break;
    case Qt::TextAlignmentRole:
        if (col == 0) {
            return Qt::AlignLeft;
        } else {
            return Qt::AlignRight;
        }
    case Qt::UserRole: {
        return QVariant::fromValue(entry.action);
    }
    case Role::Score:
        return entry.score;
    }

    return {};
}

void KCommandBarModel::actionTriggered(const QString &name)
{
    if (m_lastTriggered.size() == 6) {
        m_lastTriggered.pop_back();
    }
    m_lastTriggered.push_front(name);
}

QStringList KCommandBarModel::lastUsedActions() const
{
    return m_lastTriggered;
}

void KCommandBarModel::setLastUsedActions(const QStringList &actionNames)
{
    m_lastTriggered = actionNames;

    while (m_lastTriggered.size() > 6) {
        m_lastTriggered.pop_back();
    }
}
