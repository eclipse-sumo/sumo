#include <qmenudata.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qfont.h>
#include "QGLObjectPopupMenuItem.h"


QGLObjectPopupMenuItem::QGLObjectPopupMenuItem(QPopupMenu *parent,
                                               const std::string &name,
                                               bool bold)
    : myName(name), myFont(parent->font())
{
    if(bold) {
        myFont.setBold(TRUE);
    }
}


QGLObjectPopupMenuItem::~QGLObjectPopupMenuItem()
{
}


void
QGLObjectPopupMenuItem::paint ( QPainter * p, const QColorGroup & cg,
                               bool act, bool enabled,
                               int x, int y, int w, int h )
{
    p->setFont(myFont);
    p->drawText( x, y, w, h, AlignLeft | AlignVCenter | ShowPrefix | DontClip, myName.c_str() );
}


QSize
QGLObjectPopupMenuItem::sizeHint ()
{
    return QFontMetrics( myFont ).size( AlignLeft | AlignVCenter | ShowPrefix | DontClip,  myName.c_str() );
}
