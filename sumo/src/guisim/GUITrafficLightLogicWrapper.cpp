//---------------------------------------------------------------------------//
//                        GUITrafficLightLogicWrapper.cpp -
//  A wrapper for tl-logics to allow their visualisation and interaction
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Oct/Nov 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.3  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.2  2003/11/26 09:48:58  dkrajzew
// time display added to the tl-logic visualisation
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <gui/GUIGlObject.h>
#include <gui/GUIGlObjectStorage.h>
#include <gui/GUIApplicationWindow.h>
#include <microsim/MSTrafficLightLogic.h>
#include <gui/popup/GUIGLObjectPopupMenu.h>
#include <gui/GUIGlobals.h>
#include <gui/GUIAppEnum.h>
#include <guisim/guilogging/GLObjectValuePassConnector.h>
#include <microsim/logging/FunctionBinding.h>
#include <gui/tlstracker/GUITLLogicPhasesTrackerWindow.h>
#include "GUITrafficLightLogicWrapper.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member metho definitions
 * ======================================================================= */
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapper(
        GUIGlObjectStorage &idStorage, MSTrafficLightLogic &tll)
    : GUIGlObject(idStorage, string("tl-logic:")+tll.id()), myTLLogic(tll)
{
}


GUITrafficLightLogicWrapper::~GUITrafficLightLogicWrapper()
{
}


GUIGLObjectPopupMenu *
GUITrafficLightLogicWrapper::getPopUpMenu(GUIApplicationWindow &app,
                                          GUISUMOAbstractView &parent)
{
    myApp = &app;
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    new FXMenuCommand(ret, getFullName().c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Center", 0, ret, MID_CENTER);
    new FXMenuSeparator(ret);
    //
    if(gfIsSelected(GLO_LANE, getGlID())) {
        new FXMenuCommand(ret, "Remove From Selected", 0, ret, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected", 0, ret, MID_ADDSELECT);
    }
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Show Phases", 0, ret, MID_SHOWPHASES);
    return ret;
}


void
GUITrafficLightLogicWrapper::showPhases()
{
    GUITLLogicPhasesTrackerWindow *window =
        new GUITLLogicPhasesTrackerWindow(*myApp, myTLLogic, *this,
            new FunctionBinding<GUITrafficLightLogicWrapper, CompletePhaseDef>
                (this, &GUITrafficLightLogicWrapper::getPhaseDef));
    window->create();
    window->show();
}


CompletePhaseDef
GUITrafficLightLogicWrapper::getPhaseDef() const
{
    return CompletePhaseDef(
        MSNet::getInstance()->getCurrentTimeStep(),
        SimplePhaseDef(myTLLogic.allowed(), myTLLogic.yellowMask()));
}


GUIParameterTableWindow *
GUITrafficLightLogicWrapper::getParameterWindow(GUIApplicationWindow &app,
                                                GUISUMOAbstractView &parent)
{
    return 0;
}


GUIGlObjectType
GUITrafficLightLogicWrapper::getType() const
{
    return GLO_TLLOGIC;
}


bool
GUITrafficLightLogicWrapper::active() const
{
    return true;
}


std::string
GUITrafficLightLogicWrapper::microsimID() const
{
    return myTLLogic.id();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
