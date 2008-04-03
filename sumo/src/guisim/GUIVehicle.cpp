/****************************************************************************/
/// @file    GUIVehicle.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A MSVehicle extended by some values for usage within the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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

#include <cmath>
#include <vector>
#include <string>
#include <utils/common/StringUtils.h>
#include <microsim/MSVehicle.h>
#include "GUINet.h"
#include "GUIVehicle.h"
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSVehicleControl.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <guisim/GUIVehicleType.h>
#include <guisim/GUIRoute.h>
#include <utils/common/RandHelper.h>
#include <microsim/MSAbstractLaneChangeModel.h>

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
FXDEFMAP(GUIVehicle::GUIVehiclePopupMenu) GUIVehiclePopupMenuMap[]= {
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_ALLROUTES, GUIVehicle::GUIVehiclePopupMenu::onCmdShowAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_ALLROUTES, GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_CURRENTROUTE, GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_CURRENTROUTE, GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_BEST_LANES, GUIVehicle::GUIVehiclePopupMenu::onCmdShowBestLanes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_BEST_LANES, GUIVehicle::GUIVehiclePopupMenu::onCmdHideBestLanes),
    FXMAPFUNC(SEL_COMMAND, MID_START_TRACK, GUIVehicle::GUIVehiclePopupMenu::onCmdStartTrack),
    FXMAPFUNC(SEL_COMMAND, MID_STOP_TRACK, GUIVehicle::GUIVehiclePopupMenu::onCmdStopTrack),
};

// Object implementation
FXIMPLEMENT(GUIVehicle::GUIVehiclePopupMenu, GUIGLObjectPopupMenu, GUIVehiclePopupMenuMap, ARRAYNUMBER(GUIVehiclePopupMenuMap))


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIVehicle::GUIVehiclePopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIVehicle::GUIVehiclePopupMenu::GUIVehiclePopupMenu(
    GUIMainWindow &app, GUISUMOAbstractView &parent,
    GUIGlObject &o)
        : GUIGLObjectPopupMenu(app, parent, o)
{
}


GUIVehicle::GUIVehiclePopupMenu::~GUIVehiclePopupMenu()
{}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowAllRoutes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->showRoute(static_cast<GUIVehicle*>(myObject), -1);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->hideRoute(static_cast<GUIVehicle*>(myObject), -1);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->showRoute(static_cast<GUIVehicle*>(myObject), 0);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowBestLanes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->showBestLanes(static_cast<GUIVehicle*>(myObject));
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->hideRoute(static_cast<GUIVehicle*>(myObject), 0);
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideBestLanes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->hideBestLanes(static_cast<GUIVehicle*>(myObject));
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdStartTrack(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->startTrack(static_cast<GUIVehicle*>(myObject)->getGlID());
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdStopTrack(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->stopTrack();
    return 1;
}


/* -------------------------------------------------------------------------
 * GUIVehicle - methods
 * ----------------------------------------------------------------------- */
GUIVehicle::GUIVehicle(GUIGlObjectStorage &idStorage,
                       std::string id, MSRoute* route,
                       SUMOTime departTime,
                       const MSVehicleType* type,
                       int repNo, int repOffset, int vehicleIndex) throw()
        : MSVehicle(id, route, departTime, type, repNo, repOffset, vehicleIndex),
        GUIGlObject(idStorage, "vehicle:"+id)
{
    myIntCORNMap[MSCORN::CORN_VEH_BLINKER] = 0;
}


GUIVehicle::~GUIVehicle() throw()
{
    if (hasCORNPointerValue(MSCORN::CORN_P_VEH_OWNCOL)) {
        delete(RGBColor *) myPointerCORNMap[MSCORN::CORN_P_VEH_OWNCOL];
    }
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


SUMOReal
GUIVehicle::getTimeSinceLastLaneChangeAsReal() const
{
    return (SUMOReal) myLastLaneChangeOffset;
}


MSVehicle *
GUIVehicle::getNextPeriodical() const
{
    GUIVehicle *ret = (GUIVehicle*)MSVehicle::getNextPeriodical();
    if (ret!=0 && hasCORNPointerValue(MSCORN::CORN_P_VEH_OWNCOL)) {
        RGBColor *col = (RGBColor *)getCORNPointerValue(MSCORN::CORN_P_VEH_OWNCOL);
        ret->setCORNColor(col->red(), col->green(), col->blue());
    }
    return ret;
}


void
GUIVehicle::setCORNColor(SUMOReal red, SUMOReal green, SUMOReal blue)
{
    if (hasCORNPointerValue(MSCORN::CORN_P_VEH_OWNCOL)) {
        delete(RGBColor *) myPointerCORNMap[MSCORN::CORN_P_VEH_OWNCOL];
    }
    myPointerCORNMap[MSCORN::CORN_P_VEH_OWNCOL] = new RGBColor(red, green, blue);
}


GUIGLObjectPopupMenu *
GUIVehicle::getPopUpMenu(GUIMainWindow &app,
                         GUISUMOAbstractView &parent) throw()
{
    GUIGLObjectPopupMenu *ret = new GUIVehiclePopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //
    if (parent.amShowingRouteFor(this, 0)) {
        new FXMenuCommand(ret, "Hide Current Route", 0, ret, MID_HIDE_CURRENTROUTE);
    } else {
        new FXMenuCommand(ret, "Show Current Route", 0, ret, MID_SHOW_CURRENTROUTE);
    }
    if (parent.amShowingRouteFor(this, -1)) {
        new FXMenuCommand(ret, "Hide All Routes", 0, ret, MID_HIDE_ALLROUTES);
    } else {
        new FXMenuCommand(ret, "Show All Routes", 0, ret, MID_SHOW_ALLROUTES);
    }
    if (parent.amShowingBestLanesFor(this)) {
        new FXMenuCommand(ret, "Hide Best Lanes", 0, ret, MID_HIDE_BEST_LANES);
    } else {
        new FXMenuCommand(ret, "Show Best Lanes", 0, ret, MID_SHOW_BEST_LANES);
    }
    new FXMenuSeparator(ret);
    int trackedID = parent.getTrackedID();
    if (trackedID<0||(size_t)trackedID!=getGlID()) {
        new FXMenuCommand(ret, "Start Tracking", 0, ret, MID_START_TRACK);
    } else {
        new FXMenuCommand(ret, "Stop Tracking", 0, ret, MID_STOP_TRACK);
    }
    new FXMenuSeparator(ret);
    //
    buildShowParamsPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIVehicle::getParameterWindow(GUIMainWindow &app,
                               GUISUMOAbstractView &) throw()
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 9);
    // add items
    ret->mkItem("type [NAME]", false, myType->getID());
    ret->mkItem("left same route [#]", false, getRepetitionNo());
    ret->mkItem("emission period [s]", false, getPeriod());
    ret->mkItem("waiting time [s]", true,
                new CastingFunctionBinding<MSVehicle, SUMOReal, unsigned int>(this, &MSVehicle::getWaitingTime));
    ret->mkItem("last lane change [s]", true,
                new CastingFunctionBinding<GUIVehicle, SUMOReal, unsigned int>(this, &GUIVehicle::getLastLaneChangeOffset));
    ret->mkItem("desired depart [s]", false, getDesiredDepart());
    ret->mkItem("position [m]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getPositionOnLane));
    ret->mkItem("speed [m/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getSpeed));
    // close building
    ret->closeBuilding();
    return ret;
}


GUIGlObjectType
GUIVehicle::getType() const throw()
{
    return GLO_VEHICLE;
}


const std::string &
GUIVehicle::microsimID() const throw()
{
    return getID();
}


bool
GUIVehicle::active() const throw()
{
    return isOnRoad();
}


void
GUIVehicle::setRemoved()
{
    myLane = 0;
}


int
GUIVehicle::getRepetitionNo() const
{
    return myRepetitionNumber;
}


int
GUIVehicle::getPeriod() const
{
    return myPeriod;
}


unsigned int
GUIVehicle::getLastLaneChangeOffset() const
{
    return myLastLaneChangeOffset;
}


Boundary
GUIVehicle::getCenteringBoundary() const throw()
{
    Boundary b;
    b.add(getPosition());
    b.grow(20);
    return b;
}


const std::vector<MSVehicle::LaneQ> &
GUIVehicle::getBestLanes() const throw()
{
    myLock.lock();
    const std::vector<MSVehicle::LaneQ> &ret = MSVehicle::getBestLanes();
    myLock.unlock();
    return ret;
}


void
GUIVehicle::setBlinkerInformation()
{
    if (hasCORNIntValue(MSCORN::CORN_VEH_BLINKER)) {
        int blinker = 0;
        int state = getLaneChangeModel().getState();
        if ((state&LCA_LEFT)!=0) {
            blinker = 1;
        } else if ((state&LCA_RIGHT)!=0) {
            blinker = -1;
        } else {
            const MSLane &lane = getLane();
            MSLinkCont::const_iterator link = lane.succLinkSec(*this, 1, lane, getBestLanesContinuation());
            if (link!=lane.getLinkCont().end()&&lane.length()-getPositionOnLane()<lane.maxSpeed()*(SUMOReal) 7.) {
                switch ((*link)->getDirection()) {
                case MSLink::LINKDIR_TURN:
                case MSLink::LINKDIR_LEFT:
                case MSLink::LINKDIR_PARTLEFT:
                    blinker = 1;
                    break;
                case MSLink::LINKDIR_RIGHT:
                case MSLink::LINKDIR_PARTRIGHT:
                    blinker = -1;
                    break;
                default:
                    break;
                }
            }
        }
        myIntCORNMap[MSCORN::CORN_VEH_BLINKER] = blinker;
    }
}




/****************************************************************************/

