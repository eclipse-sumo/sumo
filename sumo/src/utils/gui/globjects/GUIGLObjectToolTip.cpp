/****************************************************************************/
/// @file    GUIGLObjectToolTip.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The tooltip class for displaying information about gl-objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <string>
#include "GUIGlObjectStorage.h"
#include "GUIGlObject.h"
#include "GUIGLObjectToolTip.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIGLObjectToolTip) GUIGLObjectToolTipMap[]={
    FXMAPFUNC(SEL_PAINT, 0, GUIGLObjectToolTip::onPaint),
};

FXIMPLEMENT(GUIGLObjectToolTip,FXToolTip,GUIGLObjectToolTipMap,ARRAYNUMBER(GUIGLObjectToolTipMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIGLObjectToolTip::GUIGLObjectToolTip(FXWindow *a)
        : FXToolTip(a->getApp(), /*TOOLTIP_NORMAL*/TOOLTIP_PERMANENT, 0, 0, 200, 200),
        myFont(a->getApp()->getNormalFont()),
        myObject(0) {
    setBackColor((255)|(204<<8)|(0<<16));
    myTextHeight = myFont->getFontHeight();
    create();
    hide();
}


GUIGLObjectToolTip::~GUIGLObjectToolTip() {}

void
GUIGLObjectToolTip::setObjectTip(GUIGlObject *object,
                                 FXint x, FXint y) {
    // check whether the object has changed
    bool objectChanged = (myObject!=object);
    // save current object
    myObject = object;
    if (object==0) {
        // hide the tool tip if there is no object below
        hide();
        return;
    }
    // if the object has changed
    if (objectChanged) {
        // get the new name and width
        myObjectName = object->getFullName();
        label = myObjectName.c_str();
        myWidth = myFont->getTextWidth(myObjectName.c_str(), (FXuint) myObjectName.length())+6;
    }
    myHeight = myTextHeight+6;
    position(x+15, y-20, 1, 1);
    position(x+15, y-20, myWidth, myHeight);
    if (!shown()) {
        show();
    } else {
        if (objectChanged) {
            recalc();
        }
        update();
    }
    myLastXPos = x;
    myLastYPos = y;
}


long
GUIGLObjectToolTip::onPaint(FXObject*,FXSelector,void*) {
    if (myObject==0) {
        return 1;
    }
    // Start drawing
    FXDCWindow dc(this);
    dc.setForeground(backColor);
    dc.fillRectangle(1, 1,
                     myFont->getTextWidth(myObjectName.c_str(), (FXuint) myObjectName.length())+4, myTextHeight+4);
    dc.setFont(myFont);
    dc.setForeground(0);
    dc.drawText(3, myTextHeight,
                myObjectName.c_str(), (FXuint) myObject->getFullName().length());
    dc.drawRectangle(0, 0, myFont->getTextWidth(myObjectName.c_str(), (FXuint) myObjectName.length())+5, myTextHeight+5);
    return 1;
}


FXint
GUIGLObjectToolTip::getDefaultWidth() {
    return myWidth;
}


FXint
GUIGLObjectToolTip::getDefaultHeight() {
    return myHeight;
}


long
GUIGLObjectToolTip::onTipShow(FXObject*,FXSelector,void*) {
    return 1;
}


long
GUIGLObjectToolTip::onTipHide(FXObject*,FXSelector,void*) {
    return 1;
}



/****************************************************************************/

