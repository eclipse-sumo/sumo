//---------------------------------------------------------------------------//
//                        GUIGLObjectToolTip.cpp -
//  A tooltip SUMORealing over a window
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
// Revision 1.3  2005/09/23 06:08:31  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.2  2005/09/15 12:19:44  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:50  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.2  2004/08/02 11:49:45  dkrajzew
// added a missing lock
//
// Revision 1.1  2004/03/19 12:56:11  dkrajzew
// porting to FOX
//
// Revision 1.8  2004/02/05 16:29:31  dkrajzew
// memory leaks removed
//
// Revision 1.7  2003/11/20 13:17:33  dkrajzew
// further work on aggregated views
//
// Revision 1.6  2003/06/05 11:37:31  dkrajzew
// class templates applied
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <iostream>
#include <string>
//#include <guisim/GUINet.h>
#include <utils/foxtools/FXMutex.h>
#include "GUIGlObjectStorage.h"
#include "GUIGlObject.h"
#include "GUIGLObjectToolTip.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIGLObjectToolTip) GUIGLObjectToolTipMap[]={
    FXMAPFUNC(SEL_PAINT, 0, GUIGLObjectToolTip::onPaint),
};

FXIMPLEMENT(GUIGLObjectToolTip,FXToolTip,GUIGLObjectToolTipMap,ARRAYNUMBER(GUIGLObjectToolTipMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIGLObjectToolTip::GUIGLObjectToolTip(FXWindow *a)
    : FXToolTip(a->getApp(), /*TOOLTIP_NORMAL*/TOOLTIP_PERMANENT, 0, 0, 200, 200),
    myLastX(-1), myLastY(-1),
    _lock(new FXEX::FXMutex()), myFont(a->getApp()->getNormalFont()),
    _object(0)
{
    setBackColor((255)|(204<<8)|(0<<16));
    _textHeight = myFont->getFontHeight();
    create();
    hide();
}


GUIGLObjectToolTip::~GUIGLObjectToolTip()
{
    // just to quit cleanly on a failure
    if(_lock->locked()) {
        _lock->unlock();
    }
    delete _lock;
}

void
GUIGLObjectToolTip::setObjectTip(GUIGlObject *object,
                                 size_t x, size_t y)
{
    _object = object;
    if(object==0) {
        hide();
        return;
    }
    _lock->lock();
    myLastX = x;
    myLastY = y;

    const std::string &name = object->getFullName();
    _width = myFont->getTextWidth(name.c_str(), name.length())+6;
    _height = _textHeight+6;
    position(x+15, y-20, _width, _height);
    if(!shown()) {
        show();
    }
    update();
    _lock->unlock();
}


long
GUIGLObjectToolTip::onPaint(FXObject*,FXSelector,void* ptr)
{
    if(_object==0) {
        return 1;
    }
    _lock->lock();
    FXint tw=0,th=0,iw=0,ih=0;
    FXEvent *ev=(FXEvent*)ptr;
    // Start drawing
    FXDCWindow dc(this);
    const std::string &name = _object->getFullName();
    dc.setForeground(backColor);
    dc.fillRectangle(0, 0,
        myFont->getTextWidth(name.c_str(), name.length())+6, _textHeight+6);
    dc.setFont(myFont);
    dc.setForeground(0);
    dc.drawText(3, _textHeight,
        _object->getFullName().c_str(), _object->getFullName().length());
    dc.drawRectangle(0, 0, myFont->getTextWidth(name.c_str(), name.length())+5, _textHeight+5);
    _lock->unlock();
    return 1;
}


FXint
GUIGLObjectToolTip::getDefaultWidth()
{
    return _width;
}


FXint
GUIGLObjectToolTip::getDefaultHeight()
{
    return _height;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


