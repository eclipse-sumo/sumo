#ifndef GUILoadThread_h
#define GUILoadThread_h

#include <string>
#include <qthread.h>

class GUIApplicationWindow;

class GUILoadThread : public QThread
{
private:
    GUIApplicationWindow *_parent;
    std::string _file;
public:
    GUILoadThread(GUIApplicationWindow *mw);
    ~GUILoadThread();
    void init(const std::string &file);
    void run();
};

#endif
