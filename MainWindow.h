#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QSlider>
#include <QRect>
#include <QWidget>
#include "MyListWidget.h"
#include <QLabel>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QStringList>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //显示歌曲进度的slider
    QSlider *SetTimeSlider;
    QWidget *VideoWidget;
    int WidgetID;
    MyListWidget *listwidget;
    QLabel *MusicName;
    QLabel *LabelTimePos;
    QLabel *LabelTimeLenth;
    QSlider *VolumnSlider;
    QString RunPath;
    int X;
    int Y;
    int W;
    int H;
    //电脑屏幕的大小
    int ScreenX;
    int ScreenY;
    int ScreenW;
    int ScreenH;
    void mouseDoubleClickEvent(QMouseEvent *);
    bool isFullScreen;
    QStringList Files;
    QDesktopWidget *DesktopWidget;
//    QFile FileCache;
    void ReadCache();
    void WriteCache();
    Ui::MainWindow *ui;
signals:
    //暂停
    void PauseSignal(void);
    //快进
    void ForwardSignal(void);
    //快退
    void RewindSignal(void);
    //下一首
    void NextSignal(int value);
    //前一首
    void BeforeSignal(int value);
    void FullScreenSignal();
private slots:
    /*通过按键发送上面的信号给相应的控件，
     *可以看main中的connect
     */
    //前一首
    void on_pushButtonRemove_clicked();
    void on_pushButtonOpenFiles_clicked();
    void on_pushButtonBefore_clicked();
    //下一首
    void on_pushButtonNext_clicked();
    //快退
    void on_pushButtonFastRewind_clicked();
    //快进
    void on_pushButtonFastForward_clicked();
    //暂停
    void on_pushButtonPause_clicked();
    /*接收其他的类发来的信号
     *设置进度条当前的位置，随音乐变化
     */
    void SetTimeSliderValue(int value);
    void SetRect();
};

#endif // MAINWINDOW_H
