#ifndef MFXEventQue_h
#define MFXEventQue_h

#include "FXMutex.h"
#include <list>

class MFXEventQue {
public:
    MFXEventQue() { }
    ~MFXEventQue() { }

    void *top();
    void pop();
    void add(void *what);
    size_t size();
    bool empty();
private:
    FXEX::FXMutex myMutex;
    std::list<void*> myEvents;
};

#endif
