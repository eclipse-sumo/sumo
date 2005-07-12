#ifndef AbstractMutex_h
#define AbstractMutex_h



class AbstractMutex {
public:
    AbstractMutex() { }
    virtual ~AbstractMutex() { }
    virtual void lock() = 0;
    virtual void unlock() = 0;

};


#endif

