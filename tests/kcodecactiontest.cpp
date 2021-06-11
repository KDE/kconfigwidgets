#include "kcodecactiontest.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>

#include <QDebug>

#include <kcodecaction.h>

int main(int argc, char **argv)
{
    QApplication::setApplicationName(QStringLiteral("kcodecactiontest"));
    QApplication app(argc, argv);

    CodecActionTest *test = new CodecActionTest;
    test->show();

    return app.exec();
}

CodecActionTest::CodecActionTest(QWidget *parent)
    : QMainWindow(parent)
    , m_comboCodec(new KCodecAction(QStringLiteral("Combo Codec Action"), this))
    , m_buttonCodec(new KCodecAction(QStringLiteral("Button Codec Action"), this))
{
    // clang-format off
    m_comboCodec->setToolBarMode(KCodecAction::ComboBoxMode);
    connect(m_comboCodec, SIGNAL(triggered(QAction*)), SLOT(actionTriggered(QAction*)));
    connect(m_comboCodec, SIGNAL(indexTriggered(int)), SLOT(indexTriggered(int)));
    connect(m_comboCodec, SIGNAL(textTriggered(QString)), SLOT(textTriggered(QString)));
    connect(m_comboCodec, SIGNAL(codecTriggered(QTextCodec*)), SLOT(codecTriggered(QTextCodec*)));

    m_buttonCodec->setToolBarMode(KCodecAction::MenuMode);
    connect(m_buttonCodec, SIGNAL(triggered(QAction*)), SLOT(actionTriggered(QAction*)));
    connect(m_buttonCodec, SIGNAL(indexTriggered(int)), SLOT(indexTriggered(int)));
    connect(m_buttonCodec, SIGNAL(textTriggered(QString)), SLOT(textTriggered(QString)));
    connect(m_buttonCodec, SIGNAL(codecTriggered(QTextCodec*)), SLOT(codecTriggered(QTextCodec*)));
    // clang-format on

    menuBar()->addAction(m_comboCodec);
    menuBar()->addAction(m_buttonCodec);

    QToolBar *toolBar = addToolBar(QStringLiteral("Test"));
    toolBar->addAction(m_comboCodec);
    toolBar->addAction(m_buttonCodec);
}

void CodecActionTest::actionTriggered(QAction *action)
{
    qDebug() << action;
}

void CodecActionTest::indexTriggered(int index)
{
    qDebug() << index;
}

void CodecActionTest::textTriggered(const QString &text)
{
    qDebug() << '"' << text << '"';
}

void CodecActionTest::codecTriggered(QTextCodec *codec)
{
    qDebug() << codec->name() << ':' << codec->mibEnum();
}

void CodecActionTest::slotActionTriggered(bool state)
{
    qDebug() << sender() << " state " << state;
}
