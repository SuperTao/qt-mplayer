#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0);

    void run(char *argv);
signals:

public slots:

};

#endif // MYTHREAD_H
