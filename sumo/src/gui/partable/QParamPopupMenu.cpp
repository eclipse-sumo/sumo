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
// Revision 1.1  2003/05/20 09:23:58  dkrajzew
// some statistics added; some debugging done
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <qpopupmenu.h>
#include "GUIParameterTable.h"
#include <gui/GUIGlObject.h>
#include "QParamPopupMenu.h"
#include <gui/vartracker/GUIParameterTracker.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
QParamPopupMenu::QParamPopupMenu(GUIApplicationWindow *app,
                                       GUIParameterTable *parent,
                                       GUIGlObject *o,
									   int pos)
    : QPopupMenu(parent), myObject(o),/* myParent(parent),*/
    myApplication(app), myItemNo(pos)
{
}


QParamPopupMenu::~QParamPopupMenu()
{
}


void
QParamPopupMenu::newTracker()
{
    GUIParameterTracker *t =
        new GUIParameterTracker(myApplication, 0,
            myObject, myItemNo);
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "QParamPopupMenu.icc"
//#endif

// Local Variables:
// mode:C++
// End:

