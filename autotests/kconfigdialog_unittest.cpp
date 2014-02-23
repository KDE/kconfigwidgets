/* This file is part of the KDE libraries

    Copyright (c) 2012 Albert Astals Cid <aacid@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <QTest>
#include <qtestevent.h>

#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QSignalSpy>

#include <kconfigdialog.h>
#include <kconfigskeleton.h>
#include <kcolorcombo.h>

#include "signaltest.h"

static const auto CONFIG_FILE = QStringLiteral("kconfigdialog_unittestrc");

class ComboBoxPage : public QWidget
{
public:
    ComboBoxPage()
    {
        colorCombo = new KColorCombo(this);
        colorCombo->setObjectName("kcfg_Color");
        colorCombo->setColor(Qt::red);

        enumCombo = new QComboBox(this);
        enumCombo->setObjectName("kcfg_Enum");
        enumCombo->addItems(QStringList() << "A" << "B" << "C");

        textCombo = new QComboBox(this);
        textCombo->setObjectName("kcfg_Text");
        textCombo->setEditable(true);
        textCombo->addItems(QStringList() << "A" << "B" << "C");

        numInput = new QSpinBox(this);
        numInput->setValue(1);
        numInput->setObjectName("kcfg_IntNumInput");
    }

    KColorCombo *colorCombo;
    QComboBox *enumCombo;
    QComboBox *textCombo;
    QSpinBox *numInput;
};

class ComboSettings : public KConfigSkeleton
{
public:
    ComboSettings() : KConfigSkeleton(CONFIG_FILE)
    {
        colorItem = new ItemColor(currentGroup(), QLatin1String("Color"), color, Qt::white);
        addItem(colorItem, QLatin1String("Color"));

        QList<ItemEnum::Choice2> textValues;
        {
            ItemEnum::Choice2 choice;
            choice.name = QLatin1String("A");
            textValues.append(choice);
        }
        {
            ItemEnum::Choice2 choice;
            choice.name = QLatin1String("B");
            textValues.append(choice);
        }
        {
            ItemEnum::Choice2 choice;
            choice.name = QLatin1String("C");
            textValues.append(choice);
        }
        enumItem = new ItemEnum(currentGroup(), QLatin1String("Enum"), enumIndex, textValues, 1);
        addItem(enumItem, QLatin1String("Enum"));

        stringItem = new ItemString(currentGroup(), QLatin1String("Text"), string, QLatin1String("hh:mm"));
        addItem(stringItem, QLatin1String("Text"));

        intValueItem = new ItemInt(currentGroup(), QLatin1String("IntNumInput"), intValue, 42);
        addItem(intValueItem, QLatin1String("IntNumInput"));
    }

    ItemColor *colorItem;
    QColor color;

    ItemEnum *enumItem;
    int enumIndex;

    ItemString *stringItem;
    QString string;

    ItemInt *intValueItem;
    int intValue;
};

class KConfigDialog_UnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::enableTestMode(true);
        // Leftover configuration breaks combosTest
        const QString configFile = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, CONFIG_FILE);
        if (!configFile.isEmpty()) {
            if (!QFile::remove(configFile)) {
                qWarning() << "Could not remove old config file:" << configFile;
            }
        }
    }

    void test()
    {
        ComboSettings *skeleton = new ComboSettings();
        KConfigDialog *dialog = new KConfigDialog(0, "settings", skeleton);
        ComboBoxPage *page = new ComboBoxPage();

        QCOMPARE(page->colorCombo->color().name(), QColor(Qt::red).name());
        QCOMPARE(page->enumCombo->currentIndex(), 0);
        QCOMPARE(page->textCombo->currentText(), QString("A"));
        QCOMPARE(page->numInput->value(), 1);

        dialog->addPage(page, "General");

        QCOMPARE(page->colorCombo->color().name(), QColor(Qt::white).name());
        QCOMPARE(page->enumCombo->currentIndex(), 1);
        QCOMPARE(page->textCombo->currentText(), QLatin1String("hh:mm"));
        QCOMPARE(page->numInput->value(), 42);

        page->colorCombo->setColor(Qt::blue);
        page->enumCombo->setCurrentIndex(2);
        page->textCombo->setCurrentIndex(2);
        page->numInput->setValue(2);

        QDialogButtonBox *buttonBox = dialog->findChild<QDialogButtonBox *>();
        QVERIFY(buttonBox != 0);
        buttonBox->button(QDialogButtonBox::Apply)->click();
        QCOMPARE(skeleton->colorItem->property().value<QColor>().name(), QColor(Qt::blue).name());
        QCOMPARE(skeleton->enumItem->property().toInt(), 2);
        QCOMPARE(skeleton->stringItem->property().toString(), QLatin1String("C"));
        QCOMPARE(skeleton->intValueItem->property().toInt(), 2);

        delete dialog;
        delete skeleton;
    }

    void testKConfigCompilerSignals()
    {
        const QString defaultValue = QStringLiteral("default value");
        const QString changedValue = QStringLiteral("changed value");
        const QString someOtherValue = QStringLiteral("some other value");
        KConfigDialog *dialog = new KConfigDialog(0, "settings2", SignalTest::self());
        QWidget* page = new QWidget;
        QLineEdit *edit = new QLineEdit(page);
        edit->setObjectName(QStringLiteral("kcfg_foo"));
        edit->setText(QStringLiteral("some text"));

        QSignalSpy spy(SignalTest::self(), SIGNAL(fooChanged(QString)));
        QVERIFY(spy.isValid());
        // now all the magic happens
        dialog->addPage(page, "General");

        //check that default value gets loaded
        QCOMPARE(spy.size(), 0);
        QCOMPARE(edit->text(), defaultValue);
        QCOMPARE(SignalTest::foo(), defaultValue);

        edit->setText(changedValue);
        // change signal should not be emitted immediately (only on save)
        QCOMPARE(spy.size(), 0);
        QCOMPARE(SignalTest::foo(), defaultValue); // should be no change to skeleton


        QDialogButtonBox *buttonBox = dialog->findChild<QDialogButtonBox *>();
        QVERIFY(buttonBox != 0);
        buttonBox->button(QDialogButtonBox::Apply)->click(); // now signal should be emitted

        QCOMPARE(spy.size(), 1);
        QVariantList args = spy.last();
        QCOMPARE(args.size(), 1);
        QCOMPARE((QMetaType::Type)args[0].type(), QMetaType::QString);
        QCOMPARE(args[0].toString(), changedValue);
        QCOMPARE(SignalTest::foo(), changedValue);

        // change it to a different value
        edit->setText(someOtherValue);
        QCOMPARE(spy.size(), 1);
        buttonBox->button(QDialogButtonBox::Apply)->click(); // now signal should be emitted

        QCOMPARE(spy.size(), 2);
        args = spy.last();
        QCOMPARE(args.size(), 1);
        QCOMPARE((QMetaType::Type)args[0].type(), QMetaType::QString);
        QCOMPARE(args[0].toString(), someOtherValue);
        QCOMPARE(SignalTest::foo(), someOtherValue);
    }

};

QTEST_MAIN(KConfigDialog_UnitTest)

#include "kconfigdialog_unittest.moc"
