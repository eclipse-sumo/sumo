/***************************************************************************
                          QParamPopupMenu.cpp
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
// Revision 1.6  2003/11/12 14:09:13  dkrajzew
// clean up after recent changes; comments added
//
// Revision 1.5  2003/11/11 08:44:05  dkrajzew
// synchronisation problems of parameter tracker updates patched; logging moved from utils to microsim
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <string>
#include <qpopupmenu.h>
#include "GUIParameterTable.h"
#include "GUIParameterTableWindow.h"
#include <gui/GUIGlObject.h>
#include "QParamPopupMenu.h"
#include <gui/vartracker/GUIParameterTracker.h>
#include <gui/vartracker/TrackerValueDesc.h>
#include <gui/GUIApplicationWindow.h>
#include <guisim/guilogging/GLObjectValuePassConnector.h>

#ifndef WIN32
#include "QParamPopupMenu.moc"
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
QParamPopupMenu::QParamPopupMenu(GUIApplicationWindow &app,
                                 GUIParameterTable &parent,
                                 GUIParameterTableWindow &parentWindow,
                                 GUIGlObject &o,
                                 const std::string &varName,
                                 ValueSource<double> *src)
    : QPopupMenu(&parent), myObject(o), myParent(parent),
    myParentWindow(parentWindow), myApplication(app), myVarName(varName),
    mySource(src)
{
}


QParamPopupMenu::~QParamPopupMenu()
{
    delete mySource;
}


void
QParamPopupMenu::newTracker()
{
    GUIParameterTracker *tr = new GUIParameterTracker(
        myApplication, myVarName, myObject, /*mySource, */0, 0);
    TrackerValueDesc *newTracked = new TrackerValueDesc(
            myVarName, RGBColor(0, 0, 0), &myObject);
    tr->addTracked(newTracked);
    // build connection (is automatically set into an execution map)
    new GLObjectValuePassConnector<double>(myObject,
        mySource->copy(), newTracked);
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "QParamPopupMenu.icc"
//#endif

// Local Variables:
// mode:C++
// End:

