#include "MFXEventQue.h"
#include <cassert>

void *
MFXEventQue::top()
{
    assert(size()!=0);
    return *(myEvents.begin());
}


void
MFXEventQue::pop()
{
    myMutex.lock();
    myEvents.erase(myEvents.begin());
    myMutex.unlock();
}


void
MFXEventQue::add(void *what)
{
    myMutex.lock();
    myEvents.push_back(what);
    myMutex.unlock();
}


size_t
MFXEventQue::size()
{
    myMutex.lock();
    size_t msize = myEvents.size();
    myMutex.unlock();
    return msize;
}


bool
MFXEventQue::empty()
{
    return size()==0;
}


