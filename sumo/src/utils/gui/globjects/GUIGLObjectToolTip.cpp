//---------------------------------------------------------------------------//
//                        GUIGLObjectToolTip.cpp -
//  The tooltip class for displaying information about gl-objects
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
// Revision 1.8  2006/11/28 12:10:44  dkrajzew
// got rid of FXEX-Mutex (now using the one supplied in FOX)
//
// Revision 1.7  2006/11/16 12:30:54  dkrajzew
// warnings removed
//
// Revision 1.6  2006/11/16 10:50:52  dkrajzew
// warnings removed
//
// Revision 1.5  2006/10/12 10:14:32  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
//
// Revision 1.4  2005/10/07 11:45:32  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
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
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <iostream>
#include <string>
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
    myFont(a->getApp()->getNormalFont()),
    myObject(0)
{
    setBackColor((255)|(204<<8)|(0<<16));
    myTextHeight = myFont->getFontHeight();
    create();
    hide();
}


GUIGLObjectToolTip::~GUIGLObjectToolTip()
{
}

void
GUIGLObjectToolTip::setObjectTip(GUIGlObject *object,
                                 size_t x, size_t y)
{
    // check whether the object has changed
    bool objectChanged = (myObject!=object);
    // save current object
    myObject = object;
    if(object==0) {
        // hide the tool tip if there is no object below
        hide();
        return;
    }
    // if the object has changed
    if(objectChanged) {
        // get the new name and width
        myObjectName = object->getFullName();
        label = myObjectName.c_str();
        myWidth = myFont->getTextWidth(myObjectName.c_str(), myObjectName.length())+6;
    }
    myHeight = myTextHeight+6;
    position(x+15, y-20, 1, 1);
    position(x+15, y-20, myWidth, myHeight);
    if(!shown()) {
        show();
    } else {
        if(objectChanged) {
            recalc();
        }
        update();
    }
    myLastXPos = x;
    myLastYPos = y;
}


long
GUIGLObjectToolTip::onPaint(FXObject*,FXSelector,void* )
{
    if(myObject==0) {
        return 1;
    }
    // Start drawing
    FXDCWindow dc(this);
    dc.setForeground(backColor);
    dc.fillRectangle(1, 1,
        myFont->getTextWidth(myObjectName.c_str(), myObjectName.length())+4, myTextHeight+4);
    dc.setFont(myFont);
    dc.setForeground(0);
    dc.drawText(3, myTextHeight,
        myObjectName.c_str(), myObject->getFullName().length());
    dc.drawRectangle(0, 0, myFont->getTextWidth(myObjectName.c_str(), myObjectName.length())+5, myTextHeight+5);
    return 1;
}


FXint
GUIGLObjectToolTip::getDefaultWidth()
{
    return myWidth;
}


FXint
GUIGLObjectToolTip::getDefaultHeight()
{
    return myHeight;
}


long 
GUIGLObjectToolTip::onTipShow(FXObject*,FXSelector,void*)
{
    return 1;
}


long 
GUIGLObjectToolTip::onTipHide(FXObject*,FXSelector,void*)
{
    return 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


