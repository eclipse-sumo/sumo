/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIBaseVehicle.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker-Walz
/// @date    Sept 2002
///
// A MSVehicle extended by some values for usage within the gui
/****************************************************************************/
#include <config.h>

#include <cmath>
#include <vector>
#include <string>
#include <functional>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/geom/GeomHelper.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIBaseVehicleHelper.h>
#include <mesosim/MEVehicle.h>
#include <mesosim/MELoop.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSStop.h>
#include <microsim/MSTrainHelper.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/devices/MSDevice_Transportable.h>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <microsim/trigger/MSStoppingPlaceRerouter.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GUIBaseVehicle.h"
#include "GUIChargingStation.h"
#include "GUIPerson.h"
#include "GUIContainer.h"
#include "GUINet.h"
#include "GUIEdge.h"
#include "GUILane.h"
#include "GUIParkingArea.h"

//#define DRAW_BOUNDING_BOX

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIBaseVehicle::GUIBaseVehiclePopupMenu) GUIBaseVehiclePopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_ALLROUTES, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_ALLROUTES, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_CURRENTROUTE, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_CURRENTROUTE, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_FUTUREROUTE, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowFutureRoute),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_FUTUREROUTE, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideFutureRoute),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_ROUTE_NOLOOPS, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowRouteNoLoops),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_ROUTE_NOLOOPS, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideRouteNoLoops),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_BEST_LANES, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowBestLanes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_BEST_LANES, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideBestLanes),
    FXMAPFUNC(SEL_COMMAND, MID_START_TRACK, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdStartTrack),
    FXMAPFUNC(SEL_COMMAND, MID_STOP_TRACK, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdStopTrack),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_LFLINKITEMS, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowLFLinkItems),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_LFLINKITEMS, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideLFLinkItems),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_FOES, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowFoes),
    FXMAPFUNC(SEL_COMMAND, MID_SELECT_TRANSPORTED, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdSelectTransported),
    FXMAPFUNC(SEL_COMMAND, MID_REMOVE_OBJECT, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdRemoveObject),
    FXMAPFUNC(SEL_COMMAND, MID_TOGGLE_STOP, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdToggleStop),
};

// Object implementation
FXIMPLEMENT(GUIBaseVehicle::GUIBaseVehiclePopupMenu, GUIGLObjectPopupMenu, GUIBaseVehiclePopupMenuMap, ARRAYNUMBER(GUIBaseVehiclePopupMenuMap))

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIBaseVehicle::GUIBaseVehiclePopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIBaseVehicle::GUIBaseVehiclePopupMenu::GUIBaseVehiclePopupMenu(
    GUIMainWindow& app, GUISUMOAbstractView& parent, GUIGlObject* o) :
    GUIGLObjectPopupMenu(app, parent, o) {
}


GUIBaseVehicle::GUIBaseVehiclePopupMenu::~GUIBaseVehiclePopupMenu() {}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowAllRoutes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIBaseVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_ALL_ROUTES)) {
        static_cast<GUIBaseVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_ALL_ROUTES);
    }
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideAllRoutes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_ALL_ROUTES);
    return 1;
}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowCurrentRoute(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIBaseVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_ROUTE)) {
        static_cast<GUIBaseVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_ROUTE);
    }
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideCurrentRoute(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_ROUTE);
    return 1;
}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowFutureRoute(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIBaseVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_FUTURE_ROUTE)) {
        static_cast<GUIBaseVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_FUTURE_ROUTE);
    }
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideFutureRoute(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_FUTURE_ROUTE);
    return 1;
}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowRouteNoLoops(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIBaseVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_ROUTE_NOLOOP)) {
        static_cast<GUIBaseVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_ROUTE_NOLOOP);
    }
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideRouteNoLoops(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_ROUTE_NOLOOP);
    return 1;
}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowBestLanes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIBaseVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_BEST_LANES)) {
        static_cast<GUIBaseVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_BEST_LANES);
    }
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideBestLanes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_BEST_LANES);
    return 1;
}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdStartTrack(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (myParent->getTrackedID() != static_cast<GUIBaseVehicle*>(myObject)->getGlID()) {
        myParent->startTrack(static_cast<GUIBaseVehicle*>(myObject)->getGlID());
    }
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdStopTrack(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    myParent->stopTrack();
    return 1;
}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowLFLinkItems(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIBaseVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_LFLINKITEMS)) {
        static_cast<GUIBaseVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_LFLINKITEMS);
    }
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideLFLinkItems(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_LFLINKITEMS);
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowFoes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->selectBlockingFoes();
    myParent->update();
    return 1;
}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdSelectTransported(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    const MSBaseVehicle& veh = static_cast<GUIBaseVehicle*>(myObject)->getVehicle();
    for (const MSTransportable* t : veh.getPersons()) {
        gSelected.select((static_cast<const GUIPerson*>(t))->getGlID());
    }
    for (MSTransportable* t : veh.getContainers()) {
        gSelected.select((static_cast<const GUIContainer*>(t))->getGlID());
    }
    myParent->update();
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdRemoveObject(FXObject*, FXSelector, void*) {
    GUIBaseVehicle* baseVeh = static_cast<GUIBaseVehicle*>(myObject);
    MSVehicle* microVeh = dynamic_cast<MSVehicle*>(&baseVeh->myVehicle);
    if (microVeh != nullptr) {
        MSLane* lane = microVeh->getMutableLane();
        if (lane != nullptr) {
            lane->getVehiclesSecure();
            lane->removeVehicle(microVeh, MSMoveReminder::NOTIFICATION_VAPORIZED_GUI);
        }
        microVeh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_VAPORIZED_GUI);
        if (lane != nullptr) {
            lane->releaseVehicles();
        }
    } else {
        MEVehicle* mesoVeh = dynamic_cast<MEVehicle*>(&baseVeh->myVehicle);
        MSGlobals::gMesoNet->vaporizeCar(mesoVeh, MSMoveReminder::NOTIFICATION_VAPORIZED_GUI);
    }
    MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(&baseVeh->myVehicle);
    myParent->destroyPopup();
    myParent->update();
    return 1;
}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdToggleStop(FXObject*, FXSelector, void*) {
    GUIBaseVehicle* baseVeh = static_cast<GUIBaseVehicle*>(myObject);
    MSVehicle* microVeh = dynamic_cast<MSVehicle*>(&baseVeh->myVehicle);
    if (microVeh != nullptr) {
        if (microVeh->isStopped()) {
            microVeh->resumeFromStopping();
        } else {
            std::string errorOut;
            const double brakeGap = microVeh->getCarFollowModel().brakeGap(microVeh->getSpeed());
            std::pair<const MSLane*, double> stopPos = microVeh->getLanePosAfterDist(brakeGap);
            if (stopPos.first != nullptr) {
                SUMOVehicleParameter::Stop stop;
                stop.lane = stopPos.first->getID();
                stop.startPos = stopPos.second;
                stop.endPos = stopPos.second + POSITION_EPS;
                stop.duration = TIME2STEPS(3600);
                microVeh->addTraciStop(stop, errorOut);
                if (errorOut != "") {
                    WRITE_WARNING(errorOut);
                }
            }
        }
    } else {
        WRITE_WARNING(TL("GUI-triggered stop not implemented for meso"));
    }
    myParent->update();
    return 1;
}


/* -------------------------------------------------------------------------
 * GUIBaseVehicle - methods
 * ----------------------------------------------------------------------- */

GUIBaseVehicle::GUIBaseVehicle(MSBaseVehicle& vehicle) :
    GUIGlObject(GLO_VEHICLE, vehicle.getID(), GUIIconSubSys::getIcon(GUIIcon::VEHICLE)),
    myVehicle(vehicle),
    myPopup(nullptr) {
    // as it is possible to show all vehicle routes, we have to store them... (bug [ 2519761 ])
    myRoutes = MSDevice_Vehroutes::buildVehicleDevices(myVehicle, myVehicle.myDevices, 5);
    myVehicle.myMoveReminders.push_back(std::make_pair(myRoutes, 0.));
    mySeatPositions.push_back(Seat()); // ensure length 1
    myContainerPositions.push_back(Seat()); // ensure length 1
}


GUIBaseVehicle::~GUIBaseVehicle() {
    myLock.lock();
    for (std::map<GUISUMOAbstractView*, int>::iterator i = myAdditionalVisualizations.begin(); i != myAdditionalVisualizations.end(); ++i) {
        if (i->first->getTrackedID() == getGlID()) {
            i->first->stopTrack();
        }
        while (i->first->removeAdditionalGLVisualisation(this));
    }
    myLock.unlock();
    delete myRoutes;
    if (myPopup != nullptr) {
        myPopup->getParentView()->destroyPopup();
    }
}


GUIGLObjectPopupMenu*
GUIBaseVehicle::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIBaseVehiclePopupMenu(app, parent, this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //
    if (hasActiveAddVisualisation(&parent, VO_SHOW_ROUTE)) {
        GUIDesigns::buildFXMenuCommand(ret, TL("Hide Current Route"), nullptr, ret, MID_HIDE_CURRENTROUTE);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, TL("Show Current Route"), nullptr, ret, MID_SHOW_CURRENTROUTE);
    }
    if (hasActiveAddVisualisation(&parent, VO_SHOW_FUTURE_ROUTE)) {
        GUIDesigns::buildFXMenuCommand(ret, TL("Hide Future Route"), nullptr, ret, MID_HIDE_FUTUREROUTE);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, TL("Show Future Route"), nullptr, ret, MID_SHOW_FUTUREROUTE);
    }
    if (hasActiveAddVisualisation(&parent, VO_SHOW_ALL_ROUTES)) {
        GUIDesigns::buildFXMenuCommand(ret, TL("Hide All Routes"), nullptr, ret, MID_HIDE_ALLROUTES);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, TL("Show All Routes"), nullptr, ret, MID_SHOW_ALLROUTES);
    }
    if (hasActiveAddVisualisation(&parent, VO_SHOW_ROUTE_NOLOOP)) {
        FXMenuCheck* showLoops = new FXMenuCheck(ret, TL("Draw looped routes"), ret, MID_HIDE_ROUTE_NOLOOPS);
        showLoops->setCheck(false);
    } else {
        FXMenuCheck* showLoops = new FXMenuCheck(ret, TL("Draw looped routes"), ret, MID_SHOW_ROUTE_NOLOOPS);
        showLoops->setCheck(true);
    }
    if (hasActiveAddVisualisation(&parent, VO_SHOW_BEST_LANES)) {
        GUIDesigns::buildFXMenuCommand(ret, TL("Hide Best Lanes"), nullptr, ret, MID_HIDE_BEST_LANES);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, TL("Show Best Lanes"), nullptr, ret, MID_SHOW_BEST_LANES);
    }
    if (hasActiveAddVisualisation(&parent, VO_SHOW_LFLINKITEMS)) {
        GUIDesigns::buildFXMenuCommand(ret, TL("Hide Link Items"), nullptr, ret, MID_HIDE_LFLINKITEMS);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, TL("Show Link Items"), nullptr, ret, MID_SHOW_LFLINKITEMS);
    }
    new FXMenuSeparator(ret);
    if (parent.getTrackedID() != getGlID()) {
        GUIDesigns::buildFXMenuCommand(ret, TL("Start Tracking"), nullptr, ret, MID_START_TRACK);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, TL("Stop Tracking"), nullptr, ret, MID_STOP_TRACK);
    }
    GUIDesigns::buildFXMenuCommand(ret, TL("Select Foes"), nullptr, ret, MID_SHOW_FOES);
    if (myVehicle.getPersons().size() + myVehicle.getContainers().size() > 0) {
        GUIDesigns::buildFXMenuCommand(ret, TL("Select transported"), nullptr, ret, MID_SELECT_TRANSPORTED);
    }
    GUIDesigns::buildFXMenuCommand(ret, myVehicle.isStopped() ? TL("Abort stop") : TL("Stop"), nullptr, ret, MID_TOGGLE_STOP);
    GUIDesigns::buildFXMenuCommand(ret, TL("Remove"), nullptr, ret, MID_REMOVE_OBJECT);

    new FXMenuSeparator(ret);
    //
    buildShowParamsPopupEntry(ret, false);
    buildShowTypeParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, app);
    myPopup = ret;
    return ret;
}


void
GUIBaseVehicle::removedPopupMenu() {
    myPopup = nullptr;
}


double
GUIBaseVehicle::getExaggeration(const GUIVisualizationSettings& s) const {
    return (s.vehicleSize.getExaggeration(s, this) *
            s.vehicleScaler.getScheme().getColor(getScaleValue(s, s.vehicleScaler.getActive())));
}


Boundary
GUIBaseVehicle::getCenteringBoundary() const {
    Boundary b;
    b.add(getVisualPosition(GUIGlobals::gSecondaryShape));
    b.grow(myVehicle.getVehicleType().getLength());
    return b;
}


const std::string
GUIBaseVehicle::getOptionalName() const {
    return myVehicle.getParameter().getParameter("name", "");
}


void
GUIBaseVehicle::drawOnPos(const GUIVisualizationSettings& s, const Position& pos, const double angle) const {
    GLHelper::pushName(getGlID());
    GLHelper::pushMatrix();
    Position p1 = pos;
    const double degAngle = RAD2DEG(angle + M_PI / 2.);
    const double length = getVType().getLength();
    if (s.trueZ) {
        glTranslated(p1.x(), p1.y(), p1.z() + 1);
    } else {
        glTranslated(p1.x(), p1.y(), getType());
    }
    glRotated(degAngle, 0, 0, 1);
    RGBColor col = setColor(s);
    // scale
    const double upscale = getExaggeration(s);
    const bool s2 = s.secondaryShape;

    if (upscale > 1 && s.laneWidthExaggeration > 1 && myVehicle.isOnRoad()) {
        // optionally shift according to edge exaggeration
        double offsetFromLeftBorder = myVehicle.getCurrentEdge()->getWidth() - myVehicle.getRightSideOnEdge() - myVehicle.getVehicleType().getWidth() / 2;
        glTranslated((s.laneWidthExaggeration - 1) * -offsetFromLeftBorder / 2, 0, 0);
    }

    double upscaleLength = MSTrainHelper::getUpscaleLength(upscale, length, getVType().getWidth(), s.vehicleQuality);
    glScaled(upscale, upscaleLength, 1);
    /*
        MSLaneChangeModel::DK2004 &m2 = static_cast<MSLaneChangeModel::DK2004&>(veh->getLaneChangeModel());
        if((m2.getState()&LCA_URGENT)!=0) {
            glColor3d(1, .4, .4);
        } else if((m2.getState()&LCA_SPEEDGAIN)!=0) {
            glColor3d(.4, .4, 1);
        } else {
            glColor3d(.4, 1, .4);
        }
        */
    // draw the vehicle
    bool drawCarriages = false;
    // do not upscale vehicles on physically impossible geometry factors > 1
    const double geometryFactor = (s.scaleLength ?
                                   MIN2(1.0, (myVehicle.getLane() != nullptr
                                     ? myVehicle.getLane()->getLengthGeometryFactor(s2)
                                     : (myVehicle.getEdge()->getLanes().size() > 0 ? myVehicle.getEdge()->getLanes()[0]->getLengthGeometryFactor(s2) : 1)))
                                   : 1);
    double scaledLength = length * geometryFactor;
    if (col.alpha() != 0) {
        switch (s.vehicleQuality) {
            case 0:
                GUIBaseVehicleHelper::drawAction_drawVehicleAsTrianglePlus(getVType().getWidth(), scaledLength, drawReversed(s));
                break;
            case 1:
                GUIBaseVehicleHelper::drawAction_drawVehicleAsBoxPlus(getVType().getWidth(), scaledLength, drawReversed(s));
                break;
            case 2:
                drawCarriages = drawAction_drawVehicleAsPolyWithCarriagges(s, scaledLength);
                // draw flashing blue light for emergency vehicles
                if (getVType().getGuiShape() == SUMOVehicleShape::EMERGENCY) {
                    glTranslated(0, 0, .1);
                    drawAction_drawVehicleBlueLight();
                }
                break;
            case 3:
                drawCarriages = drawAction_drawVehicleAsPolyWithCarriagges(s, scaledLength, true);
                break;
            case 4: {
                // do not scale circle radius by lengthGeometryFactor nor length and reduce the effect of width
                const double w = 1.8 * sqrt(getVType().getWidth() / 1.8);
                GUIBaseVehicleHelper::drawAction_drawVehicleAsCircle(w, s.scale * upscale);
                // display text at circle center
                scaledLength = 0;
                break;
            }
            default:
                break;
        }
        if (s.drawMinGap) {
            const double minGap = -getVType().getMinGap();
            glColor3d(0., 1., 0.);
            glBegin(GL_LINES);
            glVertex2d(0., 0);
            glVertex2d(0., minGap);
            glVertex2d(-.5, minGap);
            glVertex2d(.5, minGap);
            glEnd();
        }
        if (s.drawBrakeGap && !MSGlobals::gUseMesoSim
                && (!s.vehicleSize.constantSizeSelected || myVehicle.isSelected())) {
            const double brakeGap = -static_cast<MSVehicle&>(myVehicle).getCarFollowModel().brakeGap(myVehicle.getSpeed());
            glColor3d(1., 0., 0.);
            glBegin(GL_LINES);
            glVertex2d(0., 0);
            glVertex2d(0., brakeGap);
            glVertex2d(-.5, brakeGap);
            glVertex2d(.5, brakeGap);
            glEnd();
        }
        if (s.showBTRange) {
            MSVehicleDevice_BTreceiver* dev = static_cast<MSVehicleDevice_BTreceiver*>(myVehicle.getDevice(typeid(MSVehicleDevice_BTreceiver)));
            if (dev != nullptr) {
                glColor3d(1., 0., 0.);
                GLHelper::drawOutlineCircle(dev->getRange(), dev->getRange() - .2, 32);
            }
        }
        // draw the blinker and brakelights if wished
        if (s.showBlinker) {
            glTranslated(0, 0, .1);
            switch (getVType().getGuiShape()) {
                case SUMOVehicleShape::PEDESTRIAN:
                case SUMOVehicleShape::BICYCLE:
                case SUMOVehicleShape::SCOOTER:
                case SUMOVehicleShape::ANT:
                case SUMOVehicleShape::SHIP:
                case SUMOVehicleShape::RAIL:
                case SUMOVehicleShape::RAIL_CARGO:
                case SUMOVehicleShape::RAIL_CAR:
                case SUMOVehicleShape::AIRCRAFT:
                    break;
                case SUMOVehicleShape::MOTORCYCLE:
                case SUMOVehicleShape::MOPED:
                    drawAction_drawVehicleBlinker(scaledLength);
                    drawAction_drawVehicleBrakeLight(scaledLength, true);
                    break;
                default:
                    // only SUMOVehicleShape::RAIL_CAR has blinkers and brake lights but they are drawn along with the carriages
                    if (!drawCarriages) {
                        drawAction_drawVehicleBlinker(scaledLength);
                        drawAction_drawVehicleBrakeLight(scaledLength);
                    }
                    break;
            }
        }
        // draw the wish to change the lane
        if (s.drawLaneChangePreference) {
            /*
               if(gSelected.isSelected(GLO_VEHICLE, veh->getGlID())) {
               MSLaneChangeModel::DK2004 &m = static_cast<MSLaneChangeModel::DK2004&>(veh->getLaneChangeModel());
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
    }
    glTranslated(0, MIN2(scaledLength / 2, double(5)), -getType()); // drawing name at GLO_MAX fails unless translating z
    glScaled(1 / upscale, 1 / upscaleLength, 1);
    glRotated(-degAngle, 0, 0, 1);
    drawName(Position(0, 0), s.scale, s.vehicleName, s.angle);
    if (s.vehicleName.show(this) && myVehicle.getParameter().line != "") {
        glRotated(-s.angle, 0, 0, 1);
        glTranslated(0, 0.7 * s.vehicleName.scaledSize(s.scale), 0);
        glRotated(s.angle, 0, 0, 1);
        GLHelper::drawTextSettings(s.vehicleName, "line:" + myVehicle.getParameter().line, Position(0, 0), s.scale, s.angle);
    }
    if (s.vehicleValue.show(this)) {
        glRotated(-s.angle, 0, 0, 1);
        glTranslated(0, 0.7 * s.vehicleName.scaledSize(s.scale), 0);
        glRotated(s.angle, 0, 0, 1);
        const double value = getColorValue(s, s.vehicleColorer.getActive());
        if (value != s.MISSING_DATA) {
            GLHelper::drawTextSettings(s.vehicleValue, toString(value), Position(0, 0), s.scale, s.angle);
        }
    }
    if (s.vehicleScaleValue.show(this)) {
        glRotated(-s.angle, 0, 0, 1);
        glTranslated(0, 0.7 * s.vehicleName.scaledSize(s.scale), 0);
        glRotated(s.angle, 0, 0, 1);
        const double value = getScaleValue(s, s.vehicleScaler.getActive());
        if (value != s.MISSING_DATA) {
            GLHelper::drawTextSettings(s.vehicleScaleValue, toString(value), Position(0, 0), s.scale, s.angle);
        }
    }
    if (s.vehicleText.show(this)) {
        std::string error;
        std::string value = myVehicle.getPrefixedParameter(s.vehicleTextParam, error);
        if (value != "") {
            auto lines = StringTokenizer(value, StringTokenizer::NEWLINE).getVector();
            glRotated(-s.angle, 0, 0, 1);
            glTranslated(0, 0.7 * s.vehicleText.scaledSize(s.scale) * (double)lines.size(), 0);
            glRotated(s.angle, 0, 0, 1);
            for (std::string& line : lines) {
                GLHelper::drawTextSettings(s.vehicleText, line, Position(0, 0), s.scale, s.angle);
                glRotated(-s.angle, 0, 0, 1);
                glTranslated(0, -0.7 * s.vehicleText.scaledSize(s.scale), 0);
                glRotated(s.angle, 0, 0, 1);
            }
        }
    }
    if (s.showParkingInfo && myAdditionalVisualizations.size() != 0 && hasActiveAddVisualisation(
                myAdditionalVisualizations.begin()->first, VO_SHOW_ROUTE | VO_SHOW_FUTURE_ROUTE | VO_SHOW_ALL_ROUTES)) {
        glRotated(-s.angle, 0, 0, 1);
        glTranslated(0, 0.7 * s.vehicleName.scaledSize(s.scale), 0);
        glRotated(s.angle, 0, 0, 1);
        const double value = myVehicle.getNumberParkingReroutes();
        GLHelper::drawTextSettings(s.vehicleName, toString(value), Position(0, 0), s.scale, s.angle);
    }

    if (!drawCarriages) {
        mySeatPositions.clear();
        myContainerPositions.clear();
        int requiredSeats = getNumPassengers();
        int requiredContainerPositions = getNumContainers();
        const Position back = (p1 + Position(-scaledLength * upscaleLength, 0)).rotateAround2D(angle, p1);
        double extraOffset = scaledLength * 0.15;
        computeSeats(p1, back, SUMO_const_waitingPersonWidth, getVType().getPersonCapacity(), upscale, requiredSeats, mySeatPositions, extraOffset);
        computeSeats(p1, back, SUMO_const_waitingContainerWidth, getVType().getContainerCapacity(), upscale, requiredContainerPositions, myContainerPositions, extraOffset);
    }

    GLHelper::popMatrix();
    GLHelper::popName();
    drawAction_drawPersonsAndContainers(s);
}


void
GUIBaseVehicle::drawGL(const GUIVisualizationSettings& s) const {
    drawOnPos(s, getVisualPosition(s.secondaryShape), getVisualAngle(s.secondaryShape));
}


void
GUIBaseVehicle::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    if (!myVehicle.isOnRoad()) {
        drawGL(s);
    }
    GLHelper::pushName(getGlID());
    GLHelper::pushMatrix();
    glTranslated(0, 0, getType() - .1); // don't draw on top of other cars
    if (hasActiveAddVisualisation(parent, VO_SHOW_BEST_LANES)) {
        drawBestLanes();
    }
    bool noLoop = hasActiveAddVisualisation(parent, VO_SHOW_ROUTE_NOLOOP);
    if (hasActiveAddVisualisation(parent, VO_SHOW_ROUTE)) {
        drawRoute(s, 0, 0.25, false, noLoop);
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_FUTURE_ROUTE)) {
        drawRoute(s, 0, 0.25, true, noLoop);
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_ALL_ROUTES)) {
        if (myVehicle.getNumberReroutes() > 0) {
            const int noReroutePlus1 = myVehicle.getNumberReroutes() + 1;
            for (int i = noReroutePlus1 - 1; i >= 0; i--) {
                double darken = double(0.4) / double(noReroutePlus1) * double(i);
                drawRoute(s, i, darken);
            }
        } else {
            drawRoute(s, 0, 0.25, false, noLoop);
        }
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_LFLINKITEMS)) {
        drawAction_drawLinkItems(s);
    }
    GLHelper::popMatrix();
    GLHelper::popName();
}


void
GUIBaseVehicle::drawLinkItem(const Position& pos, SUMOTime arrivalTime, SUMOTime leaveTime, double exagerate) {
    glTranslated(pos.x(), pos.y(), -.1);
    GLHelper::drawFilledCircle(1);
    std::string times = toString(STEPS2TIME(arrivalTime)) + "/" + toString(STEPS2TIME(leaveTime));
    GLHelper::drawText(times.c_str(), Position(), .1, 1.6 * exagerate, RGBColor::GREEN, 0);
    glTranslated(-pos.x(), -pos.y(), .1);
}


RGBColor
GUIBaseVehicle::setColor(const GUIVisualizationSettings& s) const {
    RGBColor col;
    const GUIColorer& c = s.vehicleColorer;
    if (!setFunctionalColor(c.getActive(), &myVehicle, col)) {
        col = c.getScheme().getColor(getColorValue(s, c.getActive()));
    }
    GLHelper::setColor(col);
    return col;
}


bool
GUIBaseVehicle::setFunctionalColor(int activeScheme, const MSBaseVehicle* veh, RGBColor& col) {
    switch (activeScheme) {
        case 0: {
            //test for emergency vehicle
            if (veh->getVehicleType().getGuiShape() == SUMOVehicleShape::EMERGENCY) {
                col = RGBColor::WHITE;
                return true;
            }
            //test for firebrigade
            if (veh->getVehicleType().getGuiShape() == SUMOVehicleShape::FIREBRIGADE) {
                col = RGBColor::RED;
                return true;
            }
            //test for police car
            if (veh->getVehicleType().getGuiShape() == SUMOVehicleShape::POLICE) {
                col = RGBColor::BLUE;
                return true;
            }
            if (veh->getParameter().wasSet(VEHPARS_COLOR_SET)) {
                col = veh->getParameter().color;
                return true;
            }
            if (veh->getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                col = veh->getVehicleType().getColor();
                return true;
            }
            if (&(veh->getRoute().getColor()) != &RGBColor::DEFAULT_COLOR) {
                col = veh->getRoute().getColor();
                return true;
            }
            return false;
        }
        case 2: {
            if (veh->getParameter().wasSet(VEHPARS_COLOR_SET)) {
                col = veh->getParameter().color;
                return true;
            }
            return false;
        }
        case 3: {
            if (veh->getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                col = veh->getVehicleType().getColor();
                return true;
            }
            return false;
        }
        case 4: {
            if (&(veh->getRoute().getColor()) != &RGBColor::DEFAULT_COLOR) {
                col = veh->getRoute().getColor();
                return true;
            }
            return false;
        }
        case 5: {
            Position p = veh->getRoute().getEdges()[0]->getLanes()[0]->getShape()[0];
            const Boundary& b = ((GUINet*) MSNet::getInstance())->getBoundary();
            Position center = b.getCenter();
            double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
            double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            col = RGBColor::fromHSV(hue, sat, 1.);
            return true;
        }
        case 6: {
            Position p = veh->getRoute().getEdges().back()->getLanes()[0]->getShape()[-1];
            const Boundary& b = ((GUINet*) MSNet::getInstance())->getBoundary();
            Position center = b.getCenter();
            double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
            double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            col = RGBColor::fromHSV(hue, sat, 1.);
            return true;
        }
        case 7: {
            Position pb = veh->getRoute().getEdges()[0]->getLanes()[0]->getShape()[0];
            Position pe = veh->getRoute().getEdges().back()->getLanes()[0]->getShape()[-1];
            const Boundary& b = ((GUINet*) MSNet::getInstance())->getBoundary();
            double hue = 180. + atan2(pb.x() - pe.x(), pb.y() - pe.y()) * 180. / M_PI;
            Position minp(b.xmin(), b.ymin());
            Position maxp(b.xmax(), b.ymax());
            double sat = pb.distanceTo(pe) / minp.distanceTo(maxp);
            col = RGBColor::fromHSV(hue, sat, 1.);
            return true;
        }
        case 35: { // color randomly (by pointer hash)
            std::hash<const MSBaseVehicle*> ptr_hash;
            const double hue = (double)(ptr_hash(veh) % 360); // [0-360]
            const double sat = (double)((ptr_hash(veh) / 360) % 67) / 100.0 + 0.33; // [0.33-1]
            col = RGBColor::fromHSV(hue, sat, 1.);
            return true;
        }
        case 36: { // color by angle
            double hue = GeomHelper::naviDegree(veh->getAngle());
            col = RGBColor::fromHSV(hue, 1., 1.);
            return true;
        }
    }
    return false;
}


double
GUIBaseVehicle::getScaleValue(const GUIVisualizationSettings& s, int activeScheme) const {
    switch (activeScheme) {
        case 0: // uniform
            return 0;
        case 1: // selection
            return myVehicle.isSelected();
        case 2: // by speed
            if (myVehicle.isStopped()) {
                return myVehicle.isParking() ? -2 : -1;
            }
            return myVehicle.getSpeed();
        case 3:
            return myVehicle.getWaitingSeconds();
        case 4: {
            MSVehicle* microVeh = dynamic_cast<MSVehicle*>(&myVehicle);
            return (microVeh != nullptr ? microVeh->getAccumulatedWaitingSeconds() : 0);
        }
        case 5: {
            MSVehicle* microVeh = dynamic_cast<MSVehicle*>(&myVehicle);
            return (microVeh != nullptr ? microVeh->getLane()->getVehicleMaxSpeed(microVeh) : myVehicle.getEdge()->getVehicleMaxSpeed(&myVehicle));
        }
        case 6:
            return myVehicle.getNumberReroutes();
        case 7: {
            MSVehicle* microVeh = dynamic_cast<MSVehicle*>(&myVehicle);
            return (microVeh != nullptr
                    ? (microVeh->getLaneChangeModel().isOpposite() ? -100 : microVeh->getBestLaneOffset())
                    : 0);
        }
        case 8:
            return myVehicle.getAcceleration();
        case 9: {
            MSVehicle* microVeh = dynamic_cast<MSVehicle*>(&myVehicle);
            return (microVeh != nullptr ? microVeh->getTimeGapOnLane() : 0);
        }
        case 10:
            return STEPS2TIME(myVehicle.getDepartDelay());
        case 11:
            return myVehicle.getTimeLossSeconds();
        case 12:
            return myVehicle.getStopDelay();
        case 13:
            return myVehicle.getStopArrivalDelay();
        case 14: // by numerical param value
            std::string error;
            std::string val = myVehicle.getPrefixedParameter(s.vehicleScaleParam, error);
            try {
                if (val == "") {
                    return 0;
                } else {
                    return StringUtils::toDouble(val);
                }
            } catch (NumberFormatException&) {
                try {
                    return StringUtils::toBool(val);
                } catch (BoolFormatException&) {
                    WRITE_WARNINGF(TL("Vehicle parameter '%' key '%' is not a number for vehicle '%'."),
                                   myVehicle.getParameter().getParameter(s.vehicleScaleParam, "0"), s.vehicleScaleParam, myVehicle.getID());
                    return -1;
                }
            }
    }
    return 0;
}


// ------------ Additional visualisations
bool
GUIBaseVehicle::hasActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) const {
    return myAdditionalVisualizations.find(parent) != myAdditionalVisualizations.end() && (myAdditionalVisualizations.find(parent)->second & which) != 0;
}


void
GUIBaseVehicle::addActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) {
    if (myAdditionalVisualizations.find(parent) == myAdditionalVisualizations.end()) {
        myAdditionalVisualizations[parent] = 0;
    }
    myAdditionalVisualizations[parent] |= which;
    if (which != VO_TRACK) {
        parent->addAdditionalGLVisualisation(this);
    }
}


void
GUIBaseVehicle::removeActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) {
    myAdditionalVisualizations[parent] &= ~which;
    if (myAdditionalVisualizations[parent] == 0) {
        myAdditionalVisualizations.erase(parent);
    }
    parent->removeAdditionalGLVisualisation(this);
}


void
GUIBaseVehicle::drawRoute(const GUIVisualizationSettings& s, int routeNo, double darken, bool future, bool noLoop) const {
    RGBColor vehColor = setColor(s);
    RGBColor darker = vehColor.changedBrightness((int)(darken * -255));
    if (darker == RGBColor::BLACK) {
        darker = vehColor.multiply(1 - darken);
    }
    GLHelper::setColor(darker);
    if (routeNo == 0) {
        drawRouteHelper(s, myVehicle.getRoutePtr(), future, noLoop, darker);
        return;
    }
    ConstMSRoutePtr route = myRoutes->getRoute(routeNo - 1); // only prior routes are stored
    if (route != nullptr) {
        drawRouteHelper(s, route, future, noLoop, darker);
    }
}


void
GUIBaseVehicle::drawStopLabels(const GUIVisualizationSettings& s, bool noLoop, const RGBColor& col) const {
    // (vertical shift for repeated stops at the same position
    std::map<const MSLane*, int> repeat; // count repeated occurrences of the same position
    int stopIndex = 0;
    for (const MSStop& stop : myVehicle.getStops()) {
        double stopLanePos;
        if (stop.getSpeed() > 0) {
            stopLanePos = stop.reached ? stop.pars.endPos : stop.pars.startPos;
        } else {
            stopLanePos = stop.reached ? myVehicle.getPositionOnLane() : MAX2(0.0, stop.getEndPos(myVehicle));
        }
        if (stop.isOpposite && !stop.reached) {
            stopLanePos = stop.lane->getLength() - stopLanePos;
        }
        Position pos = stop.lane->geometryPositionAtOffset(stopLanePos);
        GLHelper::setColor(col);
        GLHelper::drawBoxLines(stop.lane->getShape().getOrthogonal(pos, 10, true, stop.lane->getWidth()), 0.1);
        std::string label = (stop.getSpeed() > 0
                             ? (stop.reached ? "passing waypoint" : "waypoint ")
                             : (stop.reached ? "stopped" : "stop "));
        if (!stop.reached) {
            label += toString(stopIndex);
        }

        if (stop.isOpposite) {
            label += " (opposite)";
        }
#ifdef _DEBUG
        label += " (" + toString(stop.edge - myVehicle.getCurrentRouteEdge()) + "e)";
#endif
        if (myVehicle.isStoppedTriggered()) {
            label += " triggered:";
            if (stop.triggered) {
                label += "person";
                if (stop.numExpectedPerson > 0) {
                    label += "(" + toString(stop.numExpectedPerson) + ")";
                }
            }
            if (stop.containerTriggered) {
                label += "container";
                if (stop.numExpectedContainer > 0) {
                    label += "(" + toString(stop.numExpectedContainer) + ")";
                }
            }
            if (stop.joinTriggered) {
                label += "join";
                if (stop.pars.join != "") {
                    label += "(" + stop.pars.join + ")";
                }
            }
        }
        if (stop.pars.ended >= 0 && MSGlobals::gUseStopEnded) {
            label += " ended:" + time2string(stop.pars.ended);
        } else if (stop.pars.until >= 0) {
            label += " until:" + time2string(stop.pars.until);
        }
        if (stop.duration >= 0 || stop.pars.duration > 0) {
            if (STEPS2TIME(stop.duration) > 3600 * 24) {
                label += " duration:1day+";
            } else {
                label += " duration:" + time2string(stop.duration);
            }
        }
        if (stop.getSpeed() > 0) {
            if (stop.skipOnDemand) {
                label += " onDemand (skipped)";
            } else {
                label += " speed:" + toString(stop.getSpeed());
            }
        }
        if (stop.pars.actType != "") {
            label += " actType:" + stop.pars.actType;
        }
        const double nameSize = s.vehicleName.size / s.scale;
        Position pos2 = pos - Position(0, nameSize * repeat[stop.lane]);
        if (noLoop && repeat[stop.lane] > 0) {
            break;
        }
        GLHelper::drawTextSettings(s.vehicleText, label, pos2, s.scale, s.angle, 1.0);
        repeat[stop.lane]++;
        stopIndex++;
    }
    // indicate arrivalPos if set
    if (myVehicle.getParameter().wasSet(VEHPARS_ARRIVALPOS_SET) || myVehicle.getArrivalLane() >= 0) {
        const int arrivalEdge = myVehicle.getParameter().arrivalEdge >= 0
                                ? myVehicle.getParameter().arrivalEdge
                                : (int)myVehicle.getRoute().getEdges().size() - 1;
        const MSLane* arrivalLane = myVehicle.getRoute().getEdges()[arrivalEdge]->getLanes()[MAX2(0, myVehicle.getArrivalLane())];
        Position pos = arrivalLane->geometryPositionAtOffset(myVehicle.getArrivalPos());
        GLHelper::setColor(col);
        GLHelper::drawBoxLines(arrivalLane->getShape().getOrthogonal(pos, 10, true, arrivalLane->getWidth() * 0.5,  90), 0.1);
        GLHelper::drawBoxLines(arrivalLane->getShape().getOrthogonal(pos, 10, true, arrivalLane->getWidth() * 0.5, 270), 0.1);
        GLHelper::drawTextSettings(s.vehicleText, "arrival", pos, s.scale, s.angle, 1.0);

    }
}

void
GUIBaseVehicle::drawParkingInfo(const GUIVisualizationSettings& s) const {
    if (s.showParkingInfo) {
        const StoppingPlaceMemory* pm = myVehicle.getParkingMemory();
        if (pm != nullptr) {
            for (auto item : *pm) {
                const GUIParkingArea* pa = dynamic_cast<const GUIParkingArea*>(item.first);
                if (item.second.blockedAtTime >= 0) {
                    std::string seenAgo = time2string(SIMSTEP - item.second.blockedAtTime);
                    //if (item.second.blockedAtTime >= 0) {
                    //    seenAgo += ", " + time2string(SIMSTEP - item.second.blockedAtTimeLocal);
                    //}
                    GLHelper::drawTextSettings(s.vehicleValue, seenAgo, pa->getSignPos(), s.scale, s.angle, 1.0);
                }
                if (item.second.score != "") {
                    const double dist = 0.4 * (s.vehicleText.scaledSize(s.scale) + s.vehicleValue.scaledSize(s.scale));
                    Position shift(0, -dist);
                    GLHelper::drawTextSettings(s.vehicleText, item.second.score, pa->getSignPos() + shift, s.scale, s.angle, 1.0);
                }
            }
        }
    }
}

void
GUIBaseVehicle::drawChargingInfo(const GUIVisualizationSettings& s) const {
    if (s.showChargingInfo) {
        const StoppingPlaceMemory* pm = myVehicle.getChargingMemory();
        if (pm != nullptr) {
            for (auto item : *pm) {
                const GUIChargingStation* cs = dynamic_cast<const GUIChargingStation*>(item.first);
                if (item.second.blockedAtTime >= 0) {
                    std::string seenAgo = time2string(SIMSTEP - item.second.blockedAtTime);
                    GLHelper::drawTextSettings(s.vehicleValue, seenAgo, cs->getSignPos(), s.scale, s.angle, 1.0);
                }
                if (item.second.score != "") {
                    const double dist = 0.4 * (s.vehicleText.scaledSize(s.scale) + s.vehicleValue.scaledSize(s.scale));
                    Position shift(0, -dist);
                    GLHelper::drawTextSettings(s.vehicleText, item.second.score, cs->getSignPos() + shift, s.scale, s.angle, 1.0);
                }
            }
        }
    }
}

const GUIBaseVehicle::Seat&
GUIBaseVehicle::getSeatPosition(int personIndex) const {
    /// if there are not enough seats in the vehicle people have to squeeze onto the last seat
    return mySeatPositions[MIN2(personIndex, (int)mySeatPositions.size() - 1)];
}

const GUIBaseVehicle::Seat&
GUIBaseVehicle::getContainerPosition(int containerIndex) const {
    /// if there are not enough positions in the vehicle containers have to squeeze onto the last position
    return myContainerPositions[MIN2(containerIndex, (int)myContainerPositions.size() - 1)];
}


void
GUIBaseVehicle::drawAction_drawPersonsAndContainers(const GUIVisualizationSettings& s) const {
    if (myVehicle.myPersonDevice != nullptr) {
        const std::vector<MSTransportable*>& ps = myVehicle.myPersonDevice->getTransportables();
        int personIndex = 0;
        for (std::vector<MSTransportable*>::const_iterator i = ps.begin(); i != ps.end(); ++i) {
            GUIPerson* person = dynamic_cast<GUIPerson*>(*i);
            assert(person != 0);
            person->setPositionInVehicle(getSeatPosition(personIndex++));
            person->drawGL(s);
        }
    }
    if (myVehicle.myContainerDevice != nullptr) {
        const std::vector<MSTransportable*>& cs = myVehicle.myContainerDevice->getTransportables();
        int containerIndex = 0;
        for (std::vector<MSTransportable*>::const_iterator i = cs.begin(); i != cs.end(); ++i) {
            GUIContainer* container = dynamic_cast<GUIContainer*>(*i);
            assert(container != 0);
            container->setPositionInVehicle(getContainerPosition(containerIndex++));
            container->drawGL(s);
        }
    }
#ifdef DRAW_BOUNDING_BOX
    if (!MSGlobals::gUseMesoSim) {
        MSVehicle& microVeh = dynamic_cast<MSVehicle&>(myVehicle);
        GLHelper::pushName(getGlID());
        GLHelper::pushMatrix();
        glTranslated(0, 0, getType());
        PositionVector smallBB = microVeh.getBoundingPoly();
        glColor3d(0.5, .8, 0);
        GLHelper::drawBoxLines(smallBB, 0.3);
        glTranslated(0, 0, 0.1);
        PositionVector boundingBox = microVeh.getBoundingBox();
        boundingBox.push_back(boundingBox.front());
        glColor3d(1, 0, 0);
        GLHelper::drawBoxLines(boundingBox, 0.15);
        GLHelper::popMatrix();
        GLHelper::popName();
    }
#endif
}

bool
GUIBaseVehicle::drawReversed(const GUIVisualizationSettings& s) const {
    return myVehicle.isReversed() && s.drawReversed;
}

bool
GUIBaseVehicle::drawAction_drawVehicleAsPolyWithCarriagges(const GUIVisualizationSettings& s, double scaledLength, bool asImage) const {
    if (getVType().getParameter().carriageLength > 0 &&
            (!myVehicle.isParking() || myVehicle.getNextStop().parkingarea == nullptr || myVehicle.getNextStop().parkingarea->parkOnRoad())) {
        drawAction_drawCarriageClass(s, scaledLength, asImage);
        return true;
    } else {
        if (asImage && GUIBaseVehicleHelper::drawAction_drawVehicleAsImage(
                    s, getVType().getImgFile(), this, getVType().getWidth(), scaledLength)) {
            return false;
        }
        GUIBaseVehicleHelper::drawAction_drawVehicleAsPoly(s, getVType().getGuiShape(), getVType().getWidth(), scaledLength, -1, myVehicle.isStopped(), drawReversed(s));
        return false;
    }
}


int
GUIBaseVehicle::getNumPassengers() const {
    if (myVehicle.getPersonDevice() != nullptr) {
        return (int)myVehicle.getPersonDevice()->size();
    }
    return 0;
}


int
GUIBaseVehicle::getNumContainers() const {
    if (myVehicle.getContainerDevice() != nullptr) {
        return (int)myVehicle.getContainerDevice()->size();
    }
    return 0;
}

std::string
GUIBaseVehicle::getDeviceDescription() {
    std::vector<std::string> devs;
    for (MSDevice* d : myVehicle.getDevices()) {
        devs.push_back(d->deviceName());
    }
    return joinToString(devs, " ");
}


void
GUIBaseVehicle::computeSeats(const Position& front, const Position& back, double seatOffset, int maxSeats, double exaggeration, int& requiredSeats, Seats& into, double extraOffset) const {
    if (requiredSeats <= 0) {
        return;
    }
    maxSeats = MAX2(maxSeats, 1); // compute at least one seat
    seatOffset *= exaggeration;
    const double vehWidth = getVType().getSeatingWidth() * exaggeration;
    const double length = front.distanceTo2D(back);
    const int rowSize = MAX2(1, (int)floor(vehWidth / seatOffset));
    const double frontSeatPos = getVType().getFrontSeatPos() + extraOffset;
    const double rowOffset = MAX2(1.0, (length - frontSeatPos - 1)) / ceil((double)maxSeats / rowSize);
    const double sideOffset = (rowSize - 1) / 2.0 * seatOffset;
    double rowPos = frontSeatPos - rowOffset;
    double angle = back.angleTo2D(front);
    const int fillDirection = MSGlobals::gLefthand ? -1 : 1;
    //if (myVehicle.getID() == "v0") std::cout << SIMTIME << " seatOffset=" << seatOffset << " max=" << maxSeats << " ex=" << exaggeration << " req=" << requiredSeats << " rowSize=" << rowSize << " sideOffset=" << sideOffset << " front=" << front << " back=" << back << " a=" << angle << " da=" << RAD2DEG(angle) << "\n";
    for (int i = 0; requiredSeats > 0 && i < maxSeats; i++) {
        int seat = (i % rowSize);
        if (seat == 0) {
            rowPos += rowOffset;
        }
        into.push_back(Seat(PositionVector::positionAtOffset2D(front, back, rowPos, (sideOffset - seat * seatOffset) * fillDirection), angle));
        requiredSeats--;
    }
}


/****************************************************************************/
