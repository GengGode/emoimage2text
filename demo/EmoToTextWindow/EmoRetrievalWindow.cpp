#include "EmoRetrievalWindow.h"
#include <QProcess>

/**************************/
const char* find_url = "https://localhost:7027/emo/find_paths";
QStringList find_emo_api(QString text)
{
    QStringList result;
    //QProcess process;
    //QStringList args;
    //args << "-X" << "POST" << find_url
    //    << "-H" << "\"accept: */*\"" << "-H" << "\"Content-Type: application/json\"" 
    //    << "-d" << "[\\\"" + text + "\\\"]";
    //auto cmd = args.join(" ");
    //process.start("curl", args);
    //process.waitForFinished(-1);
    //QString output(process.readAllStandardOutput());
    //QString error(process.readAllStandardError());
    //if (error != "") {
    //    result << error;
    //}
    //else {
    //    result << output;
    //}



    QString curl = "curl -X POST https://localhost:7027/emo/find_paths -H \"accept: */*\" -H \"Content-Type: application/json\" -d \"[\\\"" + text + "\\\"]\"";

    QProcess process;
    process.start(curl);
    process.waitForFinished(-1);
    QString output(process.readAllStandardOutput());
    QString error(process.readAllStandardError());
        result << output;
    return result;


}

/**************************/

EmoRetrievalWindow::EmoRetrievalWindow(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.lineEdit, &QLineEdit::textChanged, this, [=](QString text) {
        auto emos = find_emo_api(text);
        ui.listWidget->addItems(emos);
        });
}

EmoRetrievalWindow::~EmoRetrievalWindow()
{}
