#ifndef KCODECACTION_TEST_H
#define KCODECACTION_TEST_H

#include <QMainWindow>

class KCodecAction;

class CodecActionTest : public QMainWindow
{
    Q_OBJECT

public:
    CodecActionTest(QWidget *parent = nullptr);

public Q_SLOTS:
    void actionTriggered(QAction *action);
    void indexTriggered(int index);
    void textTriggered(const QString &text);
    void codecTriggered(QTextCodec *codec);

    void slotActionTriggered(bool state);

private:
    KCodecAction *m_comboCodec;
    KCodecAction *m_buttonCodec;
};

#endif
