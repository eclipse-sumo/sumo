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
#include <gui/popup/QGLObjectPopupMenu.h>
#include <gui/popup/QGLObjectPopupMenuItem.h>
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


QGLObjectPopupMenu *
GUITrafficLightLogicWrapper::getPopUpMenu(GUIApplicationWindow &app,
                                          GUISUMOAbstractView &parent)
{
    myApp = &app;
    QGLObjectPopupMenu *ret = new QGLObjectPopupMenu(app, parent, *this);
    int id;
    // insert name
    id = ret->insertItem(
        new QGLObjectPopupMenuItem(ret, getFullName().c_str(), true));
    ret->insertSeparator();
    // add showing option
    id = ret->insertItem("Show Phases", ret, SLOT(showPhases()));
    // add view option
    id = ret->insertItem("Center", ret, SLOT(center()));
    ret->setItemEnabled(id, TRUE);
    ret->insertSeparator();
    return ret;
}


void
GUITrafficLightLogicWrapper::showPhases()
{
    new GLObjectValuePassConnector<CompletePhaseDef>
        (*this,
        new FunctionBinding<GUITrafficLightLogicWrapper, CompletePhaseDef>
                (this, &GUITrafficLightLogicWrapper::getPhaseDef),
        new GUITLLogicPhasesTrackerWindow(*myApp, myTLLogic));
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
