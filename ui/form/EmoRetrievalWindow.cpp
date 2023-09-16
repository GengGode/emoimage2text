#include "EmoRetrievalWindow.h"
#include <QProcess>
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

/**************************/
const char *find_url = "https://localhost:7027/emo/find_paths";
QStringList find_emo_api(QString text)
{
    nlohmann::json body = {text.toStdString()};
    auto test = body.dump();
    std::cout << test << std::endl;

    auto res = cpr::Get(cpr::Url{find_url},
        cpr::Header{{"accept", "*/*"}, { "Content-Type", "application/json" }},
        cpr::Body{body.dump()});

     if (res.status_code == 200)
         return QString::fromStdString(res.text).split("\n");
     return {QString::fromStdString(res.text), QString::number(res.status_code) + " err"};
}

/**************************/

EmoRetrievalWindow::EmoRetrievalWindow(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.lineEdit, &QLineEdit::textChanged, this, [=](QString text)
            {
        auto emos = find_emo_api(text);
        ui.listWidget->addItems(emos); });
}

EmoRetrievalWindow::~EmoRetrievalWindow()
{
}
