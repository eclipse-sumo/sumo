#ifndef QGLObjectToolTip_h
#define QGLObjectToolTip_h

#include <qdialog.h>
#include <qpainter.h>

class GUISUMOAbstractView;
class GUIGlObject;
class NewQMutex;

class QGLObjectToolTip :
        public QDialog
{
public:
    QGLObjectToolTip(GUISUMOAbstractView *parent);
    ~QGLObjectToolTip();
    friend class GUISUMOAbstractView;
private:
    void setObjectTip(GUIGlObject *object,
        size_t x, size_t y);
    void eraseTip();
private:
    GUISUMOAbstractView &myParent;
    int myLastX, myLastY;
    NewQMutex *_lock;
    QPainter _painter;
    int _textHeight;
};

#endif

