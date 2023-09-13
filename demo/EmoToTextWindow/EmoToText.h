#pragma once

#include <QtWidgets/QWidget>
#include "ui_EmoToText.h"

class EmoToText : public QWidget
{
    Q_OBJECT

public:
    EmoToText(QWidget *parent = nullptr);
    ~EmoToText();

private:
    Ui::EmoToTextClass ui;
};
