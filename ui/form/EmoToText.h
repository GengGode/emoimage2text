#pragma once

#include <QtWidgets/QWidget>
#include "ui_EmoToText.h"
#include "EmoRetrievalWindow.h"
#include "QSharedPointer"

class EmoToText : public QWidget
{
    Q_OBJECT

public:
    EmoToText(QWidget *parent = nullptr);
    ~EmoToText();

    QSharedPointer<EmoRetrievalWindow> retrieval_window;

private:
    Ui::EmoToTextClass ui;
};
