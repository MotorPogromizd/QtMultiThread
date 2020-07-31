#ifndef TASKEDOBJECT_H
#define TASKEDOBJECT_H

#include <functional>

#include <QObject>
#include <QQueue>
#include <QMutex>

class TaskedObject : public QObject
{
    Q_OBJECT

public:
    explicit TaskedObject(QObject *parent = nullptr);

    void addObjectThreadTask(std::function<void()> task);

public slots:
    void processObjectThreadTasks();

signals:
    void needProcessObjectThreadTasks();

private:
    QQueue<std::function<void()>> _tasks;
    QMutex _mutex;

};

#endif // TASKEDOBJECT_H
