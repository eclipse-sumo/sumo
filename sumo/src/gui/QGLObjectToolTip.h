#ifndef QGLObjectToolTip_h
#define QGLObjectToolTip_h

#include <qtooltip.h>

class GUIViewTraffic;
class GUIGlObject;

class QGLObjectToolTip :
        public QToolTip
{
public:
    QGLObjectToolTip(GUIViewTraffic *parent);
    ~QGLObjectToolTip();
    friend class GUIViewTraffic;
protected:
    void maybeTip( const QPoint &p );
private:
    void setObjectTip(GUIGlObject *object,
        size_t x, size_t y);
private:
    GUIViewTraffic &myParent;
};

#endif

