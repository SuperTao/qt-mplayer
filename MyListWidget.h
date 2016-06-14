#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include <QListWidget>
#include <QProcess>
#include <dirent.h>
#include <unistd.h>
#include <QTimer>
#include <QTime>
#include <QString>
class MyListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit MyListWidget(QWidget *parent = 0);
    ~MyListWidget();
    //窗口中的歌曲
    QListWidgetItem *NewItem;
    int wid;
    //歌曲计数
    int SongCount;
    //进程标志
    int ProcessEnable;
    //音乐播放标志
    int MusicEnable;
    //进程
    QProcess *mplayerProcess ;
    //保存歌曲名称
    char music[128];
    //读取目录
    void ReadDirectory(void);
    //当前时间，用时分秒显示
    char TimePos[20];
    //歌曲进度slider,与歌曲总长度有关
    char TimeSliderRange[10];
    //当前时间，只用秒显示
    int CurrentTime;
    //歌曲时间总长度
    int FileLenth;
    //歌曲的号码，从0开始
    int  MusicNumber;
    int SliderValue;
    QTimer *timer;
    QTime int_to_time(int second);
    QList<QListWidgetItem> Items;
    QListWidgetItem *Item;
    int ItemCount;
    bool timer_flag;
        //当前正在运行的文件在列表中的索引
    int CurrentIndex;
    QStringList musicname;
signals:
    //歌曲开始信号
    void BeginSignal();
    //发送给slider，用于设置最大值
    void SetTimeSliderSignal(int value);

    void SetTimeSliderValueSignal(int value);
    //label显示歌曲名
    void SetMusicNameLabel(QString str);
    //label显示当前时间
    void SetTimePosLabel(QString str);
    //label显示最长时间
    void SetTimeLenthLabel(QString str);
    //让播放的当前的歌曲能够显示在listwidget中
    void ScrollList(const QListWidgetItem* index);
    //循环播放
    void LoopMusic(int value);
    void SetRectSignal();
public slots:
    //设置mplayer的播放的进度，还没有实现
    void SetTime(int value);
    //读取mplayer输出中的时间
    void GetTime(void);
    //改变声音大小，还没实现
    void ChangeVolumn(int value);
    //播放歌曲
    void SingSong(void);
    void PauseSing(void);
    void FastForward(void);
    void FastRewind(void);
    //更换歌曲
    void ChangeMusic(int value);
    //播放
    void MusicPlay();
    void FullScreen();
};

#endif // MYLISTWIDGET_H
