#include "EmoToText.h"
#include <QFileDialog>
#include <QDirIterator>
#include <QListWidgetItem>
#include <QStringListModel>
#include <QProcess>
#include <QTextCodec>
#include <QFileSystemWatcher>
#include <QDebug>
#include "g2/algorithm/g2.utils.convert.string.h"
using namespace g2::utils::convert::string;
#include <iostream>
#include <regex>


EmoToText::EmoToText(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.graphicsView->setScene(new QGraphicsScene(this));
    QFileSystemWatcher* watcher = new QFileSystemWatcher(this);
    static QSet<QString> image_set;

    // Connect the button to the slot
    connect(ui.pushButton, &QPushButton::clicked, this, [=] {
        // select the directory
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
        "/home",
        QFileDialog::ShowDirsOnly
        | QFileDialog::DontResolveSymlinks);
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
        ui.textEdit->append(path);

        // call ocr.exe C:/Users/Public/dev/Release/ppocr.exe 
        // ppocr.exe --det_model_dir="./ch_PP-OCRv3_det_infer" --rec_model_dir="./ch_PP-OCRv3_rec_infer" --image_dir="C:\Users\XiZhu\Pictures\QQ" --use_angle_cls=false --det=true --rec=true --use_gpu=False
        QProcess process;
        QString root = "C:/Users/Public/dev/Release/";
        QString program = root + "ppocr.exe";
        QStringList arguments;
        arguments << "--det_model_dir=" + root + "ch_PP-OCRv3_det_infer";
        arguments << "--rec_model_dir=" + root + "ch_PP-OCRv3_rec_infer";
        arguments << "--rec_char_dict_path=" + root + "ch_PP-OCRv3_rec_infer/ppocr_keys_v1.txt";
        arguments << "--output=" + root + "output";
        arguments << "--image_dir=" + path;
        arguments << "--use_angle_cls=false";
        arguments << "--det=true";
        arguments << "--rec=true";
        arguments << "--use_gpu=False";
        process.start(program, arguments);
        process.waitForFinished(-1); // will wait forever until finished
        QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
        ui.textEdit->append(output);
        //(\d+).*det boxes:(.*?)rec text: (.*?)rec score: (\d+.?\d*)
        //(\d+).*det boxes: (\[\[(\d+,\d+)\],\[(\d+,\d+)\],\[(\d+,\d+)\],\[(\d+,\d+)\]\]) rec text: (.*?) rec score: (\d+.?\d*)
        // (\\d+).*det boxes: (\\[\\[(\\d+,\\d+)\\],\\[(\\d+,\\d+)\\],\\[(\\d+,\\d+)\\],\\[(\\d+,\\d+)\\]\\]) rec text: (.*?) rec score: (\\d+.?\\d*)
        // Æ¥Åäoutput 
        // 1. id
        // 2. rect * 2
        // 3. text
        // 4. score
        ui.graphicsView->scene()->clear();
        ui.graphicsView->scene()->addPixmap(QPixmap(path));
        ui.graphicsView->fitInView(ui.graphicsView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);

        QRegularExpression re(R"((.*?)\s+det boxes:\s+(\S+)\s+rec text:\s+(\S+)\s+rec score:\s+(\S+))");
        for (auto& str : output.split("\n")) {
            auto match{ re.match(str) };
            if (match.hasMatch()) {
                QString id = match.captured(1);
                QString rect_str = match.captured(2);
                QString text = match.captured(3);
                QString score = match.captured(4);
                QStringList a;
                a << "id" << id << "rect" << rect_str << "text" << text << "score" << score;
                ui.textEdit->append(a.join(" "));
                QRect rect;
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
                    a2 << "x1" << x1 << "y1" << y1 << "x2" << x2 << "y2" << y2 << "x3" << x3 << "y3" << y3 << "x4" << x4 << "y4" << y4;
                    ui.textEdit->append(a2.join(" "));
                    rect = QRect(x1.toInt(), y1.toInt(), x3.toInt() - x1.toInt(), y3.toInt() - y1.toInt());
                }

                ui.graphicsView->scene()->addRect(rect, QPen(Qt::red, 3));


            }
        }

        //std::string str = output.toStdString();
        //std::regex r("(\\d+).*det boxes: (\\[\\[(\\d+,\\d+)\\],\\[(\\d+,\\d+)\\],\\[(\\d+,\\d+)\\],\\[(\\d+,\\d+)\\]\\]) rec text: (.*?) rec score: (\\d+.?\\d*)");
        //std::smatch sm;
        //std::regex_search(str, sm, r);
        //for (int i = 0; i < sm.size(); i++) {
        //    std::cout << sm[i] << std::endl;
        //}


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
            ui.listWidget->addItem(item);
            auto path = image;
            QProcess process;
            QString root = "C:/Users/Public/dev/Release/";
            QString program = root + "ppocr.exe";
            QStringList arguments;
            arguments << "--det_model_dir=" + root + "ch_PP-OCRv3_det_infer";
            arguments << "--rec_model_dir=" + root + "ch_PP-OCRv3_rec_infer";
            arguments << "--rec_char_dict_path=" + root + "ch_PP-OCRv3_rec_infer/ppocr_keys_v1.txt";
            arguments << "--output=" + root + "output";
            arguments << "--image_dir=" + path;
            arguments << "--use_angle_cls=false";
            arguments << "--det=true";
            arguments << "--rec=true";
            arguments << "--use_gpu=False";
            process.start(program, arguments);
            process.waitForFinished(-1); // will wait forever until finished
            QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
            ui.textEdit->append(output);
            //(\d+).*det boxes:(.*?)rec text: (.*?)rec score: (\d+.?\d*)
            //(\d+).*det boxes: (\[\[(\d+,\d+)\],\[(\d+,\d+)\],\[(\d+,\d+)\],\[(\d+,\d+)\]\]) rec text: (.*?) rec score: (\d+.?\d*)
            // (\\d+).*det boxes: (\\[\\[(\\d+,\\d+)\\],\\[(\\d+,\\d+)\\],\\[(\\d+,\\d+)\\],\\[(\\d+,\\d+)\\]\\]) rec text: (.*?) rec score: (\\d+.?\\d*)
            // Æ¥Åäoutput 
            // 1. id
            // 2. rect * 2
            // 3. text
            // 4. score
            ui.graphicsView->scene()->clear();
            ui.graphicsView->scene()->addPixmap(QPixmap(path));
            ui.graphicsView->fitInView(ui.graphicsView->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);

            QRegularExpression re(R"((.*?)\s+det boxes:\s+(\S+)\s+rec text:\s+(\S+)\s+rec score:\s+(\S+))");
            for (auto& str : output.split("\n")) {
                auto match{ re.match(str) };
                if (match.hasMatch()) {
                    QString id = match.captured(1);
                    QString rect_str = match.captured(2);
                    QString text = match.captured(3);
                    QString score = match.captured(4);
                    QStringList a;
                    a << "id" << id << "rect" << rect_str << "text" << text << "score" << score;
                    ui.textEdit->append(a.join(" "));
                    QRect rect;
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
                        a2 << "x1" << x1 << "y1" << y1 << "x2" << x2 << "y2" << y2 << "x3" << x3 << "y3" << y3 << "x4" << x4 << "y4" << y4;
                        ui.textEdit->append(a2.join(" "));
                        rect = QRect(x1.toInt(), y1.toInt(), x3.toInt() - x1.toInt(), y3.toInt() - y1.toInt());
                    }

                    ui.graphicsView->scene()->addRect(rect, QPen(Qt::red, 3));


                }
            }
        }

        });


}

EmoToText::~EmoToText()
{}
