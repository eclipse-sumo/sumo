#include <qthread.h>
#include "NewQMutex.h"


NewQMutex::NewQMutex()
    : _mutex(new QMutex())
{
}


NewQMutex::~NewQMutex()
{
    delete _mutex;
}


void 
NewQMutex::lock()
{
    _mutex->lock();
}


void 
NewQMutex::unlock()
{
    _mutex->unlock();
}


bool 
NewQMutex::locked ()
{
    return _mutex->locked();
}

