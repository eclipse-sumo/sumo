#ifndef QGLObjectToolTip_h
#define QGLObjectToolTip_h

#include <qdialog.h>
#include <qpainter.h>

class GUIViewTraffic;
class GUIGlObject;
class NewQMutex;

class QGLObjectToolTip :
        public QDialog
{
public:
    QGLObjectToolTip(GUIViewTraffic *parent);
    ~QGLObjectToolTip();
    friend class GUIViewTraffic;
private:
    void setObjectTip(GUIGlObject *object,
        size_t x, size_t y);
    void eraseTip();
private:
    GUIViewTraffic &myParent;
    int myLastX, myLastY;
    NewQMutex *_lock;
    QPainter _painter;
    int _textHeight;
};

#endif

