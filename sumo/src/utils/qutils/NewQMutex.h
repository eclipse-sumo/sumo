#ifndef NewQMutex_h
#define NewQMutex_h

class QMutex;

class NewQMutex {
private:
    QMutex *_mutex;
public:
    NewQMutex();
    ~NewQMutex();
    void lock();
    void unlock();
    bool locked ();
};

#endif
