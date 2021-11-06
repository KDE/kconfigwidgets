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
#include <QHeaderView>

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

    bool hasActionsWithIcons() const
    {
        return m_hasActionsWithIcons;
    }

    Q_SLOT void setFilterString(const QString &string)
    {
        // MUST reset the model here, we want to repopulate
        // invalidateFilter() will not work here
        beginResetModel();
        m_pattern = string;
        m_hasActionsWithIcons = false;
        endResetModel();
    }

protected:
    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override
    {
        const int scoreLeft = sourceLeft.data(KCommandBarModel::Score).toInt();
        const int scoreRight = sourceRight.data(KCommandBarModel::Score).toInt();
        if (scoreLeft == scoreRight) {
            const QString textLeft = sourceLeft.data().toString();
            const QString textRight = sourceRight.data().toString();

            return textRight.localeAwareCompare(textLeft) < 0;
        }

        return scoreLeft < scoreRight;
    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override
    {
        const QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

        bool accept = false;
        if (m_pattern.isEmpty()) {
            accept = true;
        } else {
            // Example row= "File: Open File"
            // actionName: OpenFile
            const QString row = index.data(Qt::DisplayRole).toString();
            const int pos = row.indexOf(QLatin1Char(':'));
            if (pos >= 0) {
                const QString actionName = row.mid(pos + 2);
                KFuzzyMatcher::Result res = KFuzzyMatcher::match(m_pattern, actionName);
                sourceModel()->setData(index, res.score, KCommandBarModel::Score);
                accept = res.matched;
            }
        }

        if (accept && !m_hasActionsWithIcons) {
            m_hasActionsWithIcons |= !index.data(Qt::DecorationRole).isNull();
        }

        return accept;
    }

private:
    QString m_pattern;
    mutable bool m_hasActionsWithIcons = false;
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
    static void paintItemText(QPainter *p, const QString &textt, const QRect &rect, const QStyleOptionViewItem &options, QVector<QTextLayout::FormatRange> formats)
    {
        QString text = options.fontMetrics.elidedText(textt, Qt::ElideRight, rect.width());

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

        const int lineWidth = rect.width();
        line.setLineWidth(lineWidth);
        line.setPosition(QPointF(0, 0));

        textLayout.endLayout();

        /**
         * get "Y" so that we can properly V-Center align the text in row
         */
        const int y = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignVCenter, textLayout.boundingRect().size().toSize(), rect).y();

        // draw the text
        const QPointF pos(rect.x(), y);
        textLayout.draw(p, pos);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        painter->save();

        /**
         * Draw everything, (widget, icon etc) except the text
         */
        QStyleOptionViewItem optionCopy = option;
        initStyleOption(&optionCopy, index);
        optionCopy.text.clear(); // clear old text
        QStyle *style = option.widget->style();
        style->drawControl(QStyle::CE_ItemViewItem, &optionCopy, painter, option.widget);

        const int hMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, &option, option.widget);

        QRect outputRect = option.rect;

        const CommandBarFilterModel *model = static_cast<const CommandBarFilterModel*>(index.model());
        if (model->hasActionsWithIcons()) {
            const int iconWidth = option.decorationSize.width() + hMargin;
            if (option.direction == Qt::RightToLeft) {
                outputRect.adjust(0, 0, -iconWidth, 0);
            } else {
                outputRect.adjust(iconWidth, 0, 0, 0);
            }
        }

        const QString original = index.data().toString();
        QStringView str = original;
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
        fmt.setForeground(option.palette.link().color());
        fmt.setFontWeight(QFont::Bold);

        /**
         * Highlight matches from fuzzy matcher
         */
        const auto fmtRanges = KFuzzyMatcher::matchedRanges(m_filterString, str);
        QTextCharFormat f;
        f.setForeground(option.palette.link());
        std::transform(fmtRanges.begin(), fmtRanges.end(), std::back_inserter(formats), [f, actionNameStart](const KFuzzyMatcher::Range &fr) {
            return QTextLayout::FormatRange{fr.start + actionNameStart, fr.length, f};
        });

        outputRect.adjust(hMargin, 0, -hMargin, 0);
        paintItemText(painter, original, outputRect, option, std::move(formats));

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

    // returns the width needed to draw the shortcut
    static int shortcutDrawingWidth(const QStyleOptionViewItem &option, const QString &shortcut, int hMargin)
    {
        int width = 0;
        if (!shortcut.isEmpty()) {
            // adapt the shortcut as it will be drawn
            // "Ctrl+A, Alt+B" => "Ctrl+A+Alt+B"
            QString adaptedShortcut = shortcut;
            adaptedShortcut.replace(QStringLiteral(", "), QStringLiteral("+"));

            width = option.fontMetrics.horizontalAdvance(adaptedShortcut);

            // count the number of segments
            // "Ctrl+A+Alt+B" => ["Ctrl", "+", "A", "+", "Alt", "+", "B"]
            static const QRegularExpression regExp(QStringLiteral("(\\+(?!\\+)|\\+(?=\\+{2}))"));
            const int segmentsCount = 2 * adaptedShortcut.count(regExp) + 1;

            // add left and right margins for each segment
            width += segmentsCount * 2 * hMargin;
        }

        return width;
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        // draw background
        option.widget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);

        const QString shortcutString = index.data().toString();
        if (shortcutString.isEmpty()) {
            return;
        }

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
            * @todo We probably dont need the full rect, just the width so the
            * "btns" vector can just be vector<pair<int, string>>
            */
        QVector<QPair<QRect, QString>> btns;
        btns.reserve(list.size());
        const int height = option.rect.height();
        const int hMargin = horizontalMargin(option);
        for (const QString &text : std::as_const(list)) {
            if (text.isEmpty()) {
                continue;
            }
            QRect r(0, 0, option.fontMetrics.horizontalAdvance(text), option.fontMetrics.lineSpacing());
            // this happens on gnome so we manually decrease the
            // height a bit
            if (r.height() == height) {
                r.setHeight(r.height() - 4);
            }
            r.setWidth(r.width() + 2 * hMargin);
            btns.append({r, text});
        }

        // we have nothing, just return
        if (btns.isEmpty()) {
            return;
        }

        const QRect plusRect = option.fontMetrics.boundingRect(QLatin1Char('+'));

        // draw them
        int x;
        if (option.direction == Qt::RightToLeft) {
            x = option.rect.x() + hMargin;
        } else {
            x = option.rect.right() - shortcutDrawingWidth(option, shortcutString, hMargin) - hMargin;
        }
        const int y = option.rect.y() + (option.rect.height() - btns.at(0).first.height()) / 2;
        const int plusY = option.rect.y() + (option.rect.height() - plusRect.height()) / 2;
        const int total = btns.size();

        int i = 0;
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        for (const auto &btn : std::as_const(btns)) {
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
                x += rect.width() + hMargin;
                painter->drawText(QPoint(x, plusY + (rect.height() / 2)), QStringLiteral("+"));
                x += plusRect.width() + hMargin;
            }
            i++;
        }

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        if (index.isValid() && index.column() == KCommandBarModel::Column_Shortcut) {
            QString shortcut = index.data().toString();
            if (!shortcut.isEmpty()) {
                const int hMargin = horizontalMargin(option);
                const int width = shortcutDrawingWidth(option, shortcut, hMargin) + 2 * hMargin;

                return QSize(width, 0);
            }
        }

        return QStyledItemDelegate::sizeHint(option, index);
    }

    int horizontalMargin(const QStyleOptionViewItem &option) const
    {
        return option.widget->style()->pixelMetric(QStyle::PM_FocusFrameHMargin, &option) + 2;
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
    d->m_treeView.setItemDelegateForColumn(KCommandBarModel::Column_Command, delegate);
    d->m_treeView.setItemDelegateForColumn(KCommandBarModel::Column_Shortcut, del);

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

    d->m_treeView.header()->setMinimumSectionSize(0);
    d->m_treeView.header()->setStretchLastSection(false);
    d->m_treeView.header()->setSectionResizeMode(KCommandBarModel::Column_Command, QHeaderView::Stretch);
    d->m_treeView.header()->setSectionResizeMode(KCommandBarModel::Column_Shortcut, QHeaderView::ResizeToContents);

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
