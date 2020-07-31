#include "taskedobject.h"

#include <QCoreApplication>

TaskedObject::TaskedObject(QObject *parent)
    : QObject(parent)
    , _tasks()
    , _mutex()
{        
    connect(this, &TaskedObject::needProcessObjectThreadTasks,
            this, &TaskedObject::processObjectThreadTasks,
            Qt::ConnectionType::QueuedConnection);
}

void TaskedObject::addObjectThreadTask(std::function<void ()> task) {
    _mutex.lock();
    bool needEmitSignal = _tasks.isEmpty();
    _tasks.enqueue(task);
    _mutex.unlock();
    if (needEmitSignal) {
        emit needProcessObjectThreadTasks();
    }
}

void TaskedObject::processObjectThreadTasks() {
    _mutex.lock();
    while (!_tasks.isEmpty()) {
        auto task = _tasks.head();
        _mutex.unlock();
        task();
        _mutex.lock();
        _tasks.dequeue();
    }
    _mutex.unlock();
}
