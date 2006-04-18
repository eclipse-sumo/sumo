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
// Revision 1.14  2006/04/18 08:12:04  dkrajzew
// consolidation of interaction with gl-objects
//
// Revision 1.13  2006/04/11 10:56:32  dkrajzew
// microsimID() now returns a const reference
//
// Revision 1.12  2006/03/28 06:12:54  dkrajzew
// unneeded string wrapping removed
//
// Revision 1.11  2006/02/27 12:03:22  dkrajzew
// variants container named properly
//
// Revision 1.10  2006/02/23 11:27:56  dkrajzew
// tls may have now several programs
//
// Revision 1.9  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/01/27 14:20:26  dkrajzew
// ability to open the complete phase definition added
//
// Revision 1.6  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.5  2004/07/02 08:54:11  dkrajzew
// some design issues
//
// Revision 1.4  2004/04/02 11:18:37  dkrajzew
// recenter view - icon added to the popup menu
//
// Revision 1.3  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.2  2003/11/26 09:48:58  dkrajzew
// time display added to the tl-logic visualisation
//
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

#include <cassert>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <gui/GUIApplicationWindow.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <guisim/guilogging/GLObjectValuePassConnector.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/tracker/GUITLLogicPhasesTrackerWindow.h>
#include "GUITrafficLightLogicWrapper.h"
#include <utils/gui/div/GUIGlobalSelection.h>

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
FXDEFMAP(GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu)
    GUITrafficLightLogicWrapperPopupMenuMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWPHASES,             GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdShowPhases),
    FXMAPFUNC(SEL_COMMAND,  MID_TRACKPHASES,            GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdBegin2TrackPhases),
    FXMAPFUNCS(SEL_COMMAND, MID_SWITCH, MID_SWITCH+20, GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLSLogic),
};

// Object implementation
FXIMPLEMENT(GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu, GUIGLObjectPopupMenu, GUITrafficLightLogicWrapperPopupMenuMap, ARRAYNUMBER(GUITrafficLightLogicWrapperPopupMenuMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::GUITrafficLightLogicWrapperPopupMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent,
        GUIGlObject &o)
    : GUIGLObjectPopupMenu(app, parent, o)
{
}


GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::~GUITrafficLightLogicWrapperPopupMenu()
{
}



long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdBegin2TrackPhases(
        FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->begin2TrackPhases();
    return 1;
}


long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdShowPhases(
        FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->showPhases();
    return 1;
}


long
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapperPopupMenu::onCmdSwitchTLSLogic(
        FXObject*,FXSelector sel,void* )
{
    assert(myObject->getType()==GLO_TLLOGIC);
    static_cast<GUITrafficLightLogicWrapper*>(myObject)->switchTLSLogic(FXSELID(sel)-MID_SWITCH);
    return 1;
}



/* -------------------------------------------------------------------------
 * GUITrafficLightLogicWrapper - methods
 * ----------------------------------------------------------------------- */
GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapper(
        GUIGlObjectStorage &idStorage,
        MSTLLogicControl &control, MSTrafficLightLogic &tll)
    : GUIGlObject(idStorage, "tl-logic:"+tll.getID()),
    myTLLogicControl(control), myTLLogic(tll)
{
}


GUITrafficLightLogicWrapper::~GUITrafficLightLogicWrapper()
{
}


GUIGLObjectPopupMenu *
GUITrafficLightLogicWrapper::getPopUpMenu(GUIMainWindow &app,
                                          GUISUMOAbstractView &parent)
{
    myApp = &app;
    GUIGLObjectPopupMenu *ret = new GUITrafficLightLogicWrapperPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    //
    const MSTLLogicControl::TLSLogicVariants &vars = myTLLogicControl.get(myTLLogic.getID());
    if(vars.ltVariants.size()>1) {
        std::map<std::string, MSTrafficLightLogic*>::const_iterator i;
        size_t index = 0;
        for(i=vars.ltVariants.begin(); i!=vars.ltVariants.end(); ++i, ++index) {
            if((*i).second!=vars.defaultTL) {
                new FXMenuCommand(ret, ("Switch to '" + (*i).second->getSubID() + "'").c_str(),
                    GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_SWITCH+index);
            }
        }
        new FXMenuSeparator(ret);
    }
    new FXMenuCommand(ret, "Track Phases", 0, ret, MID_TRACKPHASES);
    new FXMenuCommand(ret, "Show Phases", 0, ret, MID_SHOWPHASES);
    new FXMenuSeparator(ret);
    //
    buildSelectionPopupEntry(ret, false);
    return ret;
}


void
GUITrafficLightLogicWrapper::begin2TrackPhases()
{
    GUITLLogicPhasesTrackerWindow *window =
        new GUITLLogicPhasesTrackerWindow(*myApp, myTLLogic, *this,
            new FunctionBinding<GUITrafficLightLogicWrapper, CompletePhaseDef>
                (this, &GUITrafficLightLogicWrapper::getPhaseDef));
    window->create();
    window->show();
}


void
GUITrafficLightLogicWrapper::showPhases()
{
    GUITLLogicPhasesTrackerWindow *window =
        new GUITLLogicPhasesTrackerWindow(*myApp, myTLLogic, *this,
            static_cast<MSSimpleTrafficLightLogic&>(myTLLogic).getPhases());
    window->setBeginTime(0);
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
GUITrafficLightLogicWrapper::getParameterWindow(GUIMainWindow &app,
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


const std::string &
GUITrafficLightLogicWrapper::microsimID() const
{
    return myTLLogic.getID();
}

Boundary
GUITrafficLightLogicWrapper::getCenteringBoundary() const
{
	throw 1;
}


void
GUITrafficLightLogicWrapper::switchTLSLogic(int to)
{
    const MSTLLogicControl::TLSLogicVariants &vars = myTLLogicControl.get(myTLLogic.getID());
    std::map<std::string, MSTrafficLightLogic*>::const_iterator i;
    size_t index = 0;
    for(i=vars.ltVariants.begin(); i!=vars.ltVariants.end(); ++i, ++index) {
        if(index==to) {
            myTLLogicControl.switchTo((*i).second->getID(), (*i).second->getSubID());
            return;
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
