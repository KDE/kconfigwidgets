/*
    This file is part of the KDE project
    SPDX-FileCopyrightText: 2013 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <kcolorschememanager.h>
#include <KActionMenu>

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QListView>
#include <QToolButton>
#include <QVBoxLayout>

class KColorSchemeDemo : public QDialog
{
    Q_OBJECT
public:
    KColorSchemeDemo() : QDialog(nullptr)
    {
        KColorSchemeManager *manager = new KColorSchemeManager(this);

        QListView *view = new QListView(this);
        view->setModel(manager->model());
        connect(view, &QListView::activated, manager, &KColorSchemeManager::activateScheme);

        QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Close, this);
        connect(box, &QDialogButtonBox::rejected, qApp, &QApplication::quit);

        QToolButton *button = new QToolButton(box);
        button->setIcon(QIcon::fromTheme(QStringLiteral("fill-color")));
        button->setMenu(manager->createSchemeSelectionMenu(QStringLiteral("Oxygen"), button)->menu());
        box->addButton(button, QDialogButtonBox::InvalidRole);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(view);
        layout->addWidget(box);
    }
    ~KColorSchemeDemo() {}
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    KColorSchemeDemo *d = new KColorSchemeDemo;
    d->show();
    return app.exec();
}

#include "kcolorschemedemo.moc"
