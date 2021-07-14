/*
    SPDX-FileCopyrightText: 2021 Waqar Ahmed <waqar.17a@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "kcommandbar.h"
#include "kcommandbarmodel_p.h"

#include <QAction>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QTextLayout>
#include <QTreeView>
#include <QVBoxLayout>

#include <KConfigGroup>
#include <KFuzzyMatcher>
#include <KLocalizedString>
#include <KSharedConfig>

// BEGIN CommandBarFilterModel
class CommandBarFilterModel final : public QSortFilterProxyModel
{
public:
    CommandBarFilterModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
    }

    Q_SLOT void setFilterString(const QString &string)
    {
        // MUST reset the model here, we want to repopulate
        // invalidateFilter() will not work here
        beginResetModel();
        m_pattern = string;
        endResetModel();
    }

protected:
    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override
    {
        const int l = sourceLeft.data(KCommandBarModel::Score).toInt();
        const int r = sourceRight.data(KCommandBarModel::Score).toInt();
        return l < r;
    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override
    {
        if (m_pattern.isEmpty())
            return true;

        const QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
        // Example row= "File: Open File"
        // actionName: OpenFile
        const QString row = idx.data(Qt::DisplayRole).toString();
        int pos = row.indexOf(QLatin1Char(':'));
        if (pos < 0) {
            return false;
        }

        const QString actionName = row.mid(pos + 2);
        KFuzzyMatcher::Result res = KFuzzyMatcher::match(m_pattern, actionName);
        sourceModel()->setData(idx, res.score, KCommandBarModel::Score);
        return res.matched;
    }

private:
    QString m_pattern;
};
// END CommandBarFilterModel

class CommandBarStyleDelegate final : public QStyledItemDelegate
{
public:
    CommandBarStyleDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    /**
     * Paints a single item's text
     */
    static void paintItemText(QPainter *p, const QString &textt, const QStyleOptionViewItem &options, QVector<QTextLayout::FormatRange> formats)
    {
        constexpr int iconWidth = 20;
        QString text = options.fontMetrics.elidedText(textt, Qt::ElideRight, options.rect.width() - iconWidth);

        // set formats and font
        QTextLayout textLayout(text, options.font);
        const QVector<QTextLayout::FormatRange> fmts = textLayout.formats();
        formats.append(fmts);
        textLayout.setFormats(formats);

        // set alignment, rtls etc
        QTextOption textOption;
        textOption.setTextDirection(options.direction);
        textOption.setAlignment(QStyle::visualAlignment(options.direction, options.displayAlignment));
        textLayout.setTextOption(textOption);

        // layout the text
        textLayout.beginLayout();

        QTextLine line = textLayout.createLine();
        if (!line.isValid()) {
            return;
        }

        const int lineWidth = options.rect.width();
        line.setLineWidth(lineWidth);
        line.setPosition(QPointF(0, 0));

        textLayout.endLayout();

        /**
         * get "Y" so that we can properly V-Center align the text in row
         */
        const int y = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignVCenter, textLayout.boundingRect().size().toSize(), options.rect).y();

        // draw the text
        const QPointF pos(options.rect.x(), y);
        textLayout.draw(p, pos);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem options = option;
        initStyleOption(&options, index);

        painter->save();

        // paint background
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
        } else {
            painter->fillRect(option.rect, option.palette.base());
        }

        /**
         * Draw everything, (widget, icon etc) except the text
         */
        options.text = QString(); // clear old text
        QStyle *style = options.widget->style();
        style->drawControl(QStyle::CE_ItemViewItem, &options, painter, options.widget);

        const int iconWidth = options.decorationSize.width() + (2 * style->pixelMetric(QStyle::PM_FocusFrameHMargin, &options, options.widget));

        /**
         * We don't want to draw over the icon
         * so move by 20px
         */
        const QString original = index.data().toString();
        const bool rtl = original.isRightToLeft();
        if (rtl) {
            painter->translate(-iconWidth, 0);
        } else {
            painter->translate(iconWidth, 0);
        }

        /**
         * must use QString here otherwise fuzzy match display
         * won't work very well.
         */
        QString str = original;
        int componentIdx = original.indexOf(QLatin1Char(':'));
        int actionNameStart = 0;
        if (componentIdx > 0) {
            actionNameStart = componentIdx + 2;
            // + 2 because there is a space after colon
            str = str.mid(actionNameStart);
        }

        QVector<QTextLayout::FormatRange> formats;
        if (componentIdx > 0) {
            QTextCharFormat gray;
            gray.setForeground(option.palette.color(QPalette::PlaceholderText));
            formats.append({0, componentIdx, gray});
        }

        QTextCharFormat fmt;
        fmt.setForeground(options.palette.link().color());
        fmt.setFontWeight(QFont::Bold);

        /**
         * TODO: Will be enabled when KFuzzyMatcher supports returning
         *  match positions
         */
        //        const auto f = get_fuzzy_match_formats(m_filterString, str, componentIdx + 2, fmt);
        //        formats.append(f);

        paintItemText(painter, original, options, std::move(formats));

        painter->restore();
    }

public Q_SLOTS:
    void setFilterString(const QString &text)
    {
        m_filterString = text;
    }

private:
    QString m_filterString;
};

class ShortcutStyleDelegate final : public QStyledItemDelegate
{
public:
    ShortcutStyleDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    static QStringList splitShortcutString(const QString &shortcutString)
    {
        return shortcutString.split(QLatin1String(", "), Qt::SkipEmptyParts);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem options = option;
        initStyleOption(&options, index);
        painter->save();

        const QString shortcutString = index.data().toString();

        // paint background
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
        } else {
            painter->fillRect(option.rect, option.palette.base());
        }

        options.text = QString(); // clear old text
        options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter, options.widget);

        if (!shortcutString.isEmpty()) {
            /**
             * Shortcut string splitting
             *
             * We do it in two steps
             * 1. Split on ", " so that if we have multi modifier shortcuts they are nicely
             *    split into strings.
             * 2. Split each shortcut from step 1 into individual string.
             *
             * Example:
             *
             * "Ctrl+,, Alt+:"
             * Step 1: [ "Ctrl+," , "Alt+:"]
             * Step 2: [ "Ctrl", ",", "Alt", ":"]
             */
            const QStringList spaceSplitted = splitShortcutString(shortcutString);
            QStringList list;
            list.reserve(spaceSplitted.size() * 2);
            for (const QString &shortcut : spaceSplitted) {
                list += shortcut.split(QLatin1Char('+'), Qt::SkipEmptyParts);
                if (shortcut.endsWith(QLatin1Char('+'))) {
                    list.append(QStringLiteral("+"));
                }
            }

            /**
             * Create rects for each string from the previous step
             *
             * @todo boundingRect may give issues here, use horizontalAdvance
             * @todo We probably dont need the full rect, just the width so the
             * "btns" vector can just be vector<pair<int, string>>
             */
            QVector<QPair<QRect, QString>> btns;
            btns.reserve(list.size());
            const int height = options.rect.height();
            for (const QString &text : qAsConst(list)) {
                if (text.isEmpty()) {
                    continue;
                }
                QRect r = option.fontMetrics.boundingRect(text);
                // this happens on gnome so we manually decrease the
                // height a bit
                if (r.height() == height) {
                    r.setHeight(r.height() - 4);
                }
                r.setWidth(r.width() + 8);
                btns.append({r, text});
            }

            // we have nothing, just return
            if (btns.isEmpty()) {
                return;
            }

            const QRect plusRect = option.fontMetrics.boundingRect(QLatin1Char('+'));

            // draw them
            int x = option.rect.x();
            const int y = option.rect.y();
            const int plusY = option.rect.y() + plusRect.height() / 2;
            const int total = btns.size();

            // make sure our rects are nicely V-center aligned in the row
            painter->translate(QPoint(0, (option.rect.height() - btns.at(0).first.height()) / 2));

            int i = 0;
            painter->setRenderHint(QPainter::Antialiasing);
            for (const auto &btn : qAsConst(btns)) {
                painter->setPen(Qt::NoPen);
                const QRect &rect = btn.first;

                QRect buttonRect(x, y, rect.width(), rect.height());

                // draw rounded rect shadow
                auto shadowRect = buttonRect.translated(0, 1);
                painter->setBrush(option.palette.shadow());
                painter->drawRoundedRect(shadowRect, 3.0, 3.0);

                // draw rounded rect itself
                painter->setBrush(option.palette.button());
                painter->drawRoundedRect(buttonRect, 3.0, 3.0);

                // draw text inside rounded rect
                painter->setPen(option.palette.buttonText().color());
                painter->drawText(buttonRect, Qt::AlignCenter, btn.second);

                // draw '+'
                if (i + 1 < total) {
                    x += rect.width() + 5;
                    painter->drawText(QPoint(x, plusY + (rect.height() / 2)), QStringLiteral("+"));
                    x += plusRect.width() + 5;
                }
                i++;
            }
        }

        painter->restore();
    }
};

// BEGIN KCommandBarPrivate
class KCommandBarPrivate
{
public:
    QTreeView m_treeView;
    QLineEdit m_lineEdit;
    KCommandBarModel m_model;
    CommandBarFilterModel m_proxyModel;

    /**
     * selects first item in treeview
     */
    void reselectFirst()
    {
        const QModelIndex index = m_proxyModel.index(0, 0);
        m_treeView.setCurrentIndex(index);
    }

    /**
     * blocks signals before clearing line edit to ensure
     * we don't trigger filtering / sorting
     */
    void clearLineEdit()
    {
        const QSignalBlocker blocker(m_lineEdit);
        m_lineEdit.clear();
    }

    /**
     * Updates view size and position relative
     * to the parent widget.
     */
    void updateViewGeometry(KCommandBar *q);

    void slotReturnPressed(KCommandBar *q);

    void setLastUsedActions();

    QStringList lastUsedActions() const;
};

void KCommandBarPrivate::updateViewGeometry(KCommandBar *q)
{
    if (!q->parentWidget()) {
        return;
    }
    auto parentWidget = q->parentWidget();

    const QSize centralSize = parentWidget->size();

    const QSize viewMaxSize(centralSize.width() / 2.4, centralSize.height() / 2);

    // First column occupies 60% of the width
    m_treeView.setColumnWidth(0, viewMaxSize.width() * 0.6);

    // Position should be central over window
    const int xPos = std::max(0, (centralSize.width() - viewMaxSize.width()) / 2);
    const int yPos = std::max(0, (centralSize.height() - viewMaxSize.height()) * 1 / 4);

    const QPoint p(xPos, yPos);
    q->move(p + parentWidget->pos());

    q->setFixedSize(viewMaxSize);
}

void KCommandBarPrivate::slotReturnPressed(KCommandBar *q)
{
    auto act = m_proxyModel.data(m_treeView.currentIndex(), Qt::UserRole).value<QAction *>();
    if (act) {
        // if the action is a menu, we take all its actions
        // and reload our dialog with these instead.
        if (auto menu = act->menu()) {
            auto menuActions = menu->actions();
            KCommandBar::ActionGroup ag;

            // if there are no actions, trigger load actions
            // this happens with some menus that are loaded on demand
            if (menuActions.size() == 0) {
                Q_EMIT menu->aboutToShow();
                ag.actions = menu->actions();
            }

            QString groupName = KLocalizedString::removeAcceleratorMarker(act->text());
            ag.name = groupName;

            m_model.refresh({ag});
            reselectFirst();
            /**
             *  We want the "textChanged" signal here
             *  so that proxy model triggers filtering again
             *  so don't use d->clearLineEdit()
             */
            m_lineEdit.clear();
            return;
        } else {
            m_model.actionTriggered(act->text());
            act->trigger();
        }
    }
    clearLineEdit();
    q->hide();
}

void KCommandBarPrivate::setLastUsedActions()
{
    auto cfg = KSharedConfig::openConfig();
    KConfigGroup cg(cfg, "General");

    QStringList actionNames = cg.readEntry(QStringLiteral("CommandBarLastUsedActions"), QStringList());

    return m_model.setLastUsedActions(actionNames);
}

QStringList KCommandBarPrivate::lastUsedActions() const
{
    return m_model.lastUsedActions();
}
// END KCommandBarPrivate

// BEGIN KCommandBar
KCommandBar::KCommandBar(QWidget *parent)
    : QMenu(parent)
    , d(new KCommandBarPrivate)
{
    /**
     * There must be a parent, no nullptrs!
     */
    Q_ASSERT(parent);

    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(4, 4, 4, 4);
    setLayout(layout);

    setFocusProxy(&d->m_lineEdit);

    layout->addWidget(&d->m_lineEdit);

    layout->addWidget(&d->m_treeView);
    d->m_treeView.setTextElideMode(Qt::ElideLeft);
    d->m_treeView.setUniformRowHeights(true);

    CommandBarStyleDelegate *delegate = new CommandBarStyleDelegate(this);
    ShortcutStyleDelegate *del = new ShortcutStyleDelegate(this);
    d->m_treeView.setItemDelegateForColumn(0, delegate);
    d->m_treeView.setItemDelegateForColumn(1, del);

    d->m_proxyModel.setFilterRole(Qt::DisplayRole);
    d->m_proxyModel.setSortRole(KCommandBarModel::Score);

    connect(&d->m_lineEdit, &QLineEdit::returnPressed, this, [this]() {
        d->slotReturnPressed(this);
    });
    connect(&d->m_lineEdit, &QLineEdit::textChanged, &d->m_proxyModel, &CommandBarFilterModel::setFilterString);
    connect(&d->m_lineEdit, &QLineEdit::textChanged, delegate, &CommandBarStyleDelegate::setFilterString);
    connect(&d->m_lineEdit, &QLineEdit::textChanged, this, [this]() {
        d->m_treeView.viewport()->update();
        d->reselectFirst();
    });
    connect(&d->m_treeView, &QTreeView::clicked, this, [this]() {
        d->slotReturnPressed(this);
    });

    d->m_proxyModel.setSourceModel(&d->m_model);
    d->m_treeView.setSortingEnabled(true);
    d->m_treeView.setModel(&d->m_proxyModel);

    d->m_treeView.installEventFilter(this);
    d->m_lineEdit.installEventFilter(this);

    d->m_treeView.setHeaderHidden(true);
    d->m_treeView.setRootIsDecorated(false);
    d->m_treeView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->m_treeView.setSelectionMode(QTreeView::SingleSelection);

    setHidden(true);
}

/**
 * Destructor defined here to make unique_ptr work
 */
KCommandBar::~KCommandBar()
{
    auto lastUsedActions = d->lastUsedActions();
    auto cfg = KSharedConfig::openConfig();
    KConfigGroup cg(cfg, "General");
    cg.writeEntry("CommandBarLastUsedActions", lastUsedActions);
}

void KCommandBar::setActions(const QVector<ActionGroup> &actions)
{
    // First set last used actions in the model
    d->setLastUsedActions();

    d->m_model.refresh(actions);
    d->reselectFirst();

    d->updateViewGeometry(this);
    show();
    setFocus();
}

bool KCommandBar::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::ShortcutOverride) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (obj == &d->m_lineEdit) {
            const int key = keyEvent->key();
            const bool forward2list = (key == Qt::Key_Up) || (key == Qt::Key_Down) || (key == Qt::Key_PageUp) || (key == Qt::Key_PageDown);
            if (forward2list) {
                QCoreApplication::sendEvent(&d->m_treeView, event);
                return true;
            }

            if (key == Qt::Key_Escape) {
                d->clearLineEdit();
            }
        } else {
            const int key = keyEvent->key();
            const bool forward2input = (key != Qt::Key_Up) && (key != Qt::Key_Down) && (key != Qt::Key_PageUp) && (key != Qt::Key_PageDown)
                && (key != Qt::Key_Tab) && (key != Qt::Key_Backtab);
            if (forward2input) {
                QCoreApplication::sendEvent(&d->m_lineEdit, event);
                return true;
            }
        }
    }

    // hide on focus out, if neither input field nor list have focus!
    else if (event->type() == QEvent::FocusOut && !(d->m_lineEdit.hasFocus() || d->m_treeView.hasFocus())) {
        d->clearLineEdit();
        hide();
        return true;
    }

    return QWidget::eventFilter(obj, event);
}
// END KCommandBar
