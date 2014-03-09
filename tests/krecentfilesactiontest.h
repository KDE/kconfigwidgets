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

#ifndef KCONFIGWIDGETS_TESTS_KRECENTFILESACTIONTEST_H
#define KCONFIGWIDGETS_TESTS_KRECENTFILESACTIONTEST_H

#include <QMainWindow>

class KRecentFilesActionTestPrivate;

class KRecentFilesActionTest : public QMainWindow
{
    Q_OBJECT

public:
    KRecentFilesActionTest();

    virtual ~KRecentFilesActionTest();

public Q_SLOTS:
    void addUrl();
    void loadEntries();
    void saveEntries();
    void urlSelected(const QUrl& url);

private:
    KRecentFilesActionTestPrivate *const d;
};

#endif
