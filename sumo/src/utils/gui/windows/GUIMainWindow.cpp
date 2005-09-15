//---------------------------------------------------------------------------//
//                        GUIMainWindow.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.4  2005/09/15 12:20:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 12:52:07  dkrajzew
// code style adapted
//
// Revision 1.3  2005/06/14 11:29:51  dksumo
// documentation added
//


/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "GUIMainWindow.h"
#include <algorithm>
#include "GUIAppEnum.h"
#include <fx3d.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * member method definitions
 * ======================================================================= */

GUIMainWindow::GUIMainWindow(FXApp* a, int glWidth, int glHeight)
	: FXMainWindow(a,"SUMO-gui main window",NULL,NULL,DECOR_ALL,20,20,600,400),
    myGLVisual(new FXGLVisual(a, VISUAL_DOUBLEBUFFER|VISUAL_STEREO)),
    myGLWidth(glWidth), myGLHeight(glHeight), myRunAtBegin(false)
{

    FXFontDesc fdesc;
    getApp()->getNormalFont()->getFontDesc(fdesc);
    fdesc.weight = FONTWEIGHT_BOLD;
    myBoldFont = new FXFont(getApp(), fdesc);

    myTopDock=new FXDockSite(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
    myBottomDock=new FXDockSite(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
    myLeftDock=new FXDockSite(this,LAYOUT_SIDE_LEFT|LAYOUT_FILL_Y);
    myRightDock=new FXDockSite(this,LAYOUT_SIDE_RIGHT|LAYOUT_FILL_Y);
}


GUIMainWindow::~GUIMainWindow()
{
    delete myBoldFont;
    delete myTopDock;
    delete myBottomDock;
    delete myLeftDock;
    delete myRightDock;
}



void
GUIMainWindow::addChild(FXMDIChild *child, bool updateOnSimStep)
{
    mySubWindows.push_back(child);
}


void
GUIMainWindow::removeChild(FXMDIChild *child)
{
    std::vector<FXMDIChild*>::iterator i =
        std::find(mySubWindows.begin(), mySubWindows.end(), child);
    if(i!=mySubWindows.end()) {
        mySubWindows.erase(i);
    }
}


void
GUIMainWindow::addChild(FXMainWindow *child, bool updateOnSimStep)
{
    myTrackerLock.lock();
    myTrackerWindows.push_back(child);
    myTrackerLock.unlock();
}


void
GUIMainWindow::removeChild(FXMainWindow *child)
{
    myTrackerLock.lock();
    std::vector<FXMainWindow*>::iterator i =
        std::find(myTrackerWindows.begin(), myTrackerWindows.end(), child);
    myTrackerWindows.erase(i);
    myTrackerLock.unlock();
}


FXFont *
GUIMainWindow::getBoldFont()
{
    return myBoldFont;
}


int
GUIMainWindow::getMaxGLWidth() const
{
    return myGLWidth;
}


int
GUIMainWindow::getMaxGLHeight() const
{
    return myGLHeight;
}

void
GUIMainWindow::updateChildren()
{
    // inform views
    myMDIClient->forallWindows(this, FXSEL(SEL_COMMAND, MID_SIMSTEP), 0);
    // inform other windows
    myTrackerLock.lock();
    for(size_t i=0; i<myTrackerWindows.size(); i++) {
        myTrackerWindows[i]->handle(this,FXSEL(SEL_COMMAND,MID_SIMSTEP), 0);
    }
    myTrackerLock.unlock();
}

FXGLVisual *
GUIMainWindow::getGLVisual() const
{
    return myGLVisual;
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

