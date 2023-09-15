#include "EmoToText.h"
#include <QFileDialog>
#include <QDirIterator>
#include <QListWidgetItem>
#include <QStringListModel>
#include <QProcess>
#include <QTextCodec>
#include <QFileSystemWatcher>
#include <QDebug>
#include <QBuffer>
#include "g2/algorithm/g2.utils.convert.string.h"
using namespace g2::utils::convert::string;
#include <iostream>
#include <regex>

QString ocr(QString image)
{
    QProcess process;
    QString root = "C:/Users/Public/dev/Release/";
    QString program = root + "ppocr.exe";
    QStringList arguments;
    arguments << "--det_model_dir=" + root + "ch_PP-OCRv3_det_infer";
    arguments << "--rec_model_dir=" + root + "ch_PP-OCRv3_rec_infer";
    arguments << "--rec_char_dict_path=" + root + "ch_PP-OCRv3_rec_infer/ppocr_keys_v1.txt";
    arguments << "--output=" + root + "output";
    arguments << "--image_dir=" + image;
    arguments << "--use_angle_cls=false";
    arguments << "--det=true";
    arguments << "--rec=true";
    arguments << "--use_gpu=False";
    process.start(program, arguments);
    process.waitForFinished(-1); // will wait forever until finished
    return QString::fromLocal8Bit(process.readAllStandardOutput());
}

QRect match_rect(QString rect_str)
{
    QRegularExpression re2(R"(\[\[(\d+),(\d+)\],\[(\d+),(\d+)\],\[(\d+),(\d+)\],\[(\d+),(\d+)\]\])");
    auto match2{ re2.match(rect_str) };
    if (match2.hasMatch()) {
        QString x1 = match2.captured(1);
        QString y1 = match2.captured(2);
        QString x2 = match2.captured(3);
        QString y2 = match2.captured(4);
        QString x3 = match2.captured(5);
        QString y3 = match2.captured(6);
        QString x4 = match2.captured(7);
        QString y4 = match2.captured(8);
        QStringList a2;
        return QRect(x1.toInt(), y1.toInt(), x3.toInt() - x1.toInt(), y3.toInt() - y1.toInt());
    }
    return {};
}

struct OcrResult
{
    int id;
    QRect rect;
    QString text;
    double score;
    QString raw;
};
struct Results
{
    QVector<OcrResult> ocr_results;
    QString image;
};
QVector<OcrResult> match_result(QString output)
{
    QVector<OcrResult> result;
    QRegularExpression re(R"((.*?)\s+det boxes:\s+(\S+)\s+rec text:\s+(\S+)\s+rec score:\s+(\S+))");
    for (auto& str : output.split("\n")) {
        auto match{ re.match(str) };
        if (match.hasMatch()) {
            QString id = match.captured(1);
            QString rect_str = match.captured(2);
            QString text = match.captured(3);
            QString score = match.captured(4);
            QRect rect = match_rect(rect_str);
            result.push_back({ id.toInt(), rect, text, score.toDouble(), str});
        }
    }
    return result;
}

Results ocr_result(QString image)
{
    return { match_result(ocr(image)), image };
}

QString bind_curl_string(QString name, QString path, QString base64, int width, int height, QString text, QString text_raw)
{
    // curl - X 'POST' \
    //     'https://localhost:7027/emo' \
    //     - H 'accept: */*' \
    //     - H 'Content-Type: application/json' \
    //     - d '{
    //     "imageName" : "string",
    //     "imagePath" : "string",
    //     "imageWidth" : 0,
    //     "imageHeight" : 0,
    //     "emoText" : "string",
    //     "emoTextRaw" : "string"
    //     }'
    QString curl = "curl -X POST https://localhost:7027/emo -H \"accept: */*\" -H \"Content-Type: application/json\" -d \"{";
    curl += "\\\"imageName\\\" : \\\"" + name + "\\\",";
    curl += "\\\"imagePath\\\" : \\\"" + path + "\\\",";
    curl += "\\\"imageWidth\\\" : " + QString::number(width) + ",";
    curl += "\\\"imageHeight\\\" : " + QString::number(height) + ",";
    curl += "\\\"emoText\\\" : \\\"" + text + "\\\",";
    curl += "\\\"emoTextRaw\\\" : \\\"" + text_raw + "\\\"";
    curl += "}\"";
    return curl;
}
QString curl_format(QString curl)
{
    QString result = "";
    for (auto& c : curl) {
        if (c == '\n') {
            result += "\\n";
        }
        else if (c == '\r') {
            result += "\\r";
        }
        else if (c == '\t') {
            result += "\\t";
        }
        else {
            result += c;
        }
    }
    return result;
}

void to_server(Results results)
{
    std::string emo_text = "";
    std::string emo_text_raw = "";
    QString text = "";
    QString text_raw = "";

    for (auto& r : results.ocr_results) {
        emo_text += r.text.toStdString();
        emo_text_raw += r.raw.toStdString() + ";";
        text += r.text;
        text_raw += r.raw + ";";
    }
    QImage image(results.image);
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    QByteArray ba = buffer.data().toBase64();

    QString curl = bind_curl_string(results.image, results.image, "", image.width(), image.height(), text, text_raw);

    curl = curl_format(curl);
    std::string curl_str = curl.toStdString();
    curl_str = utf8_to_gbk(curl_str);
    // std::cout << curl_str << std::endl;

    int res = system(curl_str.c_str());
    std::cout << res << std::endl;
    //std::cout << curl.toStdString() << std::endl;
    //QProcess process;
    //process.start("cmd.exe", QStringList() << "/c" << curl);
    //process.waitForFinished(-1); // will wait forever until finished
    //std::cout << QString::fromLocal8Bit(process.readAllStandardOutput()).toStdString() << std::endl;

}

EmoToText::EmoToText(QWidget* parent)
    : QWidget(parent), retrieval_window(new EmoRetrievalWindow(nullptr))
{
    ui.setupUi(this);
    ui.graphicsView->setScene(new QGraphicsScene(this));
    QFileSystemWatcher* watcher = new QFileSystemWatcher(this);
    static QSet<QString> image_set;

    // Connect the button to the slot
    connect(ui.pushButton, &QPushButton::clicked, this, [=] {
        // select the directory
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty()) {
            // set the directory path to the line edit
            ui.lineEdit->setText(dir);
        }
        });
    connect(ui.lineEdit, &QLineEdit::textChanged, this, [=] {
        // set the directory path to the line edit
        watcher->addPath(ui.lineEdit->text());
        QStringList images;
        QDirIterator it(ui.lineEdit->text(), QStringList() << "*.png" << "*.jpg", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            auto image = it.next();
            images.push_back(image);
            image_set.insert(image);
        }
        ui.listWidget->clear();
        ui.listWidget->setIconSize(QSize(256, 256));
        ui.listWidget->setViewMode(QListView::IconMode);
        ui.listWidget->setResizeMode(QListView::Adjust);
        for (auto image : images) {
            QListWidgetItem* item = new QListWidgetItem;
            item->setIcon(QIcon(image));
            item->setSizeHint(QSize(256, 256));
            item->setText("");
            item->setWhatsThis(image);
            ui.listWidget->addItem(item);
        }
        });


    connect(ui.listWidget, &QListWidget::itemDoubleClicked, this, [=] {
        // set the directory path to the line edit
        QListWidgetItem* item = ui.listWidget->currentItem();
        //ui.label->setPixmap(item->icon().pixmap(256, 256));
        QString path = item->whatsThis();
        ui.graphicsView->scene()->clear();
        ui.graphicsView->scene()->addPixmap(QPixmap(path));
        ui.graphicsView->fitInView(ui.graphicsView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);

        ui.textEdit->append(path);

        auto result = ocr_result(path);

        for (auto& r : result.ocr_results) {
            ui.textEdit->append(r.text);
            ui.graphicsView->scene()->addRect(r.rect, QPen(Qt::red, 3));
        }
        to_server(result);
        });

    connect(watcher, &QFileSystemWatcher::directoryChanged, this, [=] {
        // set the directory path to the line edit
        QStringList images;
        QDirIterator it(ui.lineEdit->text(), QStringList() << "*.png" << "*.jpg", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            auto image = it.next();
            if (image_set.find(image) == image_set.end()) {
                images.push_back(image);
                image_set.insert(image);
            }
        }
        for (auto image : images) {
            QListWidgetItem* item = new QListWidgetItem;
            item->setIcon(QIcon(image));
            item->setSizeHint(QSize(256, 256));
            item->setText("");
            item->setWhatsThis(image);
            ui.graphicsView->scene()->clear();
            ui.graphicsView->scene()->addPixmap(QPixmap(image));
            ui.graphicsView->fitInView(ui.graphicsView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);

            ui.listWidget->addItem(item);

            auto result = ocr_result(image);

            for (auto& r : result.ocr_results) {
                ui.textEdit->append(r.text);
                ui.graphicsView->scene()->addRect(r.rect, QPen(Qt::red, 3));
            }

            to_server(result);
        }

        });

    retrieval_window->show();
}

EmoToText::~EmoToText()
{}
