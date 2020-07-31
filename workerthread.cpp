#include "workerthread.h"

#include <QMutexLocker>
#include "QtDebugPrint/debugoutput.h"

/********************************************************************
 * Public
 ********************************************************************
 */

WorkerThread::WorkerThread(QObject* parent)
    : QThread(parent)
    , _tasks()
    , _mutex()
    , _condition()
    , _needToAbort{false}
{

}

WorkerThread::~WorkerThread() {
    _mutex.lock();
    _needToAbort = true;
    _condition.wakeOne();
    _mutex.unlock();
    wait();
}

void WorkerThread::work(std::function<void()> task) {
    QMutexLocker l(&_mutex);
    Q_UNUSED(l);
    _tasks.enqueue(task);

    if (!isRunning()) {
        start();
    }
    else {
        _condition.wakeOne();
    }
}

/********************************************************************
 * Protected override
 ********************************************************************
 */

void WorkerThread::run() {
    for(;;) {
        _mutex.lock();
        if (_needToAbort) {
            _mutex.unlock();
            return;
        }
        auto currentTask = _tasks.dequeue();
        _mutex.unlock();

        currentTask();

        _mutex.lock();
        if (_tasks.isEmpty() && !_needToAbort) {
            //no tasks, thread going to sleep
            _condition.wait(&_mutex);
        }
        _mutex.unlock();
    }

}
