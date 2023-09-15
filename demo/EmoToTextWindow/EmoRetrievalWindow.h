#pragma once

#include <QWidget>
#include "ui_EmoRetrievalWindow.h"

class EmoRetrievalWindow : public QWidget
{
    Q_OBJECT

public:
    EmoRetrievalWindow(QWidget *parent = nullptr);
    ~EmoRetrievalWindow();

private:
    Ui::EmoRetrievalWindowClass ui;
};
