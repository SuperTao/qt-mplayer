#include <QtGui/QApplication>
#include "MainWindow.h"
#include <QListWidget>
#include <QString>
#include <QApplication>
#include "MyListWidget.h"
#include <QProcess>
#include <QLabel>
#include "MySlider.h"
#include <QTextCodec>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
//    QTextStream::setCodec(QTextCodec::codecForLocale());
//    QTextStream::setCodec(QTextCodec::codecForName("UTF-8"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
