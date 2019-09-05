/* This file is part of the KDE libraries
    Copyright (c) 2014 Gregor Mi <codeminister@publicstatic.de>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License or ( at
    your option ) version 3 or, at the discretion of KDE e.V. ( which shall
    act as a proxy as in section 14 of the GPLv3 ), any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "krecentfilesactiontest.h"

#include <QApplication>
#include <QDebug>

#include <KSharedConfig>
#include <KConfigGroup>
#include "krecentfilesaction.h"
#include "kstandardaction.h"

#include "ui_krecentfilesactiontest.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    KRecentFilesActionTest mainWindow;
    mainWindow.show();
    return app.exec();
}

class KRecentFilesActionTestPrivate
{
public:
    Ui::MainWindow *uiMainWindow;
    KRecentFilesAction *recentFiles;

public:
    void notifyOutputAvailable()
    {
        uiMainWindow->labelOutputAvailable->setText(uiMainWindow->labelOutputAvailable->text() + QLatin1Char('A'));
        qDebug() << recentFiles->items();
    }

    KConfigGroup testConfigGroup()
    {
        return KConfigGroup(KSharedConfig::openConfig(), "RecentFilesActionTest");
    }

};

KRecentFilesActionTest::KRecentFilesActionTest() : d(new KRecentFilesActionTestPrivate)
{
    d->uiMainWindow = new Ui::MainWindow();
    d->uiMainWindow->setupUi(this);

    d->recentFiles = KStandardAction::openRecent(this, SLOT(urlSelected(QUrl)), this);

    connect(d->uiMainWindow->pbAddUrl, SIGNAL(clicked()), this, SLOT(addUrl()));
    connect(d->uiMainWindow->pbLoadEntries, SIGNAL(clicked()), this, SLOT(loadEntries()));
    connect(d->uiMainWindow->pbSaveEntries, SIGNAL(clicked()), this, SLOT(saveEntries()));

    d->uiMainWindow->menuFile->addAction(d->recentFiles);

    //loadEntries();
}

KRecentFilesActionTest::~KRecentFilesActionTest()
{
    //saveEntries();

    delete d->uiMainWindow;
    delete d;
}

void KRecentFilesActionTest::urlSelected(const QUrl& url)
{
    qDebug() << "urlSelected" << url;
    d->notifyOutputAvailable();
}

void KRecentFilesActionTest::addUrl()
{
    QString url = d->uiMainWindow->lineEditUrl->text();
    qDebug() << "addUrl" << url;

    d->recentFiles->addUrl(QUrl(url));

    d->notifyOutputAvailable();
    d->uiMainWindow->lineEditUrl->setText(url + QLatin1Char('a'));
}

void KRecentFilesActionTest::loadEntries()
{
    d->notifyOutputAvailable();
    qDebug() << "recentFiles->loadEntries()";
    d->recentFiles->loadEntries(d->testConfigGroup());
    d->notifyOutputAvailable();
}

void KRecentFilesActionTest::saveEntries()
{
    qDebug() << "recentFiles->saveEntries()";
    d->recentFiles->saveEntries(d->testConfigGroup());
}
