#ifndef GUIGlObjectStorage_h
#define GUIGlObjectStorage_h

#include <map>
#include <utils/qutils/NewQMutex.h>

class GUIGlObject;

class GUIGlObjectStorage {
public:
    GUIGlObjectStorage();
    ~GUIGlObjectStorage();
    void registerObject(GUIGlObject *object);
    GUIGlObject *getObjectBlocking(size_t id);
    void remove(size_t id);
    void clear();
    void unblockObject();
private:
    typedef std::map<size_t, GUIGlObject *> ObjectMap;
    ObjectMap myMap;
    size_t myAktID;
    NewQMutex _lock;
private:
    GUIGlObjectStorage(const GUIGlObjectStorage &s);
    GUIGlObjectStorage &operator=(const GUIGlObjectStorage &s);
};

#endif

