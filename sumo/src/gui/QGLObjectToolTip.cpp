#include <iostream>

#include <qtooltip.h>
#include "GUIGlObjectStorage.h"
#include "GUIViewTraffic.h"
#include "GUIGlObject.h"
#include "QGLObjectToolTip.h"

using namespace std;

QGLObjectToolTip::QGLObjectToolTip(GUIViewTraffic *parent)
    : QToolTip(parent), myParent(*parent)
{
}


QGLObjectToolTip::~QGLObjectToolTip()
{
}


void
QGLObjectToolTip::maybeTip(const QPoint &p)
{
    myParent.setTooltipPosition(p.x(), p.y());
    myParent.update();
}



void
QGLObjectToolTip::setObjectTip(GUIGlObject *object,
                               size_t x, size_t y)
{
    tip(
        QRect(x-3, y-3, x+3, y+3),
        QString(object->getFullName().c_str()));
}
