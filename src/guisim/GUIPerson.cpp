/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GUIPerson.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// A MSPerson extended by some values for usage within the gui
/****************************************************************************/
#include <config.h>

#include <gui/GUIApplicationWindow.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/transportables/MSPModel_Striping.h>
#include <microsim/transportables/MSStageWaiting.h>
#include <microsim/transportables/MSStageWalking.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ScopedLocker.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GUIBasePersonHelper.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GUILane.h"
#include "GUIPerson.h"

//#define GUIPerson_DEBUG_DRAW_WALKINGAREA_PATHS 1

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GUIPerson::GUIPersonPopupMenu) GUIPersonPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_CURRENTROUTE,     GUIPerson::GUIPersonPopupMenu::onCmdShowCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_CURRENTROUTE,     GUIPerson::GUIPersonPopupMenu::onCmdHideCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_WALKINGAREA_PATH, GUIPerson::GUIPersonPopupMenu::onCmdShowWalkingareaPath),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_WALKINGAREA_PATH, GUIPerson::GUIPersonPopupMenu::onCmdHideWalkingareaPath),
    FXMAPFUNC(SEL_COMMAND, MID_SHOWPLAN,              GUIPerson::GUIPersonPopupMenu::onCmdShowPlan),
    FXMAPFUNC(SEL_COMMAND, MID_START_TRACK,           GUIPerson::GUIPersonPopupMenu::onCmdStartTrack),
    FXMAPFUNC(SEL_COMMAND, MID_STOP_TRACK,            GUIPerson::GUIPersonPopupMenu::onCmdStopTrack),
    FXMAPFUNC(SEL_COMMAND, MID_REMOVE_OBJECT,         GUIPerson::GUIPersonPopupMenu::onCmdRemoveObject),
};

// Object implementation
FXIMPLEMENT(GUIPerson::GUIPersonPopupMenu, GUIGLObjectPopupMenu, GUIPersonPopupMenuMap, ARRAYNUMBER(GUIPersonPopupMenuMap))

// ===========================================================================
// method definitions
// ===========================================================================

// -------------------------------------------------------------------------
// GUIPerson::GUIPersonPopupMenu - methods
// -------------------------------------------------------------------------

GUIPerson::GUIPersonPopupMenu::GUIPersonPopupMenu(
    GUIMainWindow& app, GUISUMOAbstractView& parent, GUIGlObject& o) :
    GUIGLObjectPopupMenu(app, parent, o) {
}


GUIPerson::GUIPersonPopupMenu::~GUIPersonPopupMenu() {}


long
GUIPerson::GUIPersonPopupMenu::onCmdShowCurrentRoute(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_PERSON);
    if (!static_cast<GUIPerson*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_ROUTE)) {
        static_cast<GUIPerson*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_ROUTE);
    }
    return 1;
}


long
GUIPerson::GUIPersonPopupMenu::onCmdHideCurrentRoute(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_PERSON);
    static_cast<GUIPerson*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_ROUTE);
    return 1;
}


long
GUIPerson::GUIPersonPopupMenu::onCmdShowWalkingareaPath(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_PERSON);
    if (!static_cast<GUIPerson*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_WALKINGAREA_PATH)) {
        static_cast<GUIPerson*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_WALKINGAREA_PATH);
    }
    return 1;
}


long
GUIPerson::GUIPersonPopupMenu::onCmdHideWalkingareaPath(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_PERSON);
    static_cast<GUIPerson*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_WALKINGAREA_PATH);
    return 1;
}


long
GUIPerson::GUIPersonPopupMenu::onCmdShowPlan(FXObject*, FXSelector, void*) {
    GUIPerson* p = dynamic_cast<GUIPerson*>(myObject);
    if (p == nullptr) {
        return 1;
    }
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(*myApplication, *p);
    // add items
    for (int stage = 1; stage < p->getNumStages(); stage++) {
        ret->mkItem(toString(stage).c_str(), false, p->getStageSummary(stage));
    }
    // close building (use an object that is not Parameterised as argument)
    Parameterised dummyParameterised;
    ret->closeBuilding(&dummyParameterised);
    return 1;
}


long
GUIPerson::GUIPersonPopupMenu::onCmdStartTrack(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_PERSON);
    if (myParent->getTrackedID() != static_cast<GUIPerson*>(myObject)->getGlID()) {
        myParent->startTrack(static_cast<GUIPerson*>(myObject)->getGlID());
    }
    return 1;
}


long
GUIPerson::GUIPersonPopupMenu::onCmdStopTrack(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_PERSON);
    myParent->stopTrack();
    return 1;
}


long
GUIPerson::GUIPersonPopupMenu::onCmdRemoveObject(FXObject*, FXSelector, void*) {
    GUIPerson* person = static_cast<GUIPerson*>(myObject);
    MSStage* stage = person->getCurrentStage();
    stage->abort(person);
    stage->getEdge()->removeTransportable(person);
    if (stage->getDestinationStop() != nullptr) {
        stage->getDestinationStop()->removeTransportable(person);
    }
    MSNet::getInstance()->getPersonControl().erase(person);
    myParent->update();
    return 1;
}

// -------------------------------------------------------------------------
// GUIPerson - methods
// -------------------------------------------------------------------------

GUIPerson::GUIPerson(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan, const double speedFactor) :
    MSPerson(pars, vtype, plan, speedFactor),
    GUIGlObject(GLO_PERSON, pars->id, GUIIconSubSys::getIcon(GUIIcon::PERSON)),
    myLock(true)
{ }


GUIPerson::~GUIPerson() {
    myLock.lock();
    for (std::map<GUISUMOAbstractView*, int>::iterator i = myAdditionalVisualizations.begin(); i != myAdditionalVisualizations.end(); ++i) {
        if (i->first->getTrackedID() == getGlID()) {
            i->first->stopTrack();
        }
        while (i->first->removeAdditionalGLVisualisation(this));
    }
    myLock.unlock();
}


GUIGLObjectPopupMenu*
GUIPerson::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIPersonPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    if (hasActiveAddVisualisation(&parent, VO_SHOW_ROUTE)) {
        GUIDesigns::buildFXMenuCommand(ret, "Hide Current Route", nullptr, ret, MID_HIDE_CURRENTROUTE);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, "Show Current Route", nullptr, ret, MID_SHOW_CURRENTROUTE);
    }
    if (hasActiveAddVisualisation(&parent, VO_SHOW_WALKINGAREA_PATH)) {
        GUIDesigns::buildFXMenuCommand(ret, "Hide Walkingarea Path", nullptr, ret, MID_HIDE_WALKINGAREA_PATH);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, "Show Walkingarea Path", nullptr, ret, MID_SHOW_WALKINGAREA_PATH);
    }
    new FXMenuSeparator(ret);
    if (parent.getTrackedID() != getGlID()) {
        GUIDesigns::buildFXMenuCommand(ret, "Start Tracking", nullptr, ret, MID_START_TRACK);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, "Stop Tracking", nullptr, ret, MID_STOP_TRACK);
    }
    GUIDesigns::buildFXMenuCommand(ret, "Remove", nullptr, ret, MID_REMOVE_OBJECT);
    new FXMenuSeparator(ret);
    //
    buildShowParamsPopupEntry(ret);
    buildShowTypeParamsPopupEntry(ret);
    GUIDesigns::buildFXMenuCommand(ret, "Show Plan", GUIIconSubSys::getIcon(GUIIcon::APP_TABLE), ret, MID_SHOWPLAN);
    new FXMenuSeparator(ret);
    buildPositionCopyEntry(ret, app);
    return ret;
}


GUIParameterTableWindow*
GUIPerson::getParameterWindow(GUIMainWindow& app,
                              GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem(TL("stage"), true, new FunctionBindingString<GUIPerson>(this, &MSTransportable::getCurrentStageDescription));
    // there is always the "start" stage which we do not count here because it is not strictly part of the plan
    ret->mkItem(TL("stage index"), true, new FunctionBindingString<GUIPerson>(this, &GUIPerson::getStageIndexDescription));
    ret->mkItem(TL("start edge [id]"), true, new FunctionBindingString<GUIPerson>(this, &GUIPerson::getFromEdgeID));
    ret->mkItem(TL("dest edge [id]"), true, new FunctionBindingString<GUIPerson>(this, &GUIPerson::getDestinationEdgeID));
    ret->mkItem(TL("dest stop [id]"), true, new FunctionBindingString<GUIPerson>(this, &GUIPerson::getDestinationStopID));
    ret->mkItem(TL("arrival position [m]"), true, new FunctionBinding<GUIPerson, double>(this, &GUIPerson::getStageArrivalPos));
    ret->mkItem(TL("edge [id]"), true, new FunctionBindingString<GUIPerson>(this, &GUIPerson::getEdgeID));
    ret->mkItem(TL("position [m]"), true, new FunctionBinding<GUIPerson, double>(this, &GUIPerson::getEdgePos));
    ret->mkItem(TL("speed [m/s]"), true, new FunctionBinding<GUIPerson, double>(this, &GUIPerson::getSpeed));
    ret->mkItem(TL("speed factor"), false, getChosenSpeedFactor());
    ret->mkItem(TL("angle [degree]"), true, new FunctionBinding<GUIPerson, double>(this, &GUIPerson::getNaviDegree));
    ret->mkItem(TL("waiting time [s]"), true, new FunctionBinding<GUIPerson, double>(this, &GUIPerson::getWaitingSeconds));
    ret->mkItem(TL("vehicle [id]"), true, new FunctionBindingString<GUIPerson>(this, &GUIPerson::getVehicleID));
    ret->mkItem(TL("stop duration [s]"), true, new FunctionBinding<GUIPerson, double>(this, &GUIPerson::getStopDuration));
    ret->mkItem(TL("desired depart [s]"), false, time2string(getParameter().depart));
    // close building
    ret->closeBuilding(&getParameter());
    return ret;
}


GUIParameterTableWindow*
GUIPerson::getTypeParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, "vType:" + myVType->getID());
    ret->mkItem(TL("length"), false, myVType->getLength());
    ret->mkItem(TL("width"), false, myVType->getWidth());
    ret->mkItem(TL("height"), false, myVType->getHeight());
    ret->mkItem(TL("minGap"), false, myVType->getMinGap());
    ret->mkItem(TL("mass [kg]"), false, myVType->getMass());
    ret->mkItem(TL("desired max speed [m/s]"), false, myVType->getDesiredMaxSpeed());
    ret->mkItem(TL("maximum speed [m/s]"), false, myVType->getMaxSpeed());
    ret->closeBuilding(&(myVType->getParameter()));
    return ret;
}


double
GUIPerson::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.personSize.getExaggeration(s, this, s.personQuality == 1 ? 40 : 80);
}


Boundary
GUIPerson::getCenteringBoundary() const {
    Boundary b;
    // ensure that the vehicle is drawn, otherwise myPositionInVehicle will not be updated
    b.add(getGUIPosition());
    b.grow(MAX2(getVehicleType().getWidth(), getVehicleType().getLength()));
    return b;
}


void
GUIPerson::drawGL(const GUIVisualizationSettings& s) const {
    GLHelper::pushName(getGlID());
    GLHelper::pushMatrix();
    Position p1 = getGUIPosition(&s);
    double angle = getGUIAngle();
    glTranslated(p1.x(), p1.y(), getType());
    // set person color
    setColor(s);
    // scale
    const double exaggeration = getExaggeration(s);
    glScaled(exaggeration, exaggeration, 1);
    switch (s.personQuality) {
        case 0:
            GUIBasePersonHelper::drawAction_drawAsTriangle(angle, getVehicleType().getLength(), getVehicleType().getWidth());
            break;
        case 1:
            GUIBasePersonHelper::drawAction_drawAsCircle(angle, getVehicleType().getLength(), getVehicleType().getWidth(), s.scale * exaggeration);
            break;
        case 2:
            GUIBasePersonHelper::drawAction_drawAsPoly(angle, getVehicleType().getLength(), getVehicleType().getWidth());
            break;
        case 3:
        default:
            GUIBasePersonHelper::drawAction_drawAsImage(angle, getVehicleType().getLength(), getVehicleType().getWidth(),
                    getVehicleType().getImgFile(), getVehicleType().getGuiShape(), 1);
            break;
    }
    GLHelper::popMatrix();
#ifdef GUIPerson_DEBUG_DRAW_WALKINGAREA_PATHS
    drawAction_drawWalkingareaPath(s);
#endif
    drawName(p1, s.scale, s.personName, s.angle);
    if (s.personValue.show(this)) {
        Position p2 = p1 + Position(0, 0.6 * s.personName.scaledSize(s.scale));
        const double value = getColorValue(s, s.personColorer.getActive());
        GLHelper::drawTextSettings(s.personValue, toString(value), p2, s.scale, s.angle, GLO_MAX - getType());
    }
    GLHelper::popName();
}


void
GUIPerson::drawAction_drawWalkingareaPath(const GUIVisualizationSettings& s) const {
    MSStageWalking* stage = dynamic_cast<MSStageWalking*>(getCurrentStage());
    if (stage != nullptr) {
        setColor(s);
        MSPModel_Striping::PState* stripingState = dynamic_cast<MSPModel_Striping::PState*>(stage->getPState());
        if (stripingState != nullptr) {
            const MSPModel_Striping::WalkingAreaPath* waPath = stripingState->myWalkingAreaPath;
            if (waPath != nullptr) {
                GLHelper::pushMatrix();
                glTranslated(0, 0, getType());
                GLHelper::drawBoxLines(waPath->shape, 0.05);
                GLHelper::popMatrix();
            }
        }
    }
}

void
GUIPerson::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    GLHelper::pushName(getGlID());
    GLHelper::pushMatrix();
    glTranslated(0, 0, getType() - .1); // don't draw on top of other cars
    if (hasActiveAddVisualisation(parent, VO_SHOW_WALKINGAREA_PATH)) {
        drawAction_drawWalkingareaPath(s);
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_ROUTE)) {
        if (getCurrentStageType() == MSStageType::WALKING) {
            setColor(s);
            RGBColor current = GLHelper::getColor();
            RGBColor darker = current.changedBrightness(-51);
            GLHelper::setColor(darker);
            MSStageWalking* stage = dynamic_cast<MSStageWalking*>(getCurrentStage());
            assert(stage != 0);
            const double exaggeration = getExaggeration(s);
            const ConstMSEdgeVector& edges = stage->getRoute();
            const bool s2 = s.secondaryShape;
            for (ConstMSEdgeVector::const_iterator it = edges.begin(); it != edges.end(); ++it) {
                GUILane* lane = static_cast<GUILane*>((*it)->getLanes()[0]);
                GLHelper::drawBoxLines(lane->getShape(s2), lane->getShapeRotations(s2), lane->getShapeLengths(s2), exaggeration);
            }
        }
    }
    GLHelper::popMatrix();
    GLHelper::popName();
}


void
GUIPerson::setPositionInVehicle(const GUIBaseVehicle::Seat& pos) {
    myPositionInVehicle = pos;
}


void
GUIPerson::setColor(const GUIVisualizationSettings& s) const {
    RGBColor col;
    const GUIColorer& c = s.personColorer;
    if (!setFunctionalColor(c.getActive(), this, col)) {
        col = c.getScheme().getColor(getColorValue(s, c.getActive()));
    }
    GLHelper::setColor(col);
}


bool
GUIPerson::setFunctionalColor(int activeScheme, const MSPerson* person, RGBColor& col) {
    switch (activeScheme) {
        case 0: {
            if (person->getParameter().wasSet(VEHPARS_COLOR_SET)) {
                col = person->getParameter().color;
                return true;
            }
            if (person->getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                col = person->getVehicleType().getColor();
                return true;
            }
            return false;
        }
        case 2: {
            if (person->getParameter().wasSet(VEHPARS_COLOR_SET)) {
                col = person->getParameter().color;
                return true;
            }
            return false;
        }
        case 3: {
            if (person->getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                col = person->getVehicleType().getColor();
                return true;
            }
            return false;
        }
        case 9: { // color by angle
            double hue = GeomHelper::naviDegree(person->getAngle());
            col = RGBColor::fromHSV(hue, 1., 1.);
            return true;
        }
        case 10: { // color randomly (by pointer)
            const double hue = (double)((long long int)person % 360); // [0-360]
            const double sat = (double)(((long long int)person / 360) % 67) / 100. + 0.33; // [0.33-1]
            col = RGBColor::fromHSV(hue, sat, 1.);
            return true;
        }
        default:
            return false;
    }
}


double
GUIPerson::getColorValue(const GUIVisualizationSettings& /* s */, int activeScheme) const {
    switch (activeScheme) {
        case 4:
            switch (getCurrentStageType()) {
                case MSStageType::WAITING:
                    return -1;
                case MSStageType::WAITING_FOR_DEPART:
                    return -2;
                default:
                    return getSpeed();
            }
        case 5:
            if (isWaiting4Vehicle()) {
                return 5;
            } else {
                return (double)getCurrentStageType();
            }
        case 6:
            return getWaitingSeconds();
        case 7:
            return isJammed() ? 1 : 0;
        case 8:
            return gSelected.isSelected(GLO_PERSON, getGlID());
    }
    return 0;
}


double
GUIPerson::getEdgePos() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return -1;
    }
    return MSPerson::getEdgePos();
}

int
GUIPerson::getDirection() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return MSPModel::UNDEFINED_DIRECTION;
    }
    return MSPerson::getDirection();
}


Position
GUIPerson::getPosition() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return Position::INVALID;
    }
    return MSPerson::getPosition();
}


Position
GUIPerson::getGUIPosition(const GUIVisualizationSettings* s) const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return Position::INVALID;
    }
    if (getCurrentStageType() == MSStageType::DRIVING) {
        if (!isWaiting4Vehicle() && myPositionInVehicle.pos != Position::INVALID) {
            if (s != nullptr) {
                return myPositionInVehicle.pos;
            } else {
                // centering boundary must cover the vehicle regardless of exaggeration and zoom
                SUMOVehicle* veh = getCurrentStage()->getVehicle();
                if (veh == nullptr) {
                    // should not happen
                    return myPositionInVehicle.pos;
                }
                PositionVector b = veh->getBoundingBox();
                if (b.around(myPositionInVehicle.pos)) {
                    return myPositionInVehicle.pos;
                } else {
                    return b.getCentroid();
                }
            }
        } else if (isWaiting4Vehicle()
                   && s != nullptr
                   && s->gaming
                   && getCurrentStage()->getOriginStop() != nullptr
                   && s->addSize.getExaggeration(*s, nullptr) > 1) {
            // shift position away from stop center
            Position pos = MSPerson::getPosition();
            Position ref = getCurrentStage()->getOriginStop()->getCenterPos();
            Position shifted = ref + (pos - ref) * s->personSize.getExaggeration(*s, this);
            return shifted;
        }
    }
    return MSPerson::getPosition();
}


double
GUIPerson::getGUIAngle() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return INVALID_DOUBLE;
    }
    if (getCurrentStageType() == MSStageType::DRIVING && !isWaiting4Vehicle() && myPositionInVehicle.pos != Position::INVALID) {
        return myPositionInVehicle.angle;
    } else {
        return MSPerson::getAngle();
    }
}


double
GUIPerson::getNaviDegree() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return INVALID_DOUBLE;
    }
    return GeomHelper::naviDegree(MSPerson::getAngle());
}


double
GUIPerson::getWaitingSeconds() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return -1;
    }
    return MSPerson::getWaitingSeconds();
}


double
GUIPerson::getSpeed() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return -1;
    }
    return MSPerson::getSpeed();
}


std::string
GUIPerson::getStageIndexDescription() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return "arrived";
    }
    // there is always the "start" stage which we do not count here because it is not strictly part of the plan
    return toString(getCurrentStageIndex()) + " of " + toString(getNumStages() - 1);
}


std::string
GUIPerson::getEdgeID() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return "arrived";
    }
    return  getEdge()->getID();
}


std::string
GUIPerson::getFromEdgeID() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return "arrived";
    }
    return getFromEdge()->getID();
}


std::string
GUIPerson::getDestinationEdgeID() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return "arrived";
    }
    return getDestination()->getID();
}


std::string
GUIPerson::getDestinationStopID() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return "";
    }
    MSStoppingPlace* destStop = getCurrentStage()->getDestinationStop();
    if (destStop != nullptr) {
        return destStop->getID();
    } else {
        return "";
    }
}


std::string
GUIPerson::getVehicleID() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return "";
    }
    SUMOVehicle* veh = getCurrentStage()->getVehicle();
    if (veh != nullptr) {
        return veh->getID();
    } else {
        return "";
    }
}


double
GUIPerson::getStopDuration() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return -1;
    }
    if (getCurrentStage()->getStageType() == MSStageType::WAITING) {
        return STEPS2TIME(dynamic_cast<MSStageWaiting*>(getCurrentStage())->getStopEnd() - SIMSTEP);
    } else {
        return -1;
    }
}


double
GUIPerson::getStageArrivalPos() const {
    FXMutexLock locker(myLock);
    if (hasArrived()) {
        return INVALID_DOUBLE;
    }
    return getCurrentStage()->getArrivalPos();
}

bool
GUIPerson::proceed(MSNet* net, SUMOTime time, const bool vehicleArrived) {
    // acquire lock before locking the person to avoid mutual deadlock (#9468)
    ScopedLocker<const MSEdge, true> edgeLocker(*getEdge());
    FXMutexLock locker(myLock);
    return MSTransportable::proceed(net, time, vehicleArrived);
}

// -------------------------------------------------------------------------
// GUIPerson - Additional Visualsation methods
// -------------------------------------------------------------------------

bool
GUIPerson::hasActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) const {
    return myAdditionalVisualizations.find(parent) != myAdditionalVisualizations.end() && (myAdditionalVisualizations.find(parent)->second & which) != 0;
}


void
GUIPerson::addActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) {
    if (myAdditionalVisualizations.find(parent) == myAdditionalVisualizations.end()) {
        myAdditionalVisualizations[parent] = 0;
    }
    myAdditionalVisualizations[parent] |= which;
    parent->addAdditionalGLVisualisation(this);
}


void
GUIPerson::removeActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) {
    myAdditionalVisualizations[parent] &= ~which;
    parent->removeAdditionalGLVisualisation(this);
}

bool
GUIPerson::isSelected() const {
    return gSelected.isSelected(GLO_PERSON, getGlID());
}


/****************************************************************************/
