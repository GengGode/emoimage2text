#include "EmoRetrievalWindow.h"
#include <QProcess>
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

/**************************/
QStringList find_emo_api(QString text)
{
    auto res = cpr::Get(cpr::Url{"https://localhost:7027/emo/find_paths"},
                        cpr::Header{{"accept", "*/*"}, {"Content-Type", "application/json"}},
                        cpr::Parameters{{"text", text.toStdString()}});
    std::cout << res.text << std::endl;
    if (res.status_code == 200)
        return QString::fromStdString(res.text).split("\n");
    return {};
}

/**************************/

EmoRetrievalWindow::EmoRetrievalWindow(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.lineEdit, &QLineEdit::textChanged, this, [=](QString text)
            { auto emos = find_emo_api(text);
            update_emo_list(emos); });

    ui.listWidget->setIconSize(QSize(64, 64));
    ui.listWidget->setViewMode(QListView::IconMode);
    ui.listWidget->setResizeMode(QListView::Adjust);
}

EmoRetrievalWindow::~EmoRetrievalWindow()
{
}
void EmoRetrievalWindow::update_emo_list(QStringList emos)
{
    ui.listWidget->clear();
    for (auto image : emos)
    {
        auto icon = QIcon(image);
        if (icon.isNull())
            icon = QIcon("./ui/noImage.png");

        QListWidgetItem *item = new QListWidgetItem;
        item->setIcon(icon);
        item->setSizeHint(QSize(64, 64));
        item->setText(image);
        item->setWhatsThis(image);
        ui.listWidget->addItem(item);
    }
}
