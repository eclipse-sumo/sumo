/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GUIViewTraffic.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christian Roessel
/// @author  Michael Behrisch
/// @author  Andreas Gaubatz
/// @date    Sept 2002
///
// A view on the simulation; this view is a microscopic one
/****************************************************************************/
#include <config.h>

#ifdef HAVE_FFMPEG
#include <utils/gui/div/GUIVideoEncoder.h>
#endif

#include <iostream>
#include <utility>
#include <cmath>
#include <limits>
#include <foreign/rtree/SUMORTree.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUINet.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIVehicleControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <utils/common/RGBColor.h>
#include <utils/foxtools/MFXButtonTooltip.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/shapes/ShapeContainer.h>

#include "GUISUMOViewParent.h"
#include "GUIViewTraffic.h"

// ===========================================================================
// member method definitions
// ===========================================================================
GUIViewTraffic::GUIViewTraffic(
    FXComposite* p,
    GUIMainWindow& app,
    GUISUMOViewParent* parent,
    GUINet& net, FXGLVisual* glVis,
    FXGLCanvas* share) :
    GUISUMOAbstractView(p, app, parent, net.getVisualisationSpeedUp(), glVis, share),
    myTrackedID(GUIGlObject::INVALID_ID),
    myTLSGame(OptionsCont::getOptions().getString("game.mode") == "tls")
#ifdef HAVE_FFMPEG
    , myCurrentVideo(nullptr)
#endif
{}


GUIViewTraffic::~GUIViewTraffic() {
    endSnapshot();
}


void
GUIViewTraffic::recalculateBoundaries() {
    //
}


void
GUIViewTraffic::buildViewToolBars(GUIGlChildWindow* v) {
    // build coloring tools
    {
        const std::vector<std::string>& names = gSchemeStorage.getNames();
        for (std::vector<std::string>::const_iterator i = names.begin(); i != names.end(); ++i) {
            v->getColoringSchemesCombo()->appendItem(i->c_str());
            if ((*i) == myVisualizationSettings->name) {
                v->getColoringSchemesCombo()->setCurrentItem(v->getColoringSchemesCombo()->getNumItems() - 1);
            }
        }
        v->getColoringSchemesCombo()->setNumVisible(MAX2(5, (int)names.size() + 1));
    }
    // for junctions
    new MFXButtonTooltip(v->getLocatorPopup(), myApp->getStaticTooltipMenu(),
                         (std::string("\t") + TL("Locate Junctions") + std::string("\t") + TL("Locate a junction within the network.")).c_str(),
                         GUIIconSubSys::getIcon(GUIIcon::LOCATEJUNCTION), v, MID_HOTKEY_SHIFT_J_LOCATEJUNCTION,
                         GUIDesignButtonPopup);
    // for edges
    new MFXButtonTooltip(v->getLocatorPopup(), myApp->getStaticTooltipMenu(),
                         (std::string("\t") + TL("Locate Edges") + std::string("\t") + TL("Locate an edge within the network.")).c_str(),
                         GUIIconSubSys::getIcon(GUIIcon::LOCATEEDGE), v, MID_HOTKEY_SHIFT_E_LOCATEEDGE,
                         GUIDesignButtonPopup);
    // for vehicles
    new MFXButtonTooltip(v->getLocatorPopup(), myApp->getStaticTooltipMenu(),
                         (std::string("\t") + TL("Locate Vehicles") + std::string("\t") + TL("Locate a vehicle within the network.")).c_str(),
                         GUIIconSubSys::getIcon(GUIIcon::LOCATEVEHICLE), v, MID_HOTKEY_SHIFT_V_LOCATEVEHICLE,
                         GUIDesignButtonPopup);
    // for persons
    new MFXButtonTooltip(v->getLocatorPopup(), myApp->getStaticTooltipMenu(),
                         (std::string("\t") + TL("Locate Persons") + std::string("\t") + TL("Locate a person within the network.")).c_str(),
                         GUIIconSubSys::getIcon(GUIIcon::LOCATEPERSON), v, MID_HOTKEY_SHIFT_P_LOCATEPERSON,
                         GUIDesignButtonPopup);
    // for containers
    new MFXButtonTooltip(v->getLocatorPopup(), myApp->getStaticTooltipMenu(),
                         (std::string("\t") + TL("Locate Container") + std::string("\t") + TL("Locate a container within the network.")).c_str(),
                         GUIIconSubSys::getIcon(GUIIcon::LOCATECONTAINER), v, MID_HOTKEY_SHIFT_C_LOCATECONTAINER,
                         GUIDesignButtonPopup);
    // for tls
    new MFXButtonTooltip(v->getLocatorPopup(), myApp->getStaticTooltipMenu(),
                         (std::string("\t") + TL("Locate TLS") + std::string("\t") + TL("Locate a tls within the network.")).c_str(),
                         GUIIconSubSys::getIcon(GUIIcon::LOCATETLS), v, MID_HOTKEY_SHIFT_T_LOCATETLS,
                         GUIDesignButtonPopup);
    // for additional stuff
    new MFXButtonTooltip(v->getLocatorPopup(), myApp->getStaticTooltipMenu(),
                         (std::string("\t") + TL("Locate Additional") + std::string("\t") + TL("Locate an additional structure within the network.")).c_str(),
                         GUIIconSubSys::getIcon(GUIIcon::LOCATEADD), v, MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL,
                         GUIDesignButtonPopup);
    // for pois
    new MFXButtonTooltip(v->getLocatorPopup(), myApp->getStaticTooltipMenu(),
                         (std::string("\t") + TL("Locate PoI") + std::string("\t") + TL("Locate a PoI within the network.")).c_str(),
                         GUIIconSubSys::getIcon(GUIIcon::LOCATEPOI), v, MID_HOTKEY_SHIFT_O_LOCATEPOI,
                         GUIDesignButtonPopup);
    // for polygons
    new MFXButtonTooltip(v->getLocatorPopup(), myApp->getStaticTooltipMenu(),
                         (std::string("\t") + TL("Locate Polygon") + std::string("\t") + TL("Locate a Polygon within the network.")).c_str(),
                         GUIIconSubSys::getIcon(GUIIcon::LOCATEPOLY), v, MID_HOTKEY_SHIFT_L_LOCATEPOLY,
                         GUIDesignButtonPopup);
}


bool
GUIViewTraffic::setColorScheme(const std::string& name) {
    if (!gSchemeStorage.contains(name)) {
        return false;
    }
    if (myGUIDialogViewSettings != nullptr) {
        if (myGUIDialogViewSettings->getCurrentScheme() != name) {
            myGUIDialogViewSettings->setCurrentScheme(name);
        }
    }
    myVisualizationSettings = &gSchemeStorage.get(name.c_str());
    myVisualizationSettings->gaming = myApp->isGaming();
    update();
    return true;
}


void
GUIViewTraffic::buildColorRainbow(const GUIVisualizationSettings& s, GUIColorScheme& scheme, int active, GUIGlObjectType objectType,
                                  bool hide, double hideThreshold, bool hide2, double hideThreshold2) {
    assert(!scheme.isFixed());
    double minValue = std::numeric_limits<double>::infinity();
    double maxValue = -std::numeric_limits<double>::infinity();
    // retrieve range
    bool hasMissingData = false;
    if (objectType == GLO_LANE) {
        // XXX (see #3409) multi-colors are not currently handled. this is a quick hack
        if (active == 22) {
            active = 21; // segment height, fall back to start height
        } else if (active == 24) {
            active = 23; // segment incline, fall back to total incline
        }
        const MSEdgeVector& edges = MSEdge::getAllEdges();
        for (MSEdgeVector::const_iterator it = edges.begin(); it != edges.end(); ++it) {
            if (MSGlobals::gUseMesoSim) {
                const double val = static_cast<GUIEdge*>(*it)->getColorValue(s, active);
                if (val == s.MISSING_DATA) {
                    hasMissingData = true;
                    continue;
                }
                minValue = MIN2(minValue, val);
                maxValue = MAX2(maxValue, val);
            } else {
                const std::vector<MSLane*>& lanes = (*it)->getLanes();
                for (std::vector<MSLane*>::const_iterator it_l = lanes.begin(); it_l != lanes.end(); it_l++) {
                    const double val = static_cast<GUILane*>(*it_l)->getColorValue(s, active);
                    if (val == s.MISSING_DATA) {
                        hasMissingData = true;
                        continue;
                    }
                    minValue = MIN2(minValue, val);
                    maxValue = MAX2(maxValue, val);
                }
            }
        }
    } else if (objectType == GLO_JUNCTION) {
        if (active == 3) {
            std::set<const MSJunction*> junctions;
            for (MSEdge* edge : MSEdge::getAllEdges()) {
                junctions.insert(edge->getFromJunction());
                junctions.insert(edge->getToJunction());
            }
            for (const MSJunction* junction : junctions) {
                minValue = MIN2(minValue, junction->getPosition().z());
                maxValue = MAX2(maxValue, junction->getPosition().z());
            }
        }
    }
    if (scheme.getName() == GUIVisualizationSettings::SCHEME_NAME_PERMISSION_CODE) {
        scheme.clear();
        // add threshold for every distinct value
        std::set<SVCPermissions> codes;
        for (MSEdge* edge : MSEdge::getAllEdges()) {
            for (MSLane* lane : edge->getLanes()) {
                codes.insert(lane->getPermissions());
            }
        }
        int step = MAX2(1, 360 / (int)codes.size());
        int hue = 0;
        for (SVCPermissions p : codes) {
            scheme.addColor(RGBColor::fromHSV(hue, 1, 1), p);
            hue = (hue + step) % 360;
        }
        return;
    }

    if (hide && hide2 && minValue == std::numeric_limits<double>::infinity()) {
        minValue = hideThreshold;
        maxValue = hideThreshold2;
    }
    if (minValue != std::numeric_limits<double>::infinity()) {
        scheme.clear();
        // add new thresholds
        if (scheme.getName() == GUIVisualizationSettings::SCHEME_NAME_EDGEDATA_NUMERICAL
                || scheme.getName() == GUIVisualizationSettings::SCHEME_NAME_EDGE_PARAM_NUMERICAL
                || scheme.getName() == GUIVisualizationSettings::SCHEME_NAME_LANE_PARAM_NUMERICAL
                || scheme.getName() == GUIVisualizationSettings::SCHEME_NAME_DATA_ATTRIBUTE_NUMERICAL
                || scheme.getName() == GUIVisualizationSettings::SCHEME_NAME_PARAM_NUMERICAL
                || hasMissingData)  {
            scheme.addColor(s.COL_MISSING_DATA, s.MISSING_DATA, "missing data");
        }
        if (hide) {
            const double rawRange = maxValue - minValue;
            minValue = MAX2(hideThreshold + MIN2(1.0, rawRange / 100.0), minValue);
            scheme.addColor(RGBColor(204, 204, 204), hideThreshold);
        }
        if (hide2) {
            const double rawRange = maxValue - minValue;
            maxValue = MIN2(hideThreshold2 - MIN2(1.0, rawRange / 100.0), maxValue);
            scheme.addColor(RGBColor(204, 204, 204), hideThreshold2);
        }
        double range = maxValue - minValue;
        scheme.addColor(RGBColor::RED, (minValue));
        scheme.addColor(RGBColor::ORANGE, (minValue + range * 1 / 6.0));
        scheme.addColor(RGBColor::YELLOW, (minValue + range * 2 / 6.0));
        scheme.addColor(RGBColor::GREEN, (minValue + range * 3 / 6.0));
        scheme.addColor(RGBColor::CYAN, (minValue + range * 4 / 6.0));
        scheme.addColor(RGBColor::BLUE, (minValue + range * 5 / 6.0));
        scheme.addColor(RGBColor::MAGENTA, (maxValue));
    }
}


std::vector<std::string>
GUIViewTraffic::getEdgeDataAttrs() const {
    if (GUINet::getGUIInstance() != nullptr) {
        return GUINet::getGUIInstance()->getEdgeDataAttrs();
    }
    return std::vector<std::string>();
}


std::vector<std::string>
GUIViewTraffic::getMeanDataIDs() const {
    if (GUINet::getGUIInstance() != nullptr) {
        return GUINet::getGUIInstance()->getMeanDataIDs();
    }
    return std::vector<std::string>();
}

std::vector<std::string>
GUIViewTraffic::getMeanDataAttrs(const std::string& meanDataID) const {
    if (GUINet::getGUIInstance() != nullptr) {
        return GUINet::getGUIInstance()->getMeanDataAttrs(meanDataID);
    }
    return std::vector<std::string>();
}


std::vector<std::string>
GUIViewTraffic::getEdgeLaneParamKeys(bool edgeKeys) const {
    std::set<std::string> keys;
    for (const MSEdge* e : MSEdge::getAllEdges()) {
        if (edgeKeys) {
            for (const auto& item : e->getParametersMap()) {
                keys.insert(item.first);
            }
        } else {
            for (const auto lane : e->getLanes()) {
                for (const auto& item : lane->getParametersMap()) {
                    keys.insert(item.first);
                }
            }
        }
    }
    return std::vector<std::string>(keys.begin(), keys.end());
}


std::vector<std::string>
GUIViewTraffic::getVehicleParamKeys(bool /*vTypeKeys*/) const {
    std::set<std::string> keys;
    GUIVehicleControl* vc = GUINet::getGUIInstance()->getGUIVehicleControl();
    vc->secureVehicles();
    for (auto vehIt = vc->loadedVehBegin(); vehIt != vc->loadedVehEnd(); ++vehIt) {
        for (auto kv : vehIt->second->getParameter().getParametersMap()) {
            keys.insert(kv.first);
        }
    }
    vc->releaseVehicles();
    return std::vector<std::string>(keys.begin(), keys.end());
}

std::vector<std::string>
GUIViewTraffic::getPOIParamKeys() const {
    std::set<std::string> keys;
    const ShapeContainer::POIs& pois = static_cast<ShapeContainer&>(GUINet::getInstance()->getShapeContainer()).getPOIs();
    for (auto item : pois) {
        for (auto kv : item.second->getParametersMap()) {
            keys.insert(kv.first);
        }
    }
    return std::vector<std::string>(keys.begin(), keys.end());
}

int
GUIViewTraffic::doPaintGL(int mode, const Boundary& bound) {
    if (!myVisualizationSettings->drawForPositionSelection && myVisualizationSettings->forceDrawForPositionSelection) {
        myVisualizationSettings->drawForPositionSelection = true;
    }
    // init view settings
    glRenderMode(mode);
    glMatrixMode(GL_MODELVIEW);
    GLHelper::pushMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // draw decals (if not in grabbing mode)
    drawDecals();
    myVisualizationSettings->scale = myVisualizationSettings->drawForPositionSelection ? myVisualizationSettings->scale : m2p(SUMO_const_laneWidth);
    if (myVisualizationSettings->showGrid) {
        paintGLGrid();
    }


    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    const float minB[2] = { (float)bound.xmin(), (float)bound.ymin() };
    const float maxB[2] = { (float)bound.xmax(), (float)bound.ymax() };
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_POLYGON_OFFSET_LINE);
    const SUMORTree& grid = GUINet::getGUIInstance()->getVisualisationSpeedUp(myVisualizationSettings->secondaryShape);
    int hits2 = grid.Search(minB, maxB, *myVisualizationSettings);
    GUIGlobals::gSecondaryShape = myVisualizationSettings->secondaryShape;
    // Draw additional objects
    if (myAdditionallyDrawn.size() > 0) {
        glTranslated(0, 0, -.01);
        GUINet::getGUIInstance()->lock();
        for (auto i : myAdditionallyDrawn) {
            i.first->drawGLAdditional(this, *myVisualizationSettings);
        }
        GUINet::getGUIInstance()->unlock();
        glTranslated(0, 0, .01);
    }
    GLHelper::popMatrix();
    /*
    // draw legends
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(1.-.2, 1.-.5, 0.);
    glScaled(.2, .5, 1.);
    GUIColoringSchemesMap<GUILane> &sm = GUIViewTraffic::getLaneSchemesMap(); //!!!
    sm.getColorer(myVisualizationSettings->laneEdgeMode)->drawLegend();
    */
    return hits2;
}


void
GUIViewTraffic::startTrack(int id) {
    myTrackedID = id;
    GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
    if (o != nullptr) {
        GUIBaseVehicle* v = dynamic_cast<GUIBaseVehicle*>(o);
        if (v != nullptr) {
            v->addActiveAddVisualisation(this, GUIBaseVehicle::VO_TRACK);
        }
    }
}


void
GUIViewTraffic::stopTrack() {
    myTrackedID = GUIGlObject::INVALID_ID;
}


GUIGlID
GUIViewTraffic::getTrackedID() const {
    return myTrackedID;
}


void
GUIViewTraffic::onGamingClick(Position pos) {
    if (myTLSGame) {
        MSTLLogicControl& tlsControl = MSNet::getInstance()->getTLSControl();
        MSTrafficLightLogic* minTll = nullptr;
        double minDist = std::numeric_limits<double>::infinity();
        for (MSTrafficLightLogic* const tll : tlsControl.getAllLogics()) {
            if (tlsControl.isActive(tll) && tll->getProgramID() != "off") {
                // get the links
                const MSTrafficLightLogic::LaneVector& lanes = tll->getLanesAt(0);
                if (lanes.size() > 0) {
                    const Position& endPos = lanes[0]->getShape().back();
                    if (endPos.distanceTo(pos) < minDist) {
                        minDist = endPos.distanceTo(pos);
                        minTll = tll;
                    }
                }
            }
        }
        if (minTll != nullptr) {
            if (minTll->getPhaseNumber() == 0) {
                // MSRailSignal
                return;
            }
            const int ci = minTll->getCurrentPhaseIndex();
            const int n = minTll->getPhaseNumber();
            int nextPhase = (ci + 1) % n;
            SUMOTime nextDuration = 0;
            if (minTll->getCurrentPhaseDef().isGreenPhase()) {
                nextDuration = minTll->getPhase(nextPhase).duration;
            } else {
                // we are in transition to a green phase
                // -> skip forward to the transtion into the next green phase
                // but ensure that the total transition time is maintained
                // taking into account how much time was already spent
                SUMOTime spentTransition = minTll->getSpentDuration();
                // the transition may consist of more than one phase so we
                // search backwards until the prior green phase
                for (int i = ci - 1; i != ci; i--) {
                    if (i < 0) {
                        i = n - 1;
                    }
                    if (minTll->getPhase(i).isGreenPhase()) {
                        break;
                    }
                    spentTransition += minTll->getPhase(i).duration;
                }
                // now we skip past the next greenphase
                int numGreen = 0;
                int i = nextPhase;
                for (; numGreen < 2; i = (i + 1) % n) {
                    if (minTll->getPhase(i).isGreenPhase()) {
                        numGreen++;
                        continue;
                    }
                    // transition after the next green
                    if (numGreen == 1) {
                        SUMOTime dur = minTll->getPhase(i).duration;
                        if (dur <= spentTransition) {
                            spentTransition -= dur;
                        } else {
                            nextPhase = i;
                            nextDuration = dur - spentTransition;
                            break;
                        }
                    }
                }
            }
            minTll->changeStepAndDuration(tlsControl, MSNet::getInstance()->getCurrentTimeStep(), nextPhase, nextDuration);
            update();
        }
    } else {
        // DRT game
        if (MSGlobals::gUseMesoSim) {
            return;
        }
        const std::set<GUIGlID>& sel = gSelected.getSelected(GLO_VEHICLE);
        if (sel.size() == 0) {
            // find closest pt vehicle
            double minDist = std::numeric_limits<double>::infinity();
            GUIVehicle* closest = nullptr;
            MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
            MSVehicleControl::constVehIt it = vc.loadedVehBegin();
            MSVehicleControl::constVehIt end = vc.loadedVehEnd();
            for (it = vc.loadedVehBegin(); it != end; ++it) {
                GUIVehicle* veh = dynamic_cast<GUIVehicle*>(it->second);
                assert(veh != 0);
                if (veh->getParameter().line != "") {
                    const double dist = veh->getPosition().distanceTo2D(pos);
                    if (dist < minDist) {
                        minDist = dist;
                        closest = veh;
                    }
                }
            }
            if (closest != nullptr) {
                gSelected.select(closest->getGlID());
                closest->addActiveAddVisualisation(this, GUIBaseVehicle::VO_SHOW_FUTURE_ROUTE);
            }
        } else {
            // find closest pt stop
            double minDist = std::numeric_limits<double>::infinity();
            MSStoppingPlace* closestStop = nullptr;
            const NamedObjectCont<MSStoppingPlace*>& stops = MSNet::getInstance()->getStoppingPlaces(SUMO_TAG_BUS_STOP);
            for (auto it = stops.begin(); it != stops.end(); ++it) {
                MSStoppingPlace* stop = it->second;
                const double dist = pos.distanceTo2D(stop->getLane().geometryPositionAtOffset(stop->getEndLanePosition()));
                if (dist < minDist) {
                    minDist = dist;
                    closestStop = stop;
                }
            }
            if (closestStop != 0) {
                GUIGlID id = *sel.begin();
                GUIVehicle* veh = dynamic_cast<GUIVehicle*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(id));
                assert(veh != 0);
                MSLane* lane = veh->getMutableLane();
                lane->getVehiclesSecure();
                veh->rerouteDRTStop(closestStop);
                GUIGlObjectStorage::gIDStorage.unblockObject(id);
                lane->releaseVehicles();
            }
        }
    }
}


void
GUIViewTraffic::onGamingRightClick(Position /*pos*/) {
    const std::set<GUIGlID>& sel = gSelected.getSelected(GLO_VEHICLE);
    if (sel.size() > 0) {
        GUIGlID id = *sel.begin();
        GUIVehicle* veh = dynamic_cast<GUIVehicle*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(id));
        if (veh != 0) {
            veh->removeActiveAddVisualisation(this, GUIBaseVehicle::VO_SHOW_FUTURE_ROUTE);
        }
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
    }
    gSelected.clear();
}


SUMOTime
GUIViewTraffic::getCurrentTimeStep() const {
    return MSNet::getInstance()->getCurrentTimeStep();
}


long
GUIViewTraffic::onCmdCloseLane(FXObject*, FXSelector, void*) {
    GUILane* lane = getLaneUnderCursor();
    if (lane != nullptr) {
        lane->closeTraffic();
        GUIGlObjectStorage::gIDStorage.unblockObject(lane->getGlID());
        update();
    }
    return 1;
}


long
GUIViewTraffic::onCmdCloseEdge(FXObject*, FXSelector, void*) {
    GUILane* lane = getLaneUnderCursor();
    if (lane != nullptr) {
        dynamic_cast<GUIEdge*>(&lane->getEdge())->closeTraffic(lane);
        GUIGlObjectStorage::gIDStorage.unblockObject(lane->getGlID());
        update();
    }
    return 1;
}


long
GUIViewTraffic::onCmdAddRerouter(FXObject*, FXSelector, void*) {
    GUILane* lane = getLaneUnderCursor();
    if (lane != nullptr) {
        dynamic_cast<GUIEdge*>(&lane->getEdge())->addRerouter();
        GUIGlObjectStorage::gIDStorage.unblockObject(lane->getGlID());
        update();
    }
    return 1;
}


long
GUIViewTraffic::showLaneReachability(GUILane* lane, FXObject* menu, FXSelector) {
    if (lane != nullptr) {
        // reset
        const double UNREACHED = INVALID_DOUBLE;
        gSelected.clear();
        for (const MSEdge* const e : MSEdge::getAllEdges()) {
            for (MSLane* const l : e->getLanes()) {
                GUILane* gLane = dynamic_cast<GUILane*>(l);
                gLane->setReachability(UNREACHED);
            }
        }
        // prepare
        FXMenuCommand* mc = dynamic_cast<FXMenuCommand*>(menu);
        const SUMOVehicleClass svc = SumoVehicleClassStrings.get(mc->getText().text());
        const double defaultMaxSpeed = SUMOVTypeParameter::VClassDefaultValues(svc).maxSpeed;
        // find reachable
        std::map<MSEdge*, double> reachableEdges;
        reachableEdges[&lane->getEdge()] = 0;
        MSEdgeVector check;
        check.push_back(&lane->getEdge());
        while (check.size() > 0) {
            MSEdge* e = check.front();
            check.erase(check.begin());
            double traveltime = reachableEdges[e];
            for (MSLane* const l : e->getLanes()) {
                if (l->allowsVehicleClass(svc)) {
                    GUILane* gLane = dynamic_cast<GUILane*>(l);
                    gSelected.select(gLane->getGlID(), false);
                    gLane->setReachability(traveltime);
                }
            }
            const double dt = e->getLength() / MIN2(e->getSpeedLimit(), defaultMaxSpeed);
            // ensure algorithm termination
            traveltime += MAX2(dt, NUMERICAL_EPS);
            for (MSEdge* const nextEdge : e->getSuccessors(svc)) {
                if (reachableEdges.count(nextEdge) == 0 ||
                        // revisit edge via faster path
                        reachableEdges[nextEdge] > traveltime) {
                    reachableEdges[nextEdge] = traveltime;
                    check.push_back(nextEdge);
                }
            }
            if (svc == SVC_PEDESTRIAN) {
                // can also walk backwards
                for (MSEdge* const prevEdge : e->getPredecessors()) {
                    if (prevEdge->allowedLanes(*e, svc) != nullptr &&
                            (reachableEdges.count(prevEdge) == 0 ||
                             // revisit edge via faster path
                             reachableEdges[prevEdge] > traveltime)) {
                        reachableEdges[prevEdge] = traveltime;
                        check.push_back(prevEdge);
                    }
                }
            }
        }
        gSelected.notifyChanged();
    }
    return 1;
}


long
GUIViewTraffic::onCmdShowReachability(FXObject* menu, FXSelector selector, void*) {
    GUILane* lane = getLaneUnderCursor();
    if (lane != nullptr) {
        // reset
        showLaneReachability(lane, menu, selector);
        // switch to 'color by selection' unless coloring 'by reachability'
        if (myVisualizationSettings->laneColorer.getActive() != 36) {
            myVisualizationSettings->laneColorer.setActive(1);
        }
        update();
    }
    return 1;
}


GUILane*
GUIViewTraffic::getLaneUnderCursor() {
    if (makeCurrent()) {
        int id = getObjectUnderCursor();
        if (id != 0) {
            GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
            if (o != nullptr) {
                return dynamic_cast<GUILane*>(o);
            }
        }
        makeNonCurrent();
    }
    return nullptr;
}


long
GUIViewTraffic::onDoubleClicked(FXObject*, FXSelector, void*) {
    // leave fullscreen mode
    if (myApp->isFullScreen()) {
        myApp->onCmdFullScreen(nullptr, 0, nullptr);
    } else {
        stopTrack();
    }
    return 1;
}



void
GUIViewTraffic::saveFrame(const std::string& destFile, FXColor* buf) {
#ifdef HAVE_FFMPEG
    if (myCurrentVideo == nullptr) {
        myCurrentVideo = new GUIVideoEncoder(destFile.c_str(), getWidth(), getHeight(), myApp->getDelay());
    }
    myCurrentVideo->writeFrame((uint8_t*)buf);
#else
    UNUSED_PARAMETER(destFile);
    UNUSED_PARAMETER(buf);
#endif
}


void
GUIViewTraffic::endSnapshot() {
#ifdef HAVE_FFMPEG
    if (myCurrentVideo != nullptr) {
        delete myCurrentVideo;
        myCurrentVideo = nullptr;
    }
#endif
}


void
GUIViewTraffic::checkSnapshots() {
#ifdef HAVE_FFMPEG
    if (myCurrentVideo != nullptr) {
        addSnapshot(getCurrentTimeStep() - DELTA_T, "");
    }
#endif
    GUISUMOAbstractView::checkSnapshots();
}


const std::vector<SUMOTime>
GUIViewTraffic::retrieveBreakpoints() const {
    return myApp->retrieveBreakpoints();
}


/****************************************************************************/
