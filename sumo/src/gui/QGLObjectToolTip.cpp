#include <iostream>

#include <qtooltip.h>
#include <qcolor.h>
#include <qnamespace.h>
#include <utils/qutils/NewQMutex.h>
#include "GUIGlObjectStorage.h"
#include "GUIViewTraffic.h"
#include "GUIGlObject.h"
#include <guisim/GUINet.h>
#include <qfontmetrics.h>
#include "QGLObjectToolTip.h"

using namespace std;

QGLObjectToolTip::QGLObjectToolTip(GUIViewTraffic *parent)
    : QDialog(parent, 0, FALSE,
    Qt::WType_TopLevel | Qt::WStyle_Customize | Qt::WStyle_NoBorder
    | Qt::WStyle_Tool ),
    myParent(*parent), myLastX(-1), myLastY(-1),
    _lock(new NewQMutex())
{
    setBackgroundColor(QColor(255, 204, 0));
    _painter.begin(this);
    _painter.setPen(QColor(0, 0, 0));
    _textHeight = _painter.fontMetrics().height();
    setGeometry(0, 0, 1, 1);
    hide();
}


QGLObjectToolTip::~QGLObjectToolTip()
{
}

void
QGLObjectToolTip::setObjectTip(GUIGlObject *object,
                               size_t x, size_t y)
{
    if(object==0) {
        hide();
        return;
    }
//    GUINet::lockAlloc();
    _lock->lock();
//    hide();
    myLastX = x;
    myLastY = y;
    QString str(object->getFullName().c_str());
    QRect rect = _painter.boundingRect(0, 0, 2000, _textHeight,
        Qt::AlignLeft, str);

//    _painter.drawText(5, 5, "hallo");
//    drawText(5, 5, QString(/*object->getFullName().c_str()*/ "hallo"));

//    setGeometry(x, y, rect.width()+6, _textHeight+6);
    setGeometry(x+5, y-11-_textHeight, rect.width()+6, _textHeight+6);
    show();
    erase();
//    setGeometry(x, y, rect.width()+6, _textHeight+6);
    _painter.drawText(3, _textHeight+2, str);
    _painter.drawRect(0, 0, rect.width()+6, _textHeight+6);
    _lock->unlock();
//    GUINet::unlockAlloc();
}


