/****************************************************************************/
/// @file    GUIVehicle.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A MSVehicle extended by some values for usage within the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#ifdef HAVE_OSG
#include <osg/ShapeDrawable>
#include <osgview/GUIOSGView.h>
#endif
#include <foreign/polyfonts/polyfonts.h>
#include <utils/common/StringUtils.h>
#include <utils/common/SUMOVehicleParameter.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GLObjectValuePassConnector.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSAbstractLaneChangeModel.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/devices/MSDevice_Person.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include "GUIVehicle.h"
#include "GUIPerson.h"
#include "GUINet.h"
#include "GUIEdge.h"
#include "GUILane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIVehicle::GUIVehiclePopupMenu) GUIVehiclePopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_ALLROUTES, GUIVehicle::GUIVehiclePopupMenu::onCmdShowAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_ALLROUTES, GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_CURRENTROUTE, GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_CURRENTROUTE, GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_BEST_LANES, GUIVehicle::GUIVehiclePopupMenu::onCmdShowBestLanes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_BEST_LANES, GUIVehicle::GUIVehiclePopupMenu::onCmdHideBestLanes),
    FXMAPFUNC(SEL_COMMAND, MID_START_TRACK, GUIVehicle::GUIVehiclePopupMenu::onCmdStartTrack),
    FXMAPFUNC(SEL_COMMAND, MID_STOP_TRACK, GUIVehicle::GUIVehiclePopupMenu::onCmdStopTrack),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_LFLINKITEMS, GUIVehicle::GUIVehiclePopupMenu::onCmdShowLFLinkItems),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_LFLINKITEMS, GUIVehicle::GUIVehiclePopupMenu::onCmdHideLFLinkItems),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_FOES, GUIVehicle::GUIVehiclePopupMenu::onCmdShowFoes),
};

// Object implementation
FXIMPLEMENT(GUIVehicle::GUIVehiclePopupMenu, GUIGLObjectPopupMenu, GUIVehiclePopupMenuMap, ARRAYNUMBER(GUIVehiclePopupMenuMap))



// ===========================================================================
// data definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * drawed shapes
 * ----------------------------------------------------------------------- */
double vehiclePoly_PassengerCarBody[] = { .5, 0,  0, 0,  0, .3,  0.08, .44,  0.25, .5,  0.95, .5,  1., .4,  1., -.4,  0.95, -.5,  0.25, -.5,  0.08, -.44,  0, -.3,  0, 0,  -10000 };
double vehiclePoly_PassengerCarBodyFront[] = { 0.1, 0,  0.025, 0,  0.025, 0.25,  0.27, 0.4,  0.27, -.4,  0.025, -0.25,  0.025, 0,  -10000 };
double vehiclePoly_PassengerFrontGlass[] = { 0.35, 0,  0.3, 0,  0.3, 0.4,  0.43, 0.3,  0.43, -0.3,  0.3, -0.4,  0.3, 0,  -10000 };
double vehiclePoly_PassengerSedanRightGlass[] = { 0.36, -.43,  0.34, -.47,  0.77, -.47,  0.67, -.37,  0.45, -.37,  0.34, -.47,  -10000 };
double vehiclePoly_PassengerSedanLeftGlass[] = { 0.36, .43,  0.34, .47,  0.77, .47,  0.67, .37,  0.45, .37,  0.34, .47,  -10000 };
double vehiclePoly_PassengerSedanBackGlass[] = { 0.80, 0,  0.70, 0,  0.70, 0.3,  0.83, 0.4,  0.83, -.4,  0.70, -.3,  0.70, 0,  -10000 };
double vehiclePoly_PassengerHatchbackRightGlass[] = { 0.36, -.43,  0.34, -.47,  0.94, -.47,  0.80, -.37,  0.45, -.37,  0.34, -.47,  -10000 };
double vehiclePoly_PassengerHatchbackLeftGlass[] = { 0.36, .43,  0.34, .47,  0.94, .47,  0.80, .37,  0.45, .37,  0.34, .47,  -10000 };
double vehiclePoly_PassengerHatchbackBackGlass[] = { 0.92, 0,  0.80, 0,  0.80, 0.3,  0.95, 0.4,  0.95, -.4,  0.80, -.3,  0.80, 0,  -10000 };
double vehiclePoly_PassengerWagonRightGlass[] = { 0.36, -.43,  0.34, -.47,  0.94, -.47,  0.87, -.37,  0.45, -.37,  0.34, -.47,  -10000 };
double vehiclePoly_PassengerWagonLeftGlass[] = { 0.36, .43,  0.34, .47,  0.94, .47,  0.87, .37,  0.45, .37,  0.34, .47,  -10000 };
double vehiclePoly_PassengerWagonBackGlass[] = { 0.92, 0,  0.90, 0,  0.90, 0.3,  0.95, 0.4,  0.95, -.4,  0.90, -.3,  0.90, 0,  -10000 };

double vehiclePoly_PassengerVanBody[] = { .5, 0,  0, 0,  0, .4,  0.1, .5,  0.97, .5,  1., .47,  1., -.47,  0.97, -.5,  0.1, -.5,  0, -.4,  0, 0,  -10000 };
double vehiclePoly_PassengerVanBodyFront[] = { 0.1, 0,  0.025, 0,  0.025, 0.25,  0.13, 0.4,  0.13, -.4,  0.025, -0.25,  0.025, 0,  -10000 };
double vehiclePoly_PassengerVanFrontGlass[] = { 0.21, 0,  0.16, 0,  0.16, 0.4,  0.29, 0.3,  0.29, -0.3,  0.16, -0.4,  0.16, 0,  -10000 };
double vehiclePoly_PassengerVanRightGlass[] = { 0.36, -.43,  0.20, -.47,  0.98, -.47,  0.91, -.37,  0.31, -.37,  0.20, -.47,  -10000 };
double vehiclePoly_PassengerVanLeftGlass[] = { 0.36, .43,  0.20, .47,  0.98, .47,  0.91, .37,  0.31, .37,  0.20, .47,  -10000 };
double vehiclePoly_PassengerVanBackGlass[] = { 0.95, 0,  0.94, 0,  0.94, 0.3,  0.98, 0.4,  0.98, -.4,  0.94, -.3,  0.94, 0,  -10000 };

double vehiclePoly_DeliveryMediumRightGlass[] = { 0.21, -.43,  0.20, -.47,  0.38, -.47,  0.38, -.37,  0.31, -.37,  0.20, -.47,  -10000 };
double vehiclePoly_DeliveryMediumLeftGlass[] = { 0.21, .43,  0.20, .47,  0.38, .47,  0.38, .37,  0.31, .37,  0.20, .47,  -10000 };

double vehiclePoly_TransportBody[] = { .5, 0,  0, 0,  0, .45,  0.05, .5,  2.25, .5,  2.25, -.5,  0.05, -.5,  0, -.45,  0, 0,  -10000 };
double vehiclePoly_TransportFrontGlass[] = { 0.1, 0,  0.05, 0,  0.05, 0.45,  0.25, 0.4,  0.25, -.4,  0.05, -0.45,  0.05, 0,  -10000 };
double vehiclePoly_TransportRightGlass[] = { 0.36, -.47,  0.10, -.48,  1.25, -.48,  1.25, -.4,  0.3, -.4,  0.10, -.48,  -10000 };
double vehiclePoly_TransportLeftGlass[] = { 0.36, .47,  0.10, .48,  1.25, .48,  1.25, .4,  0.3, .4,  0.10, .48,  -10000 };

double vehiclePoly_EVehicleBody[] = { .5, 0,  0, 0,  0, .3,  0.08, .44,  0.25, .5,  0.75, .5,  .92, .44,  1, .3,  1, -.3,  .92, -.44,  .75, -.5,  .25, -.5,  0.08, -.44,  0, -.3,  0, 0,  -1000 };
double vehiclePoly_EVehicleFrontGlass[] = { .5, 0,  0.05, .05,  0.05, .25,  0.13, .39,  0.3, .45,  0.70, .45,  .87, .39,  .95, .25,  .95, -.25,  .87, -.39,  .70, -.45,  .3, -.45,  0.13, -.39,  0.05, -.25,  0.05, 0.05,  -1000 };
//double vehiclePoly_EVehicleFrontGlass[] = { 0.35,0,  0.1,0,  0.1,0.4,  0.43,0.3,  0.43,-0.3,  0.1,-0.4,  0.1,0,  -10000 };
double vehiclePoly_EVehicleBackGlass[] =  { 0.65, 0,  0.9, 0,  0.9, 0.4,  0.57, 0.3,  0.57, -0.3,  0.9, -0.4,  0.9, 0,  -10000 };


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIVehicle::GUIVehiclePopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIVehicle::GUIVehiclePopupMenu::GUIVehiclePopupMenu(
    GUIMainWindow& app, GUISUMOAbstractView& parent,
    GUIGlObject& o, std::map<GUISUMOAbstractView*, int>& additionalVisualizations)
    : GUIGLObjectPopupMenu(app, parent, o), myVehiclesAdditionalVisualizations(additionalVisualizations) {
}


GUIVehicle::GUIVehiclePopupMenu::~GUIVehiclePopupMenu() {}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowAllRoutes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_ALL_ROUTES)) {
        static_cast<GUIVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_ALL_ROUTES);
    }
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_ALL_ROUTES);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_ROUTE)) {
        static_cast<GUIVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_ROUTE);
    }
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_ROUTE);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowBestLanes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_BEST_LANES)) {
        static_cast<GUIVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_BEST_LANES);
    }
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideBestLanes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_BEST_LANES);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdStartTrack(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_TRACKED)) {
        myParent->startTrack(static_cast<GUIVehicle*>(myObject)->getGlID());
        static_cast<GUIVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_TRACKED);
    }
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdStopTrack(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_TRACKED);
    myParent->stopTrack();
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowLFLinkItems(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_LFLINKITEMS)) {
        static_cast<GUIVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_LFLINKITEMS);
    }
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideLFLinkItems(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_LFLINKITEMS);
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowFoes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIVehicle*>(myObject)->selectBlockingFoes();
    return 1;
}


/* -------------------------------------------------------------------------
 * GUIVehicle - methods
 * ----------------------------------------------------------------------- */
GUIVehicle::GUIVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
                       const MSVehicleType* type,
                       SUMOReal speedFactor) :
    MSVehicle(pars, route, type, speedFactor),
    GUIGlObject(GLO_VEHICLE, pars->id) {
    // as it is possible to show all vehicle routes, we have to store them... (bug [ 2519761 ])
    myRoutes = MSDevice_Vehroutes::buildVehicleDevices(*this, myDevices, 5);
    myMoveReminders.push_back(std::make_pair(myRoutes, 0.));
    mySeatPositions.push_back(Position(0, 0)); // ensure length 1
}


GUIVehicle::~GUIVehicle() {
    myLock.lock();
    for (std::map<GUISUMOAbstractView*, int>::iterator i = myAdditionalVisualizations.begin(); i != myAdditionalVisualizations.end(); ++i) {
        while (i->first->removeAdditionalGLVisualisation(this));
    }
#ifdef HAVE_OSG
    for (std::map<GUIOSGView*, osg::ShapeDrawable*>::iterator i = myGeom.begin(); i != myGeom.end(); ++i) {
        i->first->remove(this);
    }
#endif
    myLock.unlock();
    GLObjectValuePassConnector<SUMOReal>::removeObject(*this);
    delete myRoutes;
}


GUIGLObjectPopupMenu*
GUIVehicle::getPopUpMenu(GUIMainWindow& app,
                         GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIVehiclePopupMenu(app, parent, *this, myAdditionalVisualizations);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //
    if (hasActiveAddVisualisation(&parent, VO_SHOW_ROUTE)) {
        new FXMenuCommand(ret, "Hide Current Route", 0, ret, MID_HIDE_CURRENTROUTE);
    } else {
        new FXMenuCommand(ret, "Show Current Route", 0, ret, MID_SHOW_CURRENTROUTE);
    }
    if (hasActiveAddVisualisation(&parent, VO_SHOW_ALL_ROUTES)) {
        new FXMenuCommand(ret, "Hide All Routes", 0, ret, MID_HIDE_ALLROUTES);
    } else {
        new FXMenuCommand(ret, "Show All Routes", 0, ret, MID_SHOW_ALLROUTES);
    }
    if (hasActiveAddVisualisation(&parent, VO_SHOW_BEST_LANES)) {
        new FXMenuCommand(ret, "Hide Best Lanes", 0, ret, MID_HIDE_BEST_LANES);
    } else {
        new FXMenuCommand(ret, "Show Best Lanes", 0, ret, MID_SHOW_BEST_LANES);
    }
    if (hasActiveAddVisualisation(&parent, VO_SHOW_LFLINKITEMS)) {
        new FXMenuCommand(ret, "Hide Link Items", 0, ret, MID_HIDE_LFLINKITEMS);
    } else {
        new FXMenuCommand(ret, "Show Link Items", 0, ret, MID_SHOW_LFLINKITEMS);
    }
    new FXMenuSeparator(ret);
    int trackedID = parent.getTrackedID();
    if (trackedID < 0 || (size_t)trackedID != getGlID()) {
        new FXMenuCommand(ret, "Start Tracking", 0, ret, MID_START_TRACK);
    } else {
        new FXMenuCommand(ret, "Stop Tracking", 0, ret, MID_STOP_TRACK);
    }
    new FXMenuCommand(ret, "Select Foes", 0, ret, MID_SHOW_FOES);

    new FXMenuSeparator(ret);
    //
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUIVehicle::getParameterWindow(GUIMainWindow& app,
                               GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 34);
    // add items
    ret->mkItem("lane [id]", false, myLane->getID());
    ret->mkItem("position [m]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getPositionOnLane));
    ret->mkItem("speed [m/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getSpeed));
    ret->mkItem("angle [degree]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &MSVehicle::getAngle));
    if (getChosenSpeedFactor() != 1) {
        ret->mkItem("speed factor", false, getChosenSpeedFactor());
    }
    ret->mkItem("time gap [s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &MSVehicle::getTimeGap));
    ret->mkItem("waiting time [s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &MSVehicle::getWaitingSeconds));
    ret->mkItem("impatience", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &MSVehicle::getImpatience));
    ret->mkItem("last lane change [s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getLastLaneChangeOffset));
    ret->mkItem("desired depart [s]", false, time2string(getParameter().depart));
    if (getParameter().repetitionNumber > 0) {
        ret->mkItem("left same route [#]", false, (unsigned int) getParameter().repetitionNumber);
    }
    if (getParameter().repetitionOffset > 0) {
        ret->mkItem("insertion period [s]", false, time2string(getParameter().repetitionOffset));
    }
    ret->mkItem("stop info", false, getStopInfo());
    ret->mkItem("CO2 (HBEFA) [mg/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHBEFA_CO2Emissions));
    ret->mkItem("CO (HBEFA) [mg/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHBEFA_COEmissions));
    ret->mkItem("HC (HBEFA) [mg/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHBEFA_HCEmissions));
    ret->mkItem("NOx (HBEFA) [mg/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHBEFA_NOxEmissions));
    ret->mkItem("PMx (HBEFA) [mg/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHBEFA_PMxEmissions));
    ret->mkItem("fuel (HBEFA) [ml/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHBEFA_FuelConsumption));
    ret->mkItem("noise (Harmonoise) [dB]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHarmonoise_NoiseEmissions));
    ret->mkItem("parameters [key:val]", false, toString(getParameter().getMap()));
    ret->mkItem("", false, "");
    ret->mkItem("Type Information:", false, "");
    ret->mkItem("type [id]", false, myType->getID());
    ret->mkItem("length", false, myType->getLength());
    ret->mkItem("minGap", false, myType->getMinGap());
    ret->mkItem("vehicle class", false, SumoVehicleClassStrings.getString(myType->getVehicleClass()));
    ret->mkItem("emission class", false, SumoEmissionClassStrings.getString(myType->getEmissionClass()));
    ret->mkItem("maximum speed [m/s]", false, getMaxSpeed());
    ret->mkItem("maximum acceleration [m/s^2]", false, getCarFollowModel().getMaxAccel());
    ret->mkItem("maximum deceleration [m/s^2]", false, getCarFollowModel().getMaxDecel());
    ret->mkItem("imperfection (sigma)", false, getCarFollowModel().getImperfection());
    ret->mkItem("reaction time (tau)", false, getCarFollowModel().getHeadwayTime());
    ret->mkItem("type parameters [key:val]", false, toString(myType->getParameter().getMap()));
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GUIVehicle::getCenteringBoundary() const {
    Boundary b;
    b.add(getPosition());
    b.grow(20);
    return b;
}


void
GUIVehicle::drawAction_drawVehicleAsBoxPlus() const {
    glPushMatrix();
    glScaled(getVehicleType().getWidth(), getVehicleType().getLength(), 1.);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2d(0., 0.);
    glVertex2d(-.5, .15);
    glVertex2d(.5, .15);
    glVertex2d(-.5, 1.);
    glVertex2d(.5, 1.);
    glEnd();
    glPopMatrix();
}


void
GUIVehicle::drawAction_drawVehicleAsTrianglePlus() const {
    const SUMOReal length = getVehicleType().getLength();
    if (length >= 8.) {
        drawAction_drawVehicleAsBoxPlus();
        return;
    }
    glPushMatrix();
    glScaled(getVehicleType().getWidth(), length, 1.);
    glBegin(GL_TRIANGLES);
    glVertex2d(0., 0.);
    glVertex2d(-.5, 1.);
    glVertex2d(.5, 1.);
    glEnd();
    glPopMatrix();
}


void
GUIVehicle::drawPoly(double* poses, SUMOReal offset) {
    glPushMatrix();
    glTranslated(0, 0, offset * .1);
    glPolygonOffset(0, offset * -1);
    glBegin(GL_TRIANGLE_FAN);
    int i = 0;
    while (poses[i] > -999) {
        glVertex2d(poses[i], poses[i + 1]);
        i = i + 2;
    }
    glEnd();
    glPopMatrix();
}


void
GUIVehicle::drawAction_drawVehicleAsPoly(const GUIVisualizationSettings& s) const {
    RGBColor current = GLHelper::getColor();
    RGBColor lighter = current.changedBrightness(51);
    RGBColor darker = current.changedBrightness(-51);

    const SUMOReal length = getVehicleType().getLength();
    const SUMOReal width = getVehicleType().getWidth();
    glPushMatrix();
    glRotated(90, 0, 0, 1);
    glScaled(length, width, 1.);
    SUMOVehicleShape shape = getVehicleType().getGuiShape();

    // draw main body
    switch (shape) {
        case SVS_UNKNOWN:
            drawPoly(vehiclePoly_PassengerCarBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerCarBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerFrontGlass, 4.5);
            break;
        case SVS_PEDESTRIAN:
            glTranslated(0, 0, .045);
            GLHelper::drawFilledCircle(0.25);
            glTranslated(0, 0, -.045);
            glScaled(.7, 2, 1);
            glTranslated(0, 0, .04);
            GLHelper::setColor(lighter);
            GLHelper::drawFilledCircle(0.25);
            glTranslated(0, 0, -.04);
            break;
        case SVS_BICYCLE:
        case SVS_MOTORCYCLE: {
            glPushMatrix();
            glTranslated(.5, 0, 0);
            glScaled(.25 / (length), 1, 1.);
            glTranslated(0, 0, .045);
            GLHelper::drawFilledCircle(1);
            glScaled(.7, 2, 1);
            glTranslated(0, 0, -.045);
            glTranslated(0, 0, .04);
            GLHelper::setColor(lighter);
            GLHelper::drawFilledCircle(1);
            glTranslated(0, 0, -.04);
            glPopMatrix();
        }
        break;
        case SVS_PASSENGER:
        case SVS_PASSENGER_SEDAN:
        case SVS_PASSENGER_HATCHBACK:
        case SVS_PASSENGER_WAGON:
            drawPoly(vehiclePoly_PassengerCarBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerCarBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerFrontGlass, 4.5);
            break;
        case SVS_PASSENGER_VAN:
            drawPoly(vehiclePoly_PassengerVanBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerVanBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerVanFrontGlass, 4.5);
            drawPoly(vehiclePoly_PassengerVanRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerVanLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerVanBackGlass, 4.5);
            break;
        case SVS_DELIVERY:
            drawPoly(vehiclePoly_PassengerVanBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerVanBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerVanFrontGlass, 4.5);
            drawPoly(vehiclePoly_DeliveryMediumRightGlass, 4.5);
            drawPoly(vehiclePoly_DeliveryMediumLeftGlass, 4.5);
            break;
        case SVS_TRANSPORT:
        case SVS_TRANSPORT_SEMITRAILER:
        case SVS_TRANSPORT_1TRAILER:
            glScaled(1. / (length), 1, 1.);
            drawPoly(vehiclePoly_TransportBody, 4);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_TransportFrontGlass, 4.5);
            drawPoly(vehiclePoly_TransportRightGlass, 4.5);
            drawPoly(vehiclePoly_TransportLeftGlass, 4.5);
            break;
        case SVS_BUS:
        case SVS_BUS_TROLLEY:
        case SVS_BUS_CITY: {
            SUMOReal ml = length;
            glScaled(1. / (length), 1, 1.);
            glTranslated(0, 0, .04);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2d(ml / 2., 0);
            glVertex2d(0, 0);
            glVertex2d(0, -.45);
            glVertex2d(0 + .05, -.5);
            glVertex2d(ml - .05, -.5);
            glVertex2d(ml, -.45);
            glVertex2d(ml, .45);
            glVertex2d(ml - .05, .5);
            glVertex2d(0 + .05, .5);
            glVertex2d(0, .45);
            glVertex2d(0, 0);
            glEnd();
            glTranslated(0, 0, -.04);

            glTranslated(0, 0, .045);
            glColor3d(0, 0, 0);
            glBegin(GL_QUADS);
            glVertex2d(0 + .05, .48);
            glVertex2d(0 + .05, -.48);
            glVertex2d(0 + .15, -.48);
            glVertex2d(0 + .15, .48);

            glVertex2d(ml - .1, .45);
            glVertex2d(ml - .1, -.45);
            glVertex2d(ml - .05, -.45);
            glVertex2d(ml - .05, .45);

            glVertex2d(0 + .20, .49);
            glVertex2d(0 + .20, .45);
            glVertex2d(ml - .20, .45);
            glVertex2d(ml - .20, .49);

            glVertex2d(0 + .20, -.49);
            glVertex2d(0 + .20, -.45);
            glVertex2d(ml - .20, -.45);
            glVertex2d(ml - .20, -.49);

            glEnd();
            glTranslated(0, 0, -.045);
        }
        break;
        case SVS_BUS_CITY_FLEXIBLE:
        case SVS_BUS_OVERLAND:
        case SVS_RAIL:
        case SVS_RAIL_LIGHT:
        case SVS_RAIL_CITY:
        case SVS_RAIL_SLOW:
        case SVS_RAIL_FAST:
        case SVS_RAIL_CARGO:
            drawAction_drawCarriageClass(s, shape, false);
            break;
        case SVS_E_VEHICLE:
            drawPoly(vehiclePoly_EVehicleBody, 4);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_EVehicleFrontGlass, 4.5);
            glTranslated(0, 0, .048);
            GLHelper::setColor(current);
            glBegin(GL_QUADS);
            glVertex2d(.3, .5);
            glVertex2d(.35, .5);
            glVertex2d(.35, -.5);
            glVertex2d(.3, -.5);

            glVertex2d(.3, -.05);
            glVertex2d(.7, -.05);
            glVertex2d(.7, .05);
            glVertex2d(.3, .05);

            glVertex2d(.7, .5);
            glVertex2d(.65, .5);
            glVertex2d(.65, -.5);
            glVertex2d(.7, -.5);
            glEnd();
            glTranslated(0, 0, -.048);
            //drawPoly(vehiclePoly_EVehicleBackGlass, 4.5);
            break;
        case SVS_ANT:
            glPushMatrix();
            // ant is stretched via vehicle length
            GLHelper::setColor(darker);
            // draw left side
            GLHelper::drawBoxLine(Position(-0.2, -.10), 350, 0.5, .02);
            GLHelper::drawBoxLine(Position(-0.3, -.50), 240, 0.4, .03);
            GLHelper::drawBoxLine(Position(0.3, -.10), 340, 0.8, .03);
            GLHelper::drawBoxLine(Position(0.05, -.80), 290, 0.6, .04);
            GLHelper::drawBoxLine(Position(0.4, -.10),  20, 0.8, .03);
            GLHelper::drawBoxLine(Position(0.65, -.80),  75, 0.6, .04);
            GLHelper::drawBoxLine(Position(0.5, -.10),  55, 0.8, .04);
            GLHelper::drawBoxLine(Position(1.1, -.55),  90, 0.6, .04);
            // draw right side
            GLHelper::drawBoxLine(Position(-0.2,  .10), 190, 0.5, .02);
            GLHelper::drawBoxLine(Position(-0.3,  .50), 300, 0.4, .03);
            GLHelper::drawBoxLine(Position(0.3,  .10), 200, 0.8, .03);
            GLHelper::drawBoxLine(Position(0.05, .80), 250, 0.6, .04);
            GLHelper::drawBoxLine(Position(0.4,  .10), 160, 0.8, .03);
            GLHelper::drawBoxLine(Position(0.65, .80), 105, 0.6, .04);
            GLHelper::drawBoxLine(Position(0.5,  .10), 125, 0.8, .04);
            GLHelper::drawBoxLine(Position(1.1,  .55),  90, 0.6, .04);
            // draw body
            GLHelper::setColor(current);
            glTranslated(0, 0, 0.1);
            GLHelper::drawFilledCircle(.25, 16);
            glTranslated(.4, 0, 0);
            GLHelper::drawFilledCircle(.2, 16);
            glTranslated(.4, 0, 0);
            GLHelper::drawFilledCircle(.3, 16);
            glPopMatrix();
            break;
        default: // same as passenger
            drawPoly(vehiclePoly_PassengerCarBody, 4);
            glColor3d(1, 1, 1);
            drawPoly(vehiclePoly_PassengerCarBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerFrontGlass, 4.5);
            break;
    }

    // draw decorations
    switch (shape) {
        case SVS_PEDESTRIAN:
            break;
        case SVS_BICYCLE:
            //glScaled(length, 1, 1.);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2d(1 / 2., 0);
            glVertex2d(0, 0);
            glVertex2d(0, -.03);
            glVertex2d(0 + .05, -.05);
            glVertex2d(1 - .05, -.05);
            glVertex2d(1, -.03);
            glVertex2d(1, .03);
            glVertex2d(1 - .05, .05);
            glVertex2d(0 + .05, .05);
            glVertex2d(0, .03);
            glVertex2d(0, 0);
            glEnd();
            break;
        case SVS_MOTORCYCLE:
            //glScaled(length, 1, 1.);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2d(1 / 2., 0);
            glVertex2d(0, 0);
            glVertex2d(0, -.03);
            glVertex2d(0 + .05, -.2);
            glVertex2d(1 - .05, -.2);
            glVertex2d(1, -.03);
            glVertex2d(1, .03);
            glVertex2d(1 - .05, .2);
            glVertex2d(0 + .05, .2);
            glVertex2d(0, .03);
            glVertex2d(0, 0);
            glEnd();
            break;
        case SVS_PASSENGER:
        case SVS_PASSENGER_SEDAN:
            drawPoly(vehiclePoly_PassengerSedanRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerSedanLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerSedanBackGlass, 4.5);
            break;
        case SVS_PASSENGER_HATCHBACK:
            drawPoly(vehiclePoly_PassengerHatchbackRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerHatchbackLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerHatchbackBackGlass, 4.5);
            break;
        case SVS_PASSENGER_WAGON:
            drawPoly(vehiclePoly_PassengerWagonRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerWagonLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerWagonBackGlass, 4.5);
            break;
        case SVS_PASSENGER_VAN:
        case SVS_DELIVERY:
            break;
        case SVS_TRANSPORT:
            GLHelper::setColor(current);
            GLHelper::drawBoxLine(Position(2.3, 0), 90., length - 2.3, .5);
            break;
        case SVS_TRANSPORT_SEMITRAILER:
            GLHelper::setColor(current);
            GLHelper::drawBoxLine(Position(2.8, 0), 90., length - 2.8, .5);
            break;
        case SVS_TRANSPORT_1TRAILER: {
            GLHelper::setColor(current);
            SUMOReal l = length - 2.3;
            l = l / 2.;
            GLHelper::drawBoxLine(Position(2.3, 0), 90., l, .5);
            GLHelper::drawBoxLine(Position(2.3 + l + .5, 0), 90., l - .5, .5);
            break;
        }
        case SVS_BUS_TROLLEY:
            glPushMatrix();
            glTranslated(0, 0, .1);
            GLHelper::setColor(darker);
            GLHelper::drawBoxLine(Position(3.8, 0), 90., 1, .3);
            glTranslated(0, 0, .1);
            glColor3d(0, 0, 0);
            GLHelper::drawBoxLine(Position(4.3, .2), 90., 1, .06);
            GLHelper::drawBoxLine(Position(4.3, -.2), 90., 1, .06);
            GLHelper::drawBoxLine(Position(5.3, .2), 90., 3, .03);
            GLHelper::drawBoxLine(Position(5.3, -.2), 90., 3, .03);
            glPopMatrix();
            break;
        case SVS_BUS:
        case SVS_BUS_CITY:
        case SVS_BUS_CITY_FLEXIBLE:
        case SVS_BUS_OVERLAND:
        case SVS_RAIL:
        case SVS_RAIL_LIGHT:
        case SVS_RAIL_CITY:
        case SVS_RAIL_SLOW:
        case SVS_RAIL_FAST:
        case SVS_RAIL_CARGO:
        case SVS_E_VEHICLE:
        case SVS_ANT:
            break;
        default: // same as passenger/sedan
            drawPoly(vehiclePoly_PassengerSedanRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerSedanLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerSedanBackGlass, 4.5);
            break;
    }
    /*
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(.5,.5); // center - strip begin
    glVertex2d(0,    .5); // center, front
    glVertex2d(0,    .8); // ... clockwise ... (vehicle right side)
    glVertex2d(0.08, .94);
    glVertex2d(0.25, 1.);
    glVertex2d(0.95, 1.);
    glVertex2d(1.,   .9);
    glVertex2d(1.,   .1); // (vehicle left side)
    glVertex2d(0.95, 0.);
    glVertex2d(0.25, 0.);
    glVertex2d(0.08, .06);
    glVertex2d(0,    .2); //
    glVertex2d(0,    .5); // center, front (close)
    glEnd();

    glPolygonOffset(0, -4.5);
    glColor3d(1, 1, 1); // front
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(0.1,0.5);
    glVertex2d(0.025,0.5);
    glVertex2d(0.025,0.75);
    glVertex2d(0.27,0.9);
    glVertex2d(0.27,0.1);
    glVertex2d(0.025,0.25);
    glVertex2d(0.025,0.5);
    glEnd();

    glColor3d(0, 0, 0); // front glass
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(0.35,0.5);
    glVertex2d(0.3,0.5);
    glVertex2d(0.3,0.9);
    glVertex2d(0.43,0.8);
    glVertex2d(0.43,0.2);
    glVertex2d(0.3,0.1);
    glVertex2d(0.3,0.5);
    glEnd();

    glBegin(GL_TRIANGLE_FAN); // back glass
    glVertex2d(0.92,0.5);
    glVertex2d(0.90,0.5);
    glVertex2d(0.90,0.8);
    glVertex2d(0.95,0.9);
    glVertex2d(0.95,0.1);
    glVertex2d(0.90,0.2);
    glVertex2d(0.90,0.5);
    glEnd();

    glBegin(GL_TRIANGLE_FAN); // right glass
    glVertex2d(0.36,0.07);
    glVertex2d(0.34,0.03);
    glVertex2d(0.94,0.03);
    glVertex2d(0.87,0.13);
    glVertex2d(0.45,0.13);
    glVertex2d(0.34,0.03);
    glEnd();

    glBegin(GL_TRIANGLE_FAN); // left glass
    glVertex2d(0.36,1.-0.07);
    glVertex2d(0.34,1.-0.03);
    glVertex2d(0.94,1.-0.03);
    glVertex2d(0.87,1.-0.13);
    glVertex2d(0.45,1.-0.13);
    glVertex2d(0.34,1.-0.03);
    glEnd();
    */

    glPopMatrix();
}


bool
GUIVehicle::drawAction_drawVehicleAsImage(const GUIVisualizationSettings& s, SUMOReal length) const {
    const std::string& file = getVehicleType().getImgFile();
    if (file != "") {
        int textureID = GUITexturesHelper::getTextureID(file);
        if (textureID > 0) {
            if (length < 0) {
                length = getVehicleType().getLength() * s.vehicleExaggeration;
            }
            const SUMOReal halfWidth = getVehicleType().getWidth() / 2.0 * s.vehicleExaggeration;
            GUITexturesHelper::drawTexturedBox(textureID, -halfWidth, 0, halfWidth, length);
            return true;
        }
    }
    return false;
}


bool 
GUIVehicle::drawAction_drawCarriageClass(const GUIVisualizationSettings& s, SUMOVehicleShape guiShape, bool asImage) const {
    switch (guiShape) {
        case SVS_BUS_CITY_FLEXIBLE:
        case SVS_BUS_OVERLAND:
            drawAction_drawRailCarriages(s, 8.25, 0, 0, asImage); // 16.5 overall, 2 modules http://de.wikipedia.org/wiki/Ikarus_180
            break;
        case SVS_RAIL:
            drawAction_drawRailCarriages(s, 24.5, 1, 1, asImage); // http://de.wikipedia.org/wiki/UIC-Y-Wagen_%28DR%29
            break;
        case SVS_RAIL_LIGHT:
            drawAction_drawRailCarriages(s, 16.85, 1, 0, asImage); // 67.4m overall, 4 carriages http://de.wikipedia.org/wiki/DB-Baureihe_423
            break;
        case SVS_RAIL_CITY:
            drawAction_drawRailCarriages(s, 5.71, 0, 0, asImage); // 40.0m overall, 7 modules http://de.wikipedia.org/wiki/Bombardier_Flexity_Berlin
            break;
        case SVS_RAIL_SLOW:
            drawAction_drawRailCarriages(s, 9.44, 1, 1, asImage); // actually length of the locomotive http://de.wikipedia.org/wiki/KJI_Nr._20_und_21
            break;
        case SVS_RAIL_FAST:
            drawAction_drawRailCarriages(s, 24.775, 0, 0, asImage); // http://de.wikipedia.org/wiki/ICE_3
            break;
        case SVS_RAIL_CARGO:
            drawAction_drawRailCarriages(s, 13.86, 1, 0, asImage); // UIC 571-1 http://de.wikipedia.org/wiki/Flachwagen
            break;
        default:
            return false;
    }
    return true;
}

#define BLINKER_POS_FRONT .5
#define BLINKER_POS_BACK .5

inline void
drawAction_drawBlinker(double dir, SUMOReal length) {
    glColor3d(1.f, .8f, 0);
    glPushMatrix();
    glTranslated(dir, BLINKER_POS_FRONT, -0.1);
    GLHelper::drawFilledCircle(.5, 6);
    glPopMatrix();
    glPushMatrix();
    glTranslated(dir, length - BLINKER_POS_BACK, -0.1);
    GLHelper::drawFilledCircle(.5, 6);
    glPopMatrix();
}


inline void
drawAction_drawVehicleBlinker(const GUIVehicle& veh, SUMOReal length) {
    if (!veh.signalSet(MSVehicle::VEH_SIGNAL_BLINKER_RIGHT | MSVehicle::VEH_SIGNAL_BLINKER_LEFT | MSVehicle::VEH_SIGNAL_BLINKER_EMERGENCY)) {
        return;
    }
    const double offset = MAX2(.5 * veh.getVehicleType().getWidth(), .4);
    if (veh.signalSet(MSVehicle::VEH_SIGNAL_BLINKER_RIGHT)) {
        drawAction_drawBlinker(-offset, length);
    }
    if (veh.signalSet(MSVehicle::VEH_SIGNAL_BLINKER_LEFT)) {
        drawAction_drawBlinker(offset, length);;
    }
    if (veh.signalSet(MSVehicle::VEH_SIGNAL_BLINKER_EMERGENCY)) {
        drawAction_drawBlinker(-offset, length);
        drawAction_drawBlinker(offset, length);
    }
}


inline void
drawAction_drawVehicleBrakeLight(const GUIVehicle& veh, SUMOReal length) {
    if (!veh.signalSet(MSVehicle::VEH_SIGNAL_BRAKELIGHT)) {
        return;
    }
    glColor3f(1.f, .2f, 0);
    glPushMatrix();
    glTranslated(-veh.getVehicleType().getWidth() * 0.5, length, -0.1);
    GLHelper::drawFilledCircle(.5, 6);
    glPopMatrix();
    glPushMatrix();
    glTranslated(veh.getVehicleType().getWidth() * 0.5, length, -0.1);
    GLHelper::drawFilledCircle(.5, 6);
    glPopMatrix();
}


void
GUIVehicle::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    glPushMatrix();
    Position p1 = getPosition();
    // one seat in the center of the vehicle by default
    mySeatPositions[0] = myLane->geometryPositionAtOffset(myState.pos() - getVehicleType().getLength() / 2);
    glTranslated(p1.x(), p1.y(), getType());
    glRotated(getAngle(), 0, 0, 1);
    // set lane color
    setColor(s);
    // scale
    SUMOReal upscale = s.vehicleExaggeration;
    glScaled(upscale, upscale, 1);
    /*
        MSLCM_DK2004 &m2 = static_cast<MSLCM_DK2004&>(veh->getLaneChangeModel());
        if((m2.getState()&LCA_URGENT)!=0) {
            glColor3d(1, .4, .4);
        } else if((m2.getState()&LCA_SPEEDGAIN)!=0) {
            glColor3d(.4, .4, 1);
        } else {
            glColor3d(.4, 1, .4);
        }
        */
    // draw the vehicle
    myCarriageLength = getVehicleType().getLength();
    switch (s.vehicleQuality) {
        case 0:
            drawAction_drawVehicleAsTrianglePlus();
            break;
        case 1:
            drawAction_drawVehicleAsBoxPlus();
            break;
        case 2:
            drawAction_drawVehicleAsPoly(s);
            break;
        case 3:
        default:
            // draw as image but take special care for drawing trains
            // fallback to simple shapes
            if (!drawAction_drawCarriageClass(s, getVehicleType().getGuiShape(), true)) {
                if (!drawAction_drawVehicleAsImage(s)) {
                    drawAction_drawVehicleAsPoly(s);
                };
            }
            break;
    }
    if (s.drawMinGap) {
        SUMOReal minGap = -getVehicleType().getMinGap();
        glColor3d(0., 1., 0.);
        glBegin(GL_LINES);
        glVertex2d(0., 0);
        glVertex2d(0., minGap);
        glVertex2d(-.5, minGap);
        glVertex2d(.5, minGap);
        glEnd();
    }
    // draw the blinker and brakelights if wished
    if (s.showBlinker) {
        glTranslated(0, 0, .1);
        switch (getVehicleType().getGuiShape()) {
            case SVS_PEDESTRIAN:
            case SVS_BICYCLE:
            case SVS_ANT:
            case SVS_RAIL:
            case SVS_RAIL_LIGHT:
            case SVS_RAIL_SLOW:
            case SVS_RAIL_FAST:
            case SVS_RAIL_CARGO:
                // only SVS_RAIL_CITY has blinkers and brake lights
                break;
            default:
                drawAction_drawVehicleBlinker(*this, myCarriageLength);
                drawAction_drawVehicleBrakeLight(*this, myCarriageLength);
                break;
        }
    }
    // draw the wish to change the lane
    if (s.drawLaneChangePreference) {
        /*
                if(gSelected.isSelected(GLO_VEHICLE, veh->getGlID())) {
                MSLCM_DK2004 &m = static_cast<MSLCM_DK2004&>(veh->getLaneChangeModel());
                glColor3d(.5, .5, 1);
                glBegin(GL_LINES);
                glVertex2f(0, 0);
                glVertex2f(m.getChangeProbability(), .5);
                glEnd();

                glColor3d(1, 0, 0);
                glBegin(GL_LINES);
                glVertex2f(0.1, 0);
                glVertex2f(0.1, m.myMaxJam1);
                glEnd();

                glColor3d(0, 1, 0);
                glBegin(GL_LINES);
                glVertex2f(-0.1, 0);
                glVertex2f(-0.1, m.myTDist);
                glEnd();
                }
                */
    }
    // draw best lanes
    /*
    if (true) {
        const MSLane &l = veh->getLane();
        SUMOReal r1 = veh->allowedContinuationsLength(&l, 0);
        SUMOReal r2 = l.getLeftLane()!=0 ? veh->allowedContinuationsLength(l.getLeftLane(), 0) : 0;
        SUMOReal r3 = l.getRightLane()!=0 ? veh->allowedContinuationsLength(l.getRightLane(), 0) : 0;
        SUMOReal mmax = MAX3(r1, r2, r3);
        glBegin(GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(0, r1/mmax/2.);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(.4, 0);
        glVertex2f(.4, r2/mmax/2.);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(-.4, 0);
        glVertex2f(-.4, r3/mmax/2.);
        glEnd();
    }
    */
    glPopMatrix();
    drawName(getPosition(-MIN2(getVehicleType().getLength() / 2, SUMOReal(5))),
             s.scale,
             getVehicleType().getGuiShape() == SVS_PEDESTRIAN ? s.personName : s.vehicleName);
    glPopName();
    if (myPersonDevice != 0) {
        const std::vector<MSPerson*>& ps = myPersonDevice->getPersons();
        size_t personIndex = 0;
        for (std::vector<MSPerson*>::const_iterator i = ps.begin(); i != ps.end(); ++i) {
            GUIPerson* person = dynamic_cast<GUIPerson*>(*i);
            assert(person != 0);
            person->setPositionInVehicle(getSeatPosition(personIndex++));
            person->drawGL(s);
        }
    }
}


void
GUIVehicle::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    glPushMatrix();
    glTranslated(0, 0, getType() - .1); // don't draw on top of other cars
    if (hasActiveAddVisualisation(parent, VO_SHOW_BEST_LANES)) {
        drawBestLanes();
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_ROUTE)) {
        drawRoute(s, 0, 0.25);
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_ALL_ROUTES)) {
        if (getNumberReroutes() > 0) {
            const int noReroutePlus1 = getNumberReroutes() + 1;
            for (int i = noReroutePlus1 - 1; i >= 0; i--) {
                SUMOReal darken = SUMOReal(0.4) / SUMOReal(noReroutePlus1) * SUMOReal(i);
                drawRoute(s, i, darken);
            }
        } else {
            drawRoute(s, 0, 0.25);
        }
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_LFLINKITEMS)) {
        for (DriveItemVector::const_iterator i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
            if ((*i).myLink == 0) {
                continue;
            }
            MSLink* link = (*i).myLink;
            MSLane* via = link->getViaLaneOrLane();
            if (via != 0) {
                Position p = via->getShape()[0];
                if ((*i).mySetRequest) {
                    glColor3d(0, .8, 0);
                } else {
                    glColor3d(.8, 0, 0);
                }
                const SUMOTime leaveTime = (*i).myLink->getLeaveTime(
                                               (*i).myArrivalTime, (*i).myArrivalSpeed, (*i).getLeaveSpeed(), getVehicleType().getLengthWithGap());
                drawLinkItem(p, (*i).myArrivalTime, leaveTime, s.addExaggeration);
                // the time slot that ego vehicle uses when checking opened may
                // differ from the one it requests in setApproaching
                MSLink::ApproachingVehicleInformation avi = (*i).myLink->getApproaching(this);
                assert(avi.arrivalTime == (*i).myArrivalTime && avi.leavingTime == leaveTime);
            }
        }
    }
    glPopMatrix();
    glPopName();
}


void
GUIVehicle::drawLinkItem(const Position& pos, SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal exagerate) {
    glTranslated(pos.x(), pos.y(), -.1);
    GLHelper::drawFilledCircle(1);
    std::string times = toString(STEPS2TIME(arrivalTime)) + "/" + toString(STEPS2TIME(leaveTime));
    GLHelper::drawText(times.c_str(), Position(), .1, 1.6 * exagerate, RGBColor::GREEN, 0);
    glTranslated(-pos.x(), -pos.y(), .1);
}

const std::vector<MSVehicle::LaneQ>&
GUIVehicle::getBestLanes() const {
    myLock.lock();
    const std::vector<MSVehicle::LaneQ>& ret = MSVehicle::getBestLanes();
    myLock.unlock();
    return ret;
}


void
GUIVehicle::setColor(const GUIVisualizationSettings& s) const {
    const GUIColorer& c = s.vehicleColorer;
    if (!setFunctionalColor(c.getActive())) {
        GLHelper::setColor(c.getScheme().getColor(getColorValue(c.getActive())));
    }
}


bool
GUIVehicle::setFunctionalColor(size_t activeScheme) const {
    switch (activeScheme) {
        case 0: {
            if (getParameter().wasSet(VEHPARS_COLOR_SET)) {
                GLHelper::setColor(getParameter().color);
                return true;
            }
            if (getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                GLHelper::setColor(getVehicleType().getColor());
                return true;
            }
            if (&getRoute().getColor() != &RGBColor::DEFAULT_COLOR) {
                GLHelper::setColor(getRoute().getColor());
                return true;
            }
            return false;
        }
        case 2: {
            if (getParameter().wasSet(VEHPARS_COLOR_SET)) {
                GLHelper::setColor(getParameter().color);
                return true;
            }
            return false;
        }
        case 3: {
            if (getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                GLHelper::setColor(getVehicleType().getColor());
                return true;
            }
            return false;
        }
        case 4: {
            if (&getRoute().getColor() != &RGBColor::DEFAULT_COLOR) {
                GLHelper::setColor(getRoute().getColor());
                return true;
            }
            return false;
        }
        case 5: {
            Position p = getRoute().getEdges()[0]->getLanes()[0]->getShape()[0];
            const Boundary& b = ((GUINet*) MSNet::getInstance())->getBoundary();
            Position center = b.getCenter();
            SUMOReal hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / PI;
            SUMOReal sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            return true;
        }
        case 6: {
            Position p = getRoute().getEdges().back()->getLanes()[0]->getShape()[-1];
            const Boundary& b = ((GUINet*) MSNet::getInstance())->getBoundary();
            Position center = b.getCenter();
            SUMOReal hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / PI;
            SUMOReal sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            return true;
        }
        case 7: {
            Position pb = getRoute().getEdges()[0]->getLanes()[0]->getShape()[0];
            Position pe = getRoute().getEdges().back()->getLanes()[0]->getShape()[-1];
            const Boundary& b = ((GUINet*) MSNet::getInstance())->getBoundary();
            SUMOReal hue = 180. + atan2(pb.x() - pe.x(), pb.y() - pe.y()) * 180. / PI;
            Position minp(b.xmin(), b.ymin());
            Position maxp(b.xmax(), b.ymax());
            SUMOReal sat = pb.distanceTo(pe) / minp.distanceTo(maxp);
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            return true;
        }
    }
    return false;
}


SUMOReal
GUIVehicle::getColorValue(size_t activeScheme) const {
    switch (activeScheme) {
        case 8:
            return getSpeed();
        case 9:
            return getWaitingSeconds();
        case 10:
            return getLastLaneChangeOffset();
        case 11:
            return getMaxSpeed();
        case 12:
            return getHBEFA_CO2Emissions();
        case 13:
            return getHBEFA_COEmissions();
        case 14:
            return getHBEFA_PMxEmissions();
        case 15:
            return getHBEFA_NOxEmissions();
        case 16:
            return getHBEFA_HCEmissions();
        case 17:
            return getHBEFA_FuelConsumption();
        case 18:
            return getHarmonoise_NoiseEmissions();
        case 19:
            if (getNumberReroutes() == 0) {
                return -1;
            }
            return getNumberReroutes();
        case 20:
            return gSelected.isSelected(GLO_VEHICLE, getGlID());
        case 21:
            return getBestLaneOffset();
        case 22:
            return getAcceleration();
        case 23:
            return getTimeGap();
    }
    return 0;
}


// ------------ Additional visualisations
bool
GUIVehicle::hasActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) const {
    return myAdditionalVisualizations.find(parent) != myAdditionalVisualizations.end() && (myAdditionalVisualizations.find(parent)->second & which) != 0;
}


void
GUIVehicle::addActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) {
    if (myAdditionalVisualizations.find(parent) == myAdditionalVisualizations.end()) {
        myAdditionalVisualizations[parent] = 0;
    }
    myAdditionalVisualizations[parent] |= which;
    parent->addAdditionalGLVisualisation(this);
}


void
GUIVehicle::removeActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) {
    myAdditionalVisualizations[parent] &= ~which;
    parent->removeAdditionalGLVisualisation(this);
}


void
GUIVehicle::drawRoute(const GUIVisualizationSettings& s, int routeNo, SUMOReal darken) const {
    setColor(s);
    GLdouble colors[4];
    glGetDoublev(GL_CURRENT_COLOR, colors);
    colors[0] -= darken;
    if (colors[0] < 0) {
        colors[0] = 0;
    }
    colors[1] -= darken;
    if (colors[1] < 0) {
        colors[1] = 0;
    }
    colors[2] -= darken;
    if (colors[2] < 0) {
        colors[2] = 0;
    }
    colors[3] -= darken;
    if (colors[3] < 0) {
        colors[3] = 0;
    }
    glColor3dv(colors);
    if (routeNo == 0) {
        drawRouteHelper(*myRoute, s.vehicleExaggeration);
        return;
    }
    --routeNo; // only prior routes are stored
    drawRouteHelper(*myRoutes->getRoute(routeNo), s.vehicleExaggeration);
}


void
GUIVehicle::drawBestLanes() const {
    myLock.lock();
    std::vector<std::vector<MSVehicle::LaneQ> > bestLanes = myBestLanes;
    myLock.unlock();
    SUMOReal width = 0.5;
    for (std::vector<std::vector<MSVehicle::LaneQ> >::iterator j = bestLanes.begin(); j != bestLanes.end(); ++j) {
        std::vector<MSVehicle::LaneQ>& lanes = *j;
        SUMOReal gmax = -1;
        SUMOReal rmax = -1;
        for (std::vector<MSVehicle::LaneQ>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
            gmax = MAX2((*i).length, gmax);
            rmax = MAX2((*i).occupation, rmax);
        }
        for (std::vector<MSVehicle::LaneQ>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
            const PositionVector& shape = (*i).lane->getShape();
            SUMOReal g = (*i).length / gmax;
            SUMOReal r = (*i).occupation / rmax;
            glColor3d(r, g, 0);
            GLHelper::drawBoxLines(shape, width);

            PositionVector s1 = shape;
            s1.move2side((SUMOReal) .1);
            glColor3d(r, 0, 0);
            GLHelper::drawLine(s1);
            s1.move2side((SUMOReal) - .2);
            glColor3d(0, g, 0);
            GLHelper::drawLine(s1);

            glColor3d(r, g, 0);
            Position lastPos = shape[-1];
        }
        width = .2;
    }
}


void
GUIVehicle::drawRouteHelper(const MSRoute& r, SUMOReal exaggeration) const {
    MSRouteIterator i = r.begin();
    const std::vector<MSLane*>& bestLaneConts = getBestLanesContinuation();
    // draw continuation lanes when drawing the current route where available
    size_t bestLaneIndex = (&r == myRoute ? 0 : bestLaneConts.size());
    for (; i != r.end(); ++i) {
        const GUILane* lane;
        if (bestLaneIndex < bestLaneConts.size() && bestLaneConts[bestLaneIndex] != 0 && (*i) == &(bestLaneConts[bestLaneIndex]->getEdge())) {
            lane = static_cast<GUILane*>(bestLaneConts[bestLaneIndex]);
            ++bestLaneIndex;
        } else {
            lane = static_cast<GUILane*>((*i)->getLanes()[0]);
        }
        GLHelper::drawBoxLines(lane->getShape(), lane->getShapeRotations(), lane->getShapeLengths(), exaggeration);
    }
}


MSLane*
GUIVehicle::getPreviousLane(MSLane* current, int& furtherIndex) const {
    if (furtherIndex < (int)myFurtherLanes.size()) {
        return myFurtherLanes[furtherIndex++];
    } else {
        return current;
    }
}


void
GUIVehicle::drawAction_drawRailCarriages(const GUIVisualizationSettings& s, SUMOReal defaultLength, SUMOReal carriageGap, int firstPassengerCarriage, bool asImage) const {
    RGBColor current = GLHelper::getColor();
    RGBColor darker = current.changedBrightness(-51);
    const SUMOReal length = getVehicleType().getLength() * s.vehicleExaggeration;
    const SUMOReal halfWidth = getVehicleType().getWidth() / 2.0 * s.vehicleExaggeration;
    glPopMatrix(); // undo scaling and 90 degree rotation
    glPopMatrix(); // undo initial translation and rotation
    GLHelper::setColor(darker);
    const SUMOReal xCornerCut = 0.3;
    const SUMOReal yCornerCut = 0.4;
    // round to closest integer
    const int numCarriages = floor(length / (defaultLength + carriageGap) + 0.5);
    assert(numCarriages > 0);
    const SUMOReal carriageLengthWithGap = length / numCarriages;
    const SUMOReal carriageLength = carriageLengthWithGap - carriageGap;
    // lane on which the carriage front is situated
    MSLane* lane = myLane;
    int furtherIndex = 0;
    // lane on which the carriage back is situated
    MSLane* backLane = myLane;
    int backFurtherIndex = furtherIndex;
    // offsets of front and back
    SUMOReal carriageOffset = myState.pos();
    SUMOReal carriageBackOffset = myState.pos() - carriageLength;
    // handle seats
    int requiredSeats = getNumPassengers();
    if (requiredSeats > 0) {
        mySeatPositions.clear();
    }
    Position front, back;
    SUMOReal angle;
    // draw individual carriages
    for (int i = 0; i < numCarriages; ++i) {
        while (carriageOffset < 0) {
            lane = getPreviousLane(lane, furtherIndex);
            carriageOffset += lane->getLength();
        }
        while (carriageBackOffset < 0) {
            backLane = getPreviousLane(backLane, backFurtherIndex);
            carriageBackOffset += backLane->getLength();
        }
        front = lane->getShape().positionAtOffset2D(carriageOffset);
        back = backLane->getShape().positionAtOffset2D(carriageBackOffset);
        angle = atan2((front.x() - back.x()), (back.y() - front.y())) * (SUMOReal) 180.0 / (SUMOReal) PI;
        if (i >= firstPassengerCarriage) {
            computeSeats(front, back, requiredSeats);
        }
        glPushMatrix();
        glTranslated(front.x(), front.y(), getType());
        glRotated(angle, 0, 0, 1);
        if (!asImage || !drawAction_drawVehicleAsImage(s, carriageLength)) {
            glBegin(GL_TRIANGLE_FAN);
            glVertex2d(-halfWidth + xCornerCut, 0);
            glVertex2d(-halfWidth, yCornerCut);
            glVertex2d(-halfWidth, carriageLength - yCornerCut);
            glVertex2d(-halfWidth + xCornerCut, carriageLength);
            glVertex2d(halfWidth - xCornerCut, carriageLength);
            glVertex2d(halfWidth, carriageLength - yCornerCut);
            glVertex2d(halfWidth, yCornerCut);
            glVertex2d(halfWidth - xCornerCut, 0);
            glEnd();
        }
        glPopMatrix();
        carriageOffset -= carriageLengthWithGap;
        carriageBackOffset -= carriageLengthWithGap;
        GLHelper::setColor(current);
    }
    myCarriageLength = front.distanceTo2D(back);
    // restore matrices
    glPushMatrix();
    glTranslated(front.x(), front.y(), getType());
    glRotated(angle, 0, 0, 1);
    glPushMatrix();
}


const Position&
GUIVehicle::getSeatPosition(size_t personIndex) const {
    /// if there are not enough seats in the vehicle people have to squeeze onto the last seat
    return mySeatPositions[(int)MIN2(personIndex, mySeatPositions.size() - 1)];
}


int
GUIVehicle::getNumPassengers() const {
    if (myPersonDevice != 0) {
        return (int)myPersonDevice->getPersons().size();
    }
    return 0;
}


void
GUIVehicle::computeSeats(const Position& front, const Position& back, int& requiredSeats) const {
    if (requiredSeats <= 0) {
        return; // save some work
    }
    const Line l(front, back);
    const SUMOReal length = l.length2D();
    if (length < 4) {
        // small vehicle, sit at the center
        mySeatPositions.push_back(l.getPositionAtDistance2D(length / 2));
        requiredSeats--;
    } else {
        for (SUMOReal p = 2; p <= length - 1; p += 1) {
            mySeatPositions.push_back(l.getPositionAtDistance2D(p));
            requiredSeats--;
        }
    }
}


SUMOReal
GUIVehicle::getLastLaneChangeOffset() const {
    return STEPS2TIME(getLaneChangeModel().getLastLaneChangeOffset());
}


std::string
GUIVehicle::getStopInfo() const {
    std::string result = "";
    if (isParking()) {
        result += "parking";
    } else if (isStopped()) {
        result += "stopped";
    } else {
        return "";
    }
    if (myStops.front().triggered) {
        result += ", triggered";
    } else {
        result += ", duration=" + time2string(myStops.front().duration);
    }
    return result;
}


void
GUIVehicle::selectBlockingFoes() const {
    if (myLFLinkLanes.size() == 0) {
        return;
    }
    const DriveProcessItem& dpi = myLFLinkLanes[0];
    if (dpi.myLink == 0) {
        return;
    }
    std::vector<const SUMOVehicle*> blockingFoes;
    dpi.myLink->opened(dpi.myArrivalTime, dpi.myArrivalSpeed, dpi.getLeaveSpeed(), getVehicleType().getLengthWithGap(),
                       getImpatience(), getCarFollowModel().getMaxDecel(), getWaitingTime(), &blockingFoes);
    for (std::vector<const SUMOVehicle*>::const_iterator it = blockingFoes.begin(); it != blockingFoes.end(); ++it) {
        gSelected.select(static_cast<const GUIVehicle*>(*it)->getGlID());
    }
#ifdef HAVE_INTERNAL_LANES
    const MSLink::LinkLeaders linkLeaders = (dpi.myLink)->getLeaderInfo(myLane->getLength() - getPositionOnLane(), getVehicleType().getMinGap());
    for (MSLink::LinkLeaders::const_iterator it = linkLeaders.begin(); it != linkLeaders.end(); ++it) {
        // the vehicle to enter the junction first has priority
        const MSVehicle* leader = it->first;
        if ((static_cast<const GUIVehicle*>(leader))->myLinkLeaders.count(getID()) == 0) {
            // leader isn't already following us, now we follow it
            gSelected.select(static_cast<const GUIVehicle*>(leader)->getGlID());
        }
    }
#endif
}


#ifdef HAVE_OSG
void
GUIVehicle::updateColor(GUIOSGView* view) {
    const GUIVisualizationSettings* s = view->getVisualisationSettings();
    const RGBColor& col = s->vehicleColorer.getScheme().getColor(getColorValue(s->vehicleColorer.getActive()));
    myGeom[view]->setColor(osg::Vec4(col.red() / 255., col.green() / 255., col.blue() / 255., col.alpha() / 255.));
}
#endif

/****************************************************************************/

