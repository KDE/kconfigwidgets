/* This file is part of the KDE Project
   Copyright (c) 2014 Marco Martin <mart@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kcmoduleqml.h"

#include <QDebug>

#include <QVBoxLayout>
#include <QtQml>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWidget>
#include <QQuickItem>

#include <kdeclarative/kdeclarative.h>
#include <kdeclarative/qmlobject.h>
#include <KAboutData>
#include <KLocalizedString>
#include <Plasma/Package>
#include <Plasma/PluginLoader>

class KCModuleQmlPrivate
{
public:
    KCModuleQmlPrivate()
        : quickWidget(0),
          qmlObject(0)
    {
    }

    QQuickWidget *quickWidget;
    KDeclarative::QmlObject *qmlObject;
};

KCModuleQml::KCModuleQml(QWidget* parent, const QVariantList& args)
    : KCModule(parent, args),
      d(new KCModuleQmlPrivate)
{
}

KCModuleQml::~KCModuleQml()
{
}

void KCModuleQml::showEvent(QShowEvent *event)
{
    if (d->quickWidget) {
        return;
    } else if (d->qmlObject) {
        return;
    }

    QVBoxLayout* layout = new QVBoxLayout(this);

    d->quickWidget = new QQuickWidget(this);
    d->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    Plasma::Package package = Plasma::PluginLoader::self()->loadPackage("Plasma/Generic");
    package.setDefaultPackageRoot("plasma/kcms");
    package.setPath(aboutData()->componentName());
    KDeclarative::KDeclarative dec;
    dec.setDeclarativeEngine(d->quickWidget->engine());
    dec.setTranslationDomain(aboutData()->componentName());
    dec.setupBindings();
    d->quickWidget->rootContext()->setContextProperty("kcm", this);
    d->quickWidget->setSource(QUrl::fromLocalFile(package.filePath("mainscript")));
    setMinimumHeight(d->quickWidget->initialSize().height());

    layout->addWidget(d->quickWidget);
}

QQuickItem *KCModuleQml::mainUi()
{
    if (d->qmlObject) {
        return qobject_cast<QQuickItem *>(d->qmlObject->rootObject());
    } else if (d->quickWidget) {
        return d->quickWidget->rootObject();
    }

    d->qmlObject = new KDeclarative::QmlObject(this);
    d->qmlObject->setTranslationDomain(aboutData()->componentName());
    d->qmlObject->setInitializationDelayed(true);
    
    Plasma::Package package = Plasma::PluginLoader::self()->loadPackage("Plasma/Generic");
    package.setDefaultPackageRoot("plasma/kcms");
    package.setPath(aboutData()->componentName());

    d->qmlObject->setSource(QUrl::fromLocalFile(package.filePath("mainscript")));
    d->qmlObject->engine()->rootContext()->setContextProperty("kcm", this);
    d->qmlObject->completeInitialization();

    return qobject_cast<QQuickItem *>(d->qmlObject->rootObject());
}

#include "moc_kcmoduleqml.cpp"
