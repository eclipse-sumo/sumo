#ifndef QGLObjectToolTip_h
#define QGLObjectToolTip_h

#include <qtooltip.h>

class GUIViewTraffic;
class GUIGlObject;
class NewQMutex;

class QGLObjectToolTip :
        public QToolTip
{
public:
    QGLObjectToolTip(GUIViewTraffic *parent);
    ~QGLObjectToolTip();
    void myClear();
    friend class GUIViewTraffic;
protected:
    void maybeTip( const QPoint &p );
private:
    void setObjectTip(GUIGlObject *object,
        size_t x, size_t y);
    void eraseTip();
private:
    GUIViewTraffic &myParent;
    int myLastX, myLastY;
    NewQMutex *_lock;
};

#endif

