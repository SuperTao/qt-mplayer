#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QListWidget>
#include <QDebug>
#include <QSlider>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString title("mplayer");
    this->setWindowTitle(title);
    this->DesktopWidget = QApplication::desktop();
    QRect ScreenRect = DesktopWidget->screenGeometry();
    ScreenX = ScreenRect.x();
    ScreenY = ScreenRect.y();
    ScreenW = ScreenRect.width();
    ScreenH = ScreenRect.height();
    this->setGeometry(300,230,850,450);
    //歌曲进度slider
    this->SetTimeSlider = new QSlider(this);
    this->SetTimeSlider->setGeometry(250,370,600,30);
    this->SetTimeSlider->setRange(0,100);
    this->SetTimeSlider->setOrientation(Qt::Horizontal);
    this->SetTimeSlider->show();
    //设置视频窗口
    this->VideoWidget = new QWidget(this);
    this->VideoWidget->setGeometry(250,30,440,400);
    this->VideoWidget->setFocus();
    this->WidgetID = this->VideoWidget->winId();
    //设置按键无边框
//    ui->pushButtonBefore->setFlat(true);
//    ui->pushButtonNext->setFlat(true);
//    ui->pushButtonFastForward->setFlat(true);
//    ui->pushButtonFastRewind->setFlat(true);
//    ui->pushButtonPause->setFlat(true);
//    ui->pushButtonOpenFiles->setFlat(true);

    this->listwidget = new MyListWidget(this);
    listwidget->setFocus();
    listwidget->setCurrentRow(0);
    connect(listwidget, SIGNAL(SetRectSignal()), this, SLOT(SetRect()));
    listwidget->wid = this->WidgetID;
    connect(listwidget, SIGNAL(SetTimeSliderValueSignal(int)), this, SLOT(SetTimeSliderValue(int)));
    //显示歌曲名字
//    this->MusicName = new QLabel(this);
//    MusicName->setGeometry(10,10,200,30);
//    connect(listwidget, SIGNAL(SetMusicNameLabel(QString)), MusicName, SLOT(setText(QString)));
    //显示当前歌曲时间
    this->LabelTimePos = new QLabel(this);
    LabelTimePos->setGeometry(250,410,80,30);
    LabelTimePos->setText("00:00:00");
    connect(listwidget, SIGNAL(SetTimePosLabel(QString)), LabelTimePos, SLOT(setText(QString)));
    //显示歌曲总时间
    this->LabelTimeLenth = new QLabel(this);
    LabelTimeLenth->setGeometry(790, 410,80,30);
    LabelTimeLenth->setText("00:00:00");
    connect(listwidget, SIGNAL(SetTimeLenthLabel(QString)), LabelTimeLenth, SLOT(setText(QString)));
    //控制声音, 有问题，没实现
    this->VolumnSlider = new QSlider(this);
    VolumnSlider->setGeometry(80,60,80,30);
    VolumnSlider->setRange(0,100);
    VolumnSlider->setOrientation(Qt::Horizontal);
//    VolumnSlider->show();
    VolumnSlider->hide();
    connect(VolumnSlider, SIGNAL(valueChanged(int)), listwidget, SLOT(ChangeVolumn(int)));
    //发给MyListWidget的信号，控制文件播放状态
    //下一首
    connect(this, SIGNAL(NextSignal(int)), listwidget, SLOT(ChangeMusic(int)));
    //上一首
    connect(this, SIGNAL(BeforeSignal(int)), listwidget, SLOT(ChangeMusic(int)));
    //暂停
    connect(this, SIGNAL(PauseSignal()), listwidget, SLOT(PauseSing()));
    //快进
    connect(this, SIGNAL(ForwardSignal()), listwidget, SLOT(FastForward()));
    //快退
    connect(this, SIGNAL(RewindSignal()), listwidget, SLOT(FastRewind()));
    //全屏
//    connect(this, SIGNAL(FullScreenSignal()), listwidget, SLOT(FullScreen()));
    isFullScreen = false;
    //新建一个Item
    listwidget->Item = new QListWidgetItem;

    int W = this->width();
    int H = this->height();
    ui->pushButtonPause->setGeometry((W-250)/2-35/2+250,H-70, 35, 30);
    ui->pushButtonFastForward->setGeometry((W-250)/2-35/2+250+45,H-70, 40, 30);
    ui->pushButtonNext->setGeometry((W-250)/2-35/2+250+95,H-70, 40, 30);
    ui->pushButtonFastRewind->setGeometry((W-250)/2-35/2+250-50,H-70, 40, 30);
    ui->pushButtonBefore->setGeometry((W-250)/2-35/2+250-100,H-70, 40, 30);
    ui->pushButtonOpenFiles->setGeometry(W-75,H-70,30,30);
    ui->pushButtonRemove->setGeometry(W-35,H-70,30,30);
    //从新设置VideoWidget的位置
    this->listwidget->setGeometry(7,15,240,H-38);
    this->VideoWidget->setGeometry(250,15,W-250,H-100);
    this->SetTimeSlider->setGeometry(250,H-85,W-250,20);
    this->LabelTimeLenth->setGeometry(W-60,H-70,60,20);
    this->LabelTimePos->setGeometry(250,H-70,60,20);
    this->setWindowTitle(this->listwidget->music);

    this->ReadCache();
}

void MainWindow::ReadCache()
{
    //文件存在进行读写
    if(QFile::exists("mplayerCache.txt"))
    {
        QFile file("mplayerCache.txt");
        file.open(QIODevice::ReadWrite);
        QTextStream in(&file);
        QString filename = in.readAll();
        qDebug() << filename;
        //用空格分开
        Files = filename.split(',');
        qDebug() << Files;
        file.close();
        qDebug() << Files.size();
        QStringList tmp;
        int i;
        for(i = 0; i < Files.size(); i++)
        {
            //把链表中每个项目取出来
            QString item(Files.at(i));
            qDebug() << item;
            //把每个项目的目录目录用‘/’分开
            QStringList itemlist = item.split('/');
            //提取每项最后一个，就是前面目录的歌曲名称,组成链表
            tmp.append(itemlist.last());
        }
        //将链表出给listwidget便于调用
        this->listwidget->musicname = Files;
        //更新item数量
        listwidget->ItemCount = Files.size();
        //将链表添加的ui的列表中
        listwidget->insertItems(listwidget->ItemCount,tmp);
    }
}
void MainWindow::WriteCache()
{
    //文件不存在，创建文件
    if(!QFile::exists("mplayerCache.txt"))
    {
       QFile FileCache("mplayerCache.txt");
       FileCache.open(QIODevice::ReadWrite);
       FileCache.close();
    }
    QFile FileCache("mplayerCache.txt");
    FileCache.open(QIODevice::ReadWrite | QIODevice::Truncate);
    QTextStream in(&FileCache);
    int i;
    for(i = 0; i < listwidget->ItemCount; i++)
    {
        QString str(Files.at(i));
        str.append(',');
//        char *ch = str.toLatin1().data();
//        QString str1 = codec->toUnicode(ch);
//        ch = str1.toUtf8().data();
        in << str;
//        in << ch ;
    }
    FileCache.flush();
    FileCache.close();
}
MainWindow::~MainWindow()
{
    delete SetTimeSlider;
    delete VideoWidget;
    delete listwidget;
    delete MusicName;
    delete LabelTimePos;
    delete LabelTimeLenth;
    delete VolumnSlider;
    this->listwidget->mplayerProcess->close();
    delete listwidget->Item;
    delete ui;
}

void MainWindow::SetRect()
{
    if(isFullScreen == false)
    {
//        int X = this->x();
//        int Y = this->y();
        int W = this->width();
        int H = this->height();
//        this->setGeometry(X,Y,W,H);
        ui->pushButtonPause->setGeometry((W-250)/2-35/2+250,H-70, 35, 30);
        ui->pushButtonFastForward->setGeometry((W-250)/2-35/2+250+45,H-70, 40, 30);
        ui->pushButtonNext->setGeometry((W-250)/2-35/2+250+95,H-70, 40, 30);
        ui->pushButtonFastRewind->setGeometry((W-250)/2-35/2+250-50,H-70, 40, 30);
        ui->pushButtonBefore->setGeometry((W-250)/2-35/2+250-100,H-70, 40, 30);
        ui->pushButtonOpenFiles->setGeometry(W-75,H-70,30,30);
        ui->pushButtonRemove->setGeometry(W-35,H-70,30,30);
        //从新设置VideoWidget的位置
        this->VideoWidget->setGeometry(250,15,W-250,H-100);
        this->listwidget->setGeometry(7,15,240,H-38);
        this->SetTimeSlider->setGeometry(250,H-85,W-250,20);
        this->LabelTimeLenth->setGeometry(W-60,H-70,60,20);
        this->LabelTimePos->setGeometry(250,H-70,60,20);
        this->setWindowTitle(this->listwidget->music);
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *)
{
    //没全屏,设成全屏
    if(isFullScreen == false)
    {
        isFullScreen = true;
        //隐藏
        this->listwidget->hide();
        this->SetTimeSlider->hide();
        this->LabelTimeLenth->hide();
        this->LabelTimePos->hide();
        ui->pushButtonPause->hide();
        ui->pushButtonFastForward->hide();
        ui->pushButtonNext->hide();
        ui->pushButtonFastRewind->hide();
        ui->pushButtonBefore->hide();
        ui->pushButtonOpenFiles->hide();
        ui->pushButtonRemove->hide();
//        this->VolumnSlider->hide();
        this->X = this->x();
        this->Y = this->y();
        this->W = this->width();
        this->H = this->height();
        qDebug() << X;
        qDebug() << Y;
        qDebug() << W;
        qDebug() << H;
        this->showFullScreen();
        this->VideoWidget->setGeometry(ScreenX,ScreenY,ScreenW,ScreenH);;
    }
    else if(isFullScreen == true)
    {
        isFullScreen = false;
        this->listwidget->show();
        this->SetTimeSlider->show();
        this->LabelTimeLenth->show();
        this->LabelTimePos->show();
        ui->pushButtonPause->show();
        ui->pushButtonFastForward->show();
        ui->pushButtonNext->show();
        ui->pushButtonFastRewind->show();
        ui->pushButtonBefore->show();
        ui->pushButtonOpenFiles->show();
        ui->pushButtonRemove->show();
        this->showNormal();
        qDebug() << "else";
        qDebug() << this->x();
        qDebug() << this->y();
        qDebug() << this->width();
        qDebug() << this->height();
    }
}

void MainWindow::on_pushButtonOpenFiles_clicked()
{
    //打开文件
    QStringList files = QFileDialog::getOpenFileNames(
                             this,
                             "Select one or more files to open",
                             "/",
                             "Files (*.mp3 *.mp4 *.rmvb *.3gp *.avi *.mpg)");
    int i;
    QStringList tmp;
    for(i = 0; i < files.size(); i++)
    {
        //把链表中每个项目取出来
        QString item(files.at(i));
        //把每个项目的目录目录用‘/’分开
        QStringList itemlist = item.split('/');
        //提取每项最后一个，就是出去前面目录的歌曲名称,组成链表
        tmp.append(itemlist.last());
        //保存包含全部路径的链表
        this->Files.append(item);
    }
    qDebug() << Files;
    //将链表出给listwidget便于调用
    this->listwidget->musicname = Files;
    //把选择的文件添加到列表中
    listwidget->insertItems(listwidget->ItemCount,tmp);
    //获取文件个数，下次再添加，从这后面添加
    listwidget->ItemCount += files.size();

    this->WriteCache();
}

void MainWindow::on_pushButtonRemove_clicked()
{
    //保存当前行
    int current_row = this->listwidget->currentRow();
    //当前文件正在运行，移除时关闭
    if(listwidget->CurrentIndex == listwidget->currentRow())
    {
        listwidget->mplayerProcess->write("quit\n");
    }
    //移除文件列表里面的内容
    Files.removeAt(this->listwidget->currentRow());
    qDebug() << Files;
    //清空列表
    listwidget->clear();
    //从新插入
    QStringList tmp;
    int i;
    for(i = 0; i < Files.size(); i++)
    {
        //把链表中每个项目取出来
        QString item(Files.at(i));
        //把每个项目的目录目录用‘/’分开
        QStringList itemlist = item.split('/');
        //提取每项最后一个，就是前面目录的歌曲名称,组成链表
        tmp.append(itemlist.last());
    }
    //将链表出给listwidget便于调用
    this->listwidget->musicname = Files;
    //更新item数量
    listwidget->ItemCount = Files.size();
    //将链表添加的ui的列表中
    listwidget->insertItems(listwidget->ItemCount,tmp);
    //更新item个数
    //从新设置当前行
    this->listwidget->setCurrentRow(current_row);
    this->WriteCache();
}
void MainWindow::on_pushButtonPause_clicked()
{
    emit this->PauseSignal();
    qDebug() << "emit pause signal";
}

void MainWindow::on_pushButtonFastForward_clicked()
{
    emit this->ForwardSignal();
    qDebug() << "emit fast forward signal";
}

void MainWindow::on_pushButtonFastRewind_clicked()
{
    emit this->RewindSignal();
    qDebug() << "emit fast rewind signal";
}

void MainWindow::SetTimeSliderValue(int value)
{
    this->SetTimeSlider->setValue(value);
}
//下一首
void MainWindow::on_pushButtonNext_clicked()
{
    int value = 1;
    emit this->NextSignal(value);
    qDebug() << "next song";
}
//上一首
void MainWindow::on_pushButtonBefore_clicked()
{
    int value = -1;
    emit this->BeforeSignal(value);
    qDebug() << "previous song";
}

