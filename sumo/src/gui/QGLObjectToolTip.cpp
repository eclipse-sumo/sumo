#include <iostream>

#include <qtooltip.h>
#include <utils/qutils/NewQMutex.h>
#include "GUIGlObjectStorage.h"
#include "GUIViewTraffic.h"
#include "GUIGlObject.h"
#include <guisim/GUINet.h>
#include "QGLObjectToolTip.h"

using namespace std;

QGLObjectToolTip::QGLObjectToolTip(GUIViewTraffic *parent)
    : QToolTip(parent), myParent(*parent), myLastX(-1), myLastY(-1),
    _lock(new NewQMutex())
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
//    GUINet::lockAlloc();
    _lock->lock();
/*    if(myLastX==x&&myLastY==y) {
        _lock->unlock();
        GUINet::unlockAlloc();
        return;
    }*/

    if(myLastX!=-1&&myLastY!=-1) {
/*        QToolTip::remove(&myParent, 
            QRect(myLastX-1, myLastY-1, myLastX+1, myLastY+1));*/
//        hide();
        myLastX = -1;
        myLastY = -1;
    }

    myLastX = x;
    myLastY = y;
    tip(
        QRect(x-1, y-1, x+1, y+1),
        QString(object->getFullName().c_str()));
    _lock->unlock();
//    GUINet::unlockAlloc();
}



void 
QGLObjectToolTip::myClear()
{
    if(myLastX!=-1&&myLastY!=-1) {
        cout << "Tip: remove#1" << endl;
        QToolTip::remove(&myParent, 
            QRect(myLastX-1, myLastY-1, myLastX+1, myLastY+1));
        myLastX = -1;
        myLastY = -1;
    }
    remove(&myParent);
    clear();
}
