#include "MyListWidget.h"
#include <QtDebug>
#include <string.h>
#include <QObject>
#include <QProcess>
#include <stdio.h>

MyListWidget::MyListWidget(QWidget *parent) :
    QListWidget(parent)
{
    //初值
    this->setGeometry(7,120,240,300);
    //进程开启标志
    this->ProcessEnable = 0;
    this->mplayerProcess = new QProcess();
    //鼠标双击
    connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(MusicPlay()));
    //ENTER
    connect(this, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(MusicPlay()));
    connect(this, SIGNAL(BeginSignal()), this, SLOT(SingSong()));
    //对正在播放的进行滚动，让其显示
    connect(this, SIGNAL(ScrollList(const QListWidgetItem*)), this, SLOT(scrollToItem(const QListWidgetItem*)));

    this->timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(GetTime()));
    timer->start(10);
    this->timer_flag = true;
    this->ItemCount = 0;
}

MyListWidget::~MyListWidget()
{
    mplayerProcess->close();
    delete this->NewItem;
}
//播放歌曲
void MyListWidget::MusicPlay()
{
    //由于有些是中文的，所以这里转成utf8
    QByteArray str = this->item(this->currentRow())->text().toUtf8();
    qDebug() << this->currentRow();
    qDebug() << str.data();
    memcpy(this->music, str.data(), 128);
    //如果有其他进程,相当与有歌曲在播放
    if(this->ProcessEnable == 1)
    {
        //关闭这个进程,就是关闭这首歌，播放其他的歌
        //以免开启好多进程，好多首歌曲混在一起
        mplayerProcess->write("quit\n");
        //可以播放歌曲
        this->MusicEnable = 1;
    }
    //开始
    SingSong();
    //从新设置slider的值
    emit SetTimeSliderValueSignal(0);
    //有进程
    this->ProcessEnable = 1;
}

void MyListWidget::SingSong()
{
    //mplayer 路径
    QString mplayerPath("/usr/local/bin/mplayer");
    //要播放歌曲的绝对路径
    QString music(this->musicname.at(currentRow()));
    qDebug() << "current" << this->musicname.at(currentRow());
    //MaiWindow中用到，用于判断但前选中的文件是否正在运行
    CurrentIndex = this->currentRow();
    //开启新进程
    this->mplayerProcess = new QProcess();
    QStringList args;
    //使用指令进行控制
    args<<"-slave";
    args<<"-quiet";
//    args<<"-ac";
//    args<<"mad";
    args<<"-zoom";
//    args<<"movie";
    args<<music;
    //在指定窗口播放
    args<<"-wid";
    args<<QString::number(this->wid);
    mplayerProcess->setProcessChannelMode(QProcess::MergedChannels);
    //开始播放
    mplayerProcess->start(mplayerPath,args);
}


void MyListWidget::FullScreen()
{
    this->mplayerProcess->write("vo_fullscreen 1\n");
    qDebug() << "full screen";
}

void MyListWidget::GetTime()
{
    //这两句在会于暂停按键有冲突，所以暂停的时候，把定时器关了
    mplayerProcess->write("get_time_pos\n");
    mplayerProcess->write("get_time_length\n");
    while(mplayerProcess->canReadLine())
    {
        QString message(mplayerProcess->readLine());
        QStringList message_list = message.split('=');
        QByteArray data = message.toLatin1();
        char *str = data.data();
        if(*str == 'A' && *(str+1) == 'N' && *(str+2) == 'S')
        {
            if(message_list.at(0) == "ANS_TIME_POSITION")
            {
                QString tmp(message_list.at(1));
                this->CurrentTime = int(tmp.toDouble());
                QTime time1 = int_to_time(CurrentTime);
                emit SetTimePosLabel(time1.toString("hh:mm:ss"));
            }
            else if(message_list.at(0) == "ANS_LENGTH")
            {
                QString tmp(message_list.at(1));
                this->FileLenth = int(tmp.toDouble());
                QTime time2 = int_to_time(FileLenth);
                emit SetTimeLenthLabel(time2.toString("hh:mm:ss"));
            }
            //进行时间显示
            if(CurrentTime > 0 && FileLenth > 0)
            {
                SliderValue = 100*CurrentTime/FileLenth;
                emit SetTimeSliderValueSignal(SliderValue);
            }
            //循环播放
            if(CurrentTime > 0 && FileLenth > 0 && CurrentTime >= FileLenth-10)
            {
                this->ChangeMusic(1);
            }
        }
    }
    //更新外框
//    qDebug() << "set rect";
    emit SetRectSignal();
}
//将整形的时间转为QTime
QTime MyListWidget::int_to_time(int second)
{
    int sec = 0, min = 0, hour = 0;
    QTime time;
    if(second < 60)
    {
        sec = second;
        min = 0;
        hour= 0;
    }
    if(second >= 60 && second <3600)
    {
        sec = second%60;
        min = second/60;
        hour= 0;
    }
    if(second > 3600)
    {
        sec = second%60;
        min = second/60%60;
        hour= second/3600;
    }
    time.setHMS(hour, min ,sec);
    return time;
}

//拉slider更改时间，未实现
void MyListWidget::SetTime(int value)
{
    char TimeValue[128];
    //设置进度
    sprintf(TimeValue, "seek %d 2\n", value);
    this->mplayerProcess->write(TimeValue);
}
//上一首，下一首
void MyListWidget::ChangeMusic(int value)
{
    //没有音乐的时候，上下切换无效
    if(!MusicEnable)
            return ;
    //第一个跳到最后一个
    if(this->currentRow()+value < 0)
        this->setCurrentRow(this->ItemCount-1);
    //最后一个循环到第一首
    else if(this->currentRow() + value > this->ItemCount-1)
        this->setCurrentRow(0);
    else
        this->setCurrentRow(this->currentRow()+value);
    this->MusicPlay();
    //pause的时候定时器关闭了，需要从新打开
    timer->start(10);
}
void MyListWidget::PauseSing()
{
    timer_flag = !timer_flag;
    if(timer_flag == false)
    {
        this->timer->stop();
        this->mplayerProcess->write("pause\n");
        qDebug() << "pause";
    }
    else if(timer_flag == true)
    {
        this->mplayerProcess->write("pause\n");
        qDebug() << "start";
        this->timer->start(10);
    }
}

void MyListWidget::FastForward()
{
    if(MusicEnable == 1)
    {
    //pause的时候定时器关闭了，需要从新打开
        timer->start(10);
        this->mplayerProcess->write("seek +10 0\n");
    }
    qDebug() << "fast forward";
}

void MyListWidget::FastRewind()
{
    if(MusicEnable == 1)
    {
    //pause的时候定时器关闭了，需要从新打开
        timer->start(10);
        this->mplayerProcess->write("seek -10 0\n");
    }
    qDebug() << "fast rewind";
}

void MyListWidget::ChangeVolumn(int value)
{
    char volumn[128];
    sprintf(volumn, "volumn %d 1\n", value);
    qDebug() << "volunm:" << volumn;
    this->mplayerProcess->write(volumn);
}
