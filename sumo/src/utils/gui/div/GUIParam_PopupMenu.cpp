/***************************************************************************
                          GUIParam_PopupMenu.cpp
    The popup-menu which appears hen pressing right mouse button over a
     parameter table
                             -------------------
    project              : SUMO - Simulation of Urban MObility
    begin                : Mai 2003
    copyright            : (C) 2003 by Daniel Krajzewicz
    organisation         : IVF/DLR http://ivf.dlr.de
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.5  2005/10/07 11:44:53  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/23 06:07:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/09/15 12:18:59  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/07/12 11:55:37  dkrajzew
// fonts are now drawn using polyfonts; dialogs have icons; searching for structures improved;
//
// Revision 1.1  2004/11/23 10:38:29  dkrajzew
// debugging
//
// Revision 1.2  2004/10/29 06:20:47  dksumo
// patched some false dependencies
//
// Revision 1.1  2004/10/22 12:50:46  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.1  2004/03/19 12:40:14  dkrajzew
// porting to FOX
//
// Revision 1.6  2003/11/12 14:09:13  dkrajzew
// clean up after recent changes; comments added
//
// Revision 1.5  2003/11/11 08:44:05  dkrajzew
// synchronisation problems of parameter tracker updates patched; logging
//  moved from utils to microsim
//
// Revision 1.4  2003/07/30 08:48:28  dkrajzew
// new parameter table usage paradigm; undocummented yet
//
// Revision 1.3  2003/07/18 12:30:14  dkrajzew
// removed some warnings
//
// Revision 1.2  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.1  2003/05/20 09:23:58  dkrajzew
// some statistics added; some debugging done
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
#include "GUIParameterTableWindow.h"
#include <utils/gui/globjects/GUIGlObject.h>
#include "GUIParam_PopupMenu.h"
#include <utils/gui/tracker/GUIParameterTracker.h>
#include <utils/gui/tracker/TrackerValueDesc.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <microsim/MSNet.h>

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
FXDEFMAP(GUIParam_PopupMenu) GUIParam_PopupMenuMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_OPENTRACKER, GUIParam_PopupMenu::onCmdOpenTracker),
};

// Object implementation
FXIMPLEMENT(GUIParam_PopupMenu, FXMenuPane, GUIParam_PopupMenuMap, ARRAYNUMBER(GUIParam_PopupMenuMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIParam_PopupMenu::GUIParam_PopupMenu(GUIMainWindow &app,
//                                 GUIParameterTable &parent,
                                 GUIParameterTableWindow &parentWindow,
                                 GUIGlObject &o,
                                 const std::string &varName,
                                 ValueSource<SUMOReal> *src)
    : FXMenuPane(&parentWindow), myObject(&o), //myParent(&parent),
    myParentWindow(&parentWindow), myApplication(&app), myVarName(varName),
    mySource(src)
{
}


GUIParam_PopupMenu::~GUIParam_PopupMenu()
{
    delete mySource;
}


long
GUIParam_PopupMenu::onCmdOpenTracker(FXObject*,FXSelector,void*)
{
    string trackerName = myVarName + " from " + myObject->getFullName();
    GUIParameterTracker *tr = new GUIParameterTracker(*myApplication,
        trackerName, *myObject, 0, 0);
    TrackerValueDesc *newTracked = new TrackerValueDesc(
		myVarName, RGBColor(0, 0, 0), myObject, myApplication->getCurrentSimTime());
    tr->addTracked(*myObject, mySource->copy(), newTracked);
    tr->create();
    tr->show();
    return 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

