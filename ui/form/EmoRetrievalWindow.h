#pragma once

#include <QWidget>
#include "ui_EmoRetrievalWindow.h"

class EmoRetrievalWindow : public QWidget
{
    Q_OBJECT

public:
    EmoRetrievalWindow(QWidget *parent = nullptr);
    ~EmoRetrievalWindow();
private slots:
    void update_emo_list(QStringList emos);

private:
    Ui::EmoRetrievalWindowClass ui;
};
