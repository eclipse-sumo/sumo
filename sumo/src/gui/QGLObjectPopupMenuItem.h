#ifndef QGLObjectPopupMenuItem_h
#define QGLObjectPopupMenuItem_h

#include <string>
#include <qfont.h>
#include <qmenudata.h>

class QGLObjectPopupMenuItem :
    public QCustomMenuItem
{
public:
    QGLObjectPopupMenuItem(QPopupMenu *parent, const std::string &name,
        bool bold=false);
    ~QGLObjectPopupMenuItem();
    void paint ( QPainter * p, const QColorGroup & cg, bool act, bool enabled, int x, int y, int w, int h ) ;
    QSize sizeHint ();
private:
    std::string myName;
    QFont myFont;
};

#endif
