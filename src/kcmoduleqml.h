/*
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

#ifndef KCMODULEQML_H
#define KCMODULEQML_H

#include <kcmodule.h>

class QQuickItem;
class KCModuleQmlPrivate;

/**
 * The base class for configuration modules written in QML
 *
 * Configuration modules are realized as plugins that are loaded only when
 * needed.
 *
 * The KCModuleQml subclass should implement only the logic of the config module,
 * and will be accessible as the "kcm" property from the QML code
 *
 * The QML code should be provided as a KPackage installed under
 * share/kpackage/kcms with the same name as the KAboutdata component name
 * of the KCModuleQml implementation.
 *
 * To write a config module, you have to create a library
 * that contains a factory function like the following:
 *
 * \code
 * #include <KPluginFactory>
 *
 * K_PLUGIN_FACTORY(MyKCModuleFactory, registerPlugin<MyKCModule>() )
 * \endcode
 *
 * The constructor of the KCModule then looks like this:
 * \code
 * YourKCModule::YourKCModule( QWidget* parent )
 *   : KCModule( parent )
 * {
 *   KAboutData *about = new KAboutData(
 *     <kcm name>, i18n( "..." ),
 *     KDE_VERSION_STRING, QString(), KAboutLicense::GPL,
 *     i18n( "Copyright 2006 ..." ) );
 *   about->addAuthor( i18n(...) );
 *   setAboutData( about );
 *   .
 *   .
 *   .
 * }
 * \endcode
 *
 * The actual load of the QML will be performed automatically at the first showEvent()
 * of this widget.
 * The KCModuleQml subclass will have to provide properties, signal and slots to
 * provide an interface for the QML part to read and write the configuration.
 * The actual read and write of the configuration will still be performed in the
 * load() and save() method as normal KCModule instances.
 */
class KCONFIGWIDGETS_EXPORT KCModuleQml : public KCModule
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *mainUi READ mainUi CONSTANT)

public:
    KCModuleQml(QWidget* parent, const QVariantList& args);
    ~KCModuleQml();

    QQuickItem *mainUi();

protected:
    virtual void showEvent(QShowEvent *event);

private:
    KCModuleQmlPrivate *const d;
};

#endif
