/*
 *    SPDX-FileCopyrightText: 2021 Waqar Ahmed <waqar.17a@gmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include <KCommandBar>
#include <QMainWindow>

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QToolBar>
#include <QVBoxLayout>

class Window;

/**
 * Fwd decl
 * A helper function to generate a QAction
 */
static QAction *genAction(Window *p, const QString &icon, int i, const int shortcut = Qt::CTRL);

class Window : public QMainWindow
{
public:
    Window(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , pe(this)
    {
        setCentralWidget(&pe);

        auto toolBar = new QToolBar(this);
        this->addToolBar(toolBar);

        auto qo1 = toolBar->addAction(QStringLiteral("Open Command Bar"));
        connect(qo1, &QAction::triggered, this, [this] {
            KCommandBar bar(this);
            bar.setActions(getActions());
            bar.exec();
        });

        auto qo2 = toolBar->addAction(QStringLiteral("Open Command Bar (RTL)"));
        connect(qo2, &QAction::triggered, this, [this] {
            KCommandBar bar(this);
            bar.setActions(getRTLActions());
            bar.exec();
        });
    }

    QMenu *getMenu()
    {
        QMenu *file = new QMenu(this);
        file->setTitle(QStringLiteral("File"));

        auto createActionAndConnect = [file, this](const char *name) {
            auto a = file->addAction(QString::fromUtf8(name));
            connect(a, &QAction::triggered, [a, this] {
                pe.appendPlainText(a->text() + QStringLiteral(" triggered"));
            });
        };

        createActionAndConnect("File Menu action 1");
        createActionAndConnect("File Menu act 2");
        return file;
    }

    QAction *getAboutToShowMenu()
    {
        auto toolMenuAction = new QAction(this);
        toolMenuAction->setText(QStringLiteral("Tool"));
        QMenu *menu = new QMenu(this);
        toolMenuAction->setMenu(menu);

        connect(menu, &QMenu::aboutToShow, this, [this, menu] {
            if (!menu->actions().isEmpty()) {
                return;
            }

            auto createActionAndConnect = [menu, this](const char *name) {
                auto a = menu->addAction(QString::fromUtf8(name));
                connect(a, &QAction::triggered, [a, this] {
                    pe.appendPlainText(a->text() + QStringLiteral(" triggered"));
                });
            };

            createActionAndConnect("About to show action 1");
            createActionAndConnect("About to show 2");
        });
        return toolMenuAction;
    }

    QVector<KCommandBar::ActionGroup> getActions()
    {
        QVector<KCommandBar::ActionGroup> acts(5);
        int i = 0;

        /**
         * Add simple menu
         */
        auto menu = getMenu();
        acts[0].name = menu->title();
        acts[0].actions = menu->actions();

        /**
         * Add a menu that loads on aboutToShow
         */
        auto absMenu = getAboutToShowMenu();
        acts[1].name = absMenu->text();
        acts[1].actions = {absMenu};

        acts[2].name = QStringLiteral("First Menu Group");
        for (; i < 5; ++i) {
            acts[0].actions.append(genAction(this, QStringLiteral("folder"), i));
        }

        acts[3].name = QStringLiteral("Second Menu Group");
        for (; i < 4; ++i) {
            acts[1].actions.append(genAction(this, QStringLiteral("zoom-out"), i));
        }

        acts[4].name = QStringLiteral("Third Menu Group");
        for (; i < 3; ++i) {
            acts[2].actions.append(genAction(this, QStringLiteral("security-low"), i, Qt::CTRL | Qt::ALT));
        }

        return acts;
    }

    // Use ./bin/kcommandbartest -reverse to test this
    QVector<KCommandBar::ActionGroup> getRTLActions()
    {
        QVector<KCommandBar::ActionGroup> acts(2);

        acts[0].name = QStringLiteral("مینو گروپ");
        acts[0].actions = {new QAction(QIcon::fromTheme("folder"), QStringLiteral("یہ فولڈر ایکشن ہے"), this),
                           new QAction(QIcon::fromTheme("folder"), QStringLiteral("یہ ایک اور فولڈر ایکشن ہے"), this)};

        acts[1].name = QStringLiteral("گروپ");
        acts[1].actions = {new QAction(QIcon::fromTheme("zoom-out"), QStringLiteral("یہ فولڈر ایکشن ہے"), this),
                           new QAction(QIcon::fromTheme("security-low"), QStringLiteral("یہ ایک اور فولڈر ایکشن ہے"), this)};

        return acts;
    }

    QPlainTextEdit *textEdit()
    {
        return &pe;
    }

private:
    QPlainTextEdit pe;
};

static QAction *genAction(Window *p, const QString &icon, int i, const int shortcut)
{
    QString text = QStringLiteral("A long long Action name %1").arg(i++);
    QAction *action = new QAction(QIcon::fromTheme(icon), text, p);
    QObject::connect(action, &QAction::triggered, [action, p] {
        p->textEdit()->appendPlainText(action->text() + QStringLiteral(" triggered"));
    });

    static int key = Qt::Key_1;
    key++;
    // Reset
    if (key == Qt::Key_BraceRight) {
        key = Qt::Key_1;
    }
    const auto ss = shortcut | key;
    action->setShortcut(ss);
    return action;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("kcommandbartest"));

    Window *window = new Window();
    window->resize(1024, 600);
    window->show();

    return app.exec();
}
