//---------------------------------------------------------------------------//
//                        QGLObjectToolTip.cpp -
//  A tooltip floating over a window
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.7  2003/11/20 13:17:33  dkrajzew
// further work on aggregated views
//
// Revision 1.6  2003/06/05 11:37:31  dkrajzew
// class templates applied
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>

#include <guisim/GUINet.h>

#include <qtooltip.h>
#include <qcolor.h>
#include <qnamespace.h>
#include <utils/qutils/NewQMutex.h>
#include "GUIGlObjectStorage.h"
#include "GUISUMOAbstractView.h"
#include "GUIGlObject.h"
#include <qfontmetrics.h>
#include "QGLObjectToolTip.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
QGLObjectToolTip::QGLObjectToolTip(GUISUMOAbstractView *parent)
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
    _lock->lock();
    myLastX = x;
    myLastY = y;
    QString str(object->getFullName().c_str());
    QRect rect = _painter.boundingRect(0, 0, 2000, _textHeight,
        Qt::AlignLeft, str);

    setGeometry(x+5, y-11-_textHeight, rect.width()+6, _textHeight+6);
    show();
    erase();
    _painter.drawText(3, _textHeight+2, str);
    _painter.drawRect(0, 0, rect.width()+6, _textHeight+6);
    _lock->unlock();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "QGLObjectToolTip.icc"
//#endif

// Local Variables:
// mode:C++
// End:


