/****************************************************************************/
/// @file    GUINet.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
/// @version $Id$
///
// A MSNet extended by some values for usage within the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utility>
#include <set>
#include <vector>
#include <map>
#include <utils/shapes/ShapeContainer.h>
#include <utils/gui/globjects/GUIPolygon.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIShapeContainer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/RGBColor.h>
#include <utils/gui/div/GLObjectValuePassConnector.h>
#include <microsim/MSNet.h>
#include <microsim/MSJunction.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSEdge.h>
#include <microsim/pedestrians/MSPModel.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/MSJunctionControl.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUITransportableControl.h>
#include <guisim/GUILaneSpeedTrigger.h>
#include <guisim/GUIDetectorWrapper.h>
#include <guisim/GUITrafficLightLogicWrapper.h>
#include <guisim/GUIJunctionWrapper.h>
#include <guisim/GUIVehicleControl.h>
#include <gui/GUIGlobals.h>
#include "GUINet.h"

#include <mesogui/GUIMEVehicleControl.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// definition of static variables used for visualisation of objects' values
// ===========================================================================
template std::vector< GLObjectValuePassConnector<SUMOReal>* > GLObjectValuePassConnector<SUMOReal>::myContainer;
template MFXMutex GLObjectValuePassConnector<SUMOReal>::myLock;

template std::vector< GLObjectValuePassConnector<std::pair<int, class MSPhaseDefinition> >* > GLObjectValuePassConnector<std::pair<int, class MSPhaseDefinition> >::myContainer;
template MFXMutex GLObjectValuePassConnector<std::pair<int, class MSPhaseDefinition> >::myLock;


// ===========================================================================
// member method definitions
// ===========================================================================
GUINet::GUINet(MSVehicleControl* vc, MSEventControl* beginOfTimestepEvents,
               MSEventControl* endOfTimestepEvents, MSEventControl* insertionEvents) :
    MSNet(vc, beginOfTimestepEvents, endOfTimestepEvents, insertionEvents, new GUIShapeContainer(myGrid)),
    GUIGlObject(GLO_NETWORK, ""),
    myLastSimDuration(0), /*myLastVisDuration(0),*/ myLastIdleDuration(0),
    myLastVehicleMovementCount(0), myOverallVehicleCount(0), myOverallSimDuration(0) {
    GUIGlObjectStorage::gIDStorage.setNetObject(this);
}


GUINet::~GUINet() {
    if (myLock.locked()) {
        myLock.unlock();
    }
    // delete allocated wrappers
    //  of junctions
    for (std::vector<GUIJunctionWrapper*>::iterator i1 = myJunctionWrapper.begin(); i1 != myJunctionWrapper.end(); i1++) {
        delete(*i1);
    }
    //  of additional structures
    GUIGlObject_AbstractAdd::clearDictionary();
    //  of tl-logics
    for (Logics2WrapperMap::iterator i3 = myLogics2Wrapper.begin(); i3 != myLogics2Wrapper.end(); i3++) {
        delete(*i3).second;
    }
    //  of detectors
    for (std::vector<GUIDetectorWrapper*>::iterator i = myDetectorDict.begin(); i != myDetectorDict.end(); ++i) {
        delete *i;
    }
}


const Boundary&
GUINet::getBoundary() const {
    return myBoundary;
}


MSTransportableControl&
GUINet::getPersonControl() {
    if (myPersonControl == 0) {
        myPersonControl = new GUITransportableControl();
    }
    return *myPersonControl;
}


MSTransportableControl&
GUINet::getContainerControl() {
    if (myContainerControl == 0) {
        myContainerControl = new GUITransportableControl();
    }
    return *myContainerControl;
}


void
GUINet::initTLMap() {
    // get the list of loaded tl-logics
    const std::vector<MSTrafficLightLogic*>& logics = getTLSControl().getAllLogics();
    // allocate storage for the wrappers
    myTLLogicWrappers.reserve(logics.size());
    // go through the logics
    for (std::vector<MSTrafficLightLogic*>::const_iterator i = logics.begin(); i != logics.end(); ++i) {
        createTLWrapper(*i);
    }
}


GUIGlID
GUINet::createTLWrapper(MSTrafficLightLogic* tll) {
    if (myLogics2Wrapper.count(tll) > 0) {
        return myLogics2Wrapper[tll]->getGlID();
    }
    // get the links
    const MSTrafficLightLogic::LinkVectorVector& links = tll->getLinks();
    if (links.size() == 0) { // @legacy this should never happen in 0.13.0+ networks
        return 0;
    }
    // build the wrapper
    GUITrafficLightLogicWrapper* tllw =
        new GUITrafficLightLogicWrapper(*myLogics, *tll);
    // build the association link->wrapper
    MSTrafficLightLogic::LinkVectorVector::const_iterator j;
    for (j = links.begin(); j != links.end(); ++j) {
        MSTrafficLightLogic::LinkVector::const_iterator j2;
        for (j2 = (*j).begin(); j2 != (*j).end(); ++j2) {
            myLinks2Logic[*j2] = tll->getID();
        }
    }
    myGrid.addAdditionalGLObject(tllw);
    myLogics2Wrapper[tll] = tllw;
    return tllw->getGlID();
}


Position
GUINet::getJunctionPosition(const std::string& name) const {
    // !!! no check for existance!
    return myJunctions->get(name)->getPosition();
}


bool
GUINet::vehicleExists(const std::string& name) const {
    return myVehicleControl->getVehicle(name) != 0;
}


int
GUINet::getLinkTLID(MSLink* link) const {
    if (myLinks2Logic.count(link) == 0) {
        assert(false);
        return 0;
    }
    MSTrafficLightLogic* tll = myLogics->getActive(myLinks2Logic.find(link)->second);
    if (myLogics2Wrapper.count(tll) == 0) {
        // tll may have been added via traci. @see ticket #459
        return 0;
    }
    return myLogics2Wrapper.find(tll)->second->getGlID();
}


int
GUINet::getLinkTLIndex(MSLink* link) const {
    Links2LogicMap::const_iterator i = myLinks2Logic.find(link);
    if (i == myLinks2Logic.end()) {
        return -1;
    }
    if (myLogics2Wrapper.find(myLogics->getActive((*i).second)) == myLogics2Wrapper.end()) {
        return -1;
    }
    return myLogics2Wrapper.find(myLogics->getActive((*i).second))->second->getLinkIndex(link);
}


void
GUINet::guiSimulationStep() {
    GLObjectValuePassConnector<SUMOReal>::updateAll();
    GLObjectValuePassConnector<std::pair<SUMOTime, MSPhaseDefinition> >::updateAll();
}


void
GUINet::simulationStep() {
    AbstractMutex::ScopedLocker locker(myLock);
    MSNet::simulationStep();
}


std::vector<GUIGlID>
GUINet::getJunctionIDs(bool includeInternal) const {
    std::vector<GUIGlID> ret;
    for (std::vector<GUIJunctionWrapper*>::const_iterator i = myJunctionWrapper.begin(); i != myJunctionWrapper.end(); ++i) {
        if (!(*i)->isInner() || includeInternal) {
            ret.push_back((*i)->getGlID());
        }
    }
    return ret;
}


std::vector<GUIGlID>
GUINet::getTLSIDs() const {
    std::vector<GUIGlID> ret;
    std::vector<std::string> ids;
    for (std::map<MSTrafficLightLogic*, GUITrafficLightLogicWrapper*>::const_iterator i = myLogics2Wrapper.begin(); i != myLogics2Wrapper.end(); ++i) {
        std::string sid = (*i).second->getMicrosimID();
        if (find(ids.begin(), ids.end(), sid) == ids.end()) {
            ret.push_back((*i).second->getGlID());
            ids.push_back(sid);
        }
    }
    return ret;
}


void
GUINet::initGUIStructures() {
    // initialise detector storage for gui
    const std::vector<SumoXMLTag> types = myDetectorControl->getAvailableTypes();
    for (std::vector<SumoXMLTag>::const_iterator i = types.begin(); i != types.end(); ++i) {
        const std::map<std::string, MSDetectorFileOutput*>& dets = myDetectorControl->getTypedDetectors(*i).getMyMap();
        for (std::map<std::string, MSDetectorFileOutput*>::const_iterator j = dets.begin(); j != dets.end(); ++j) {
            GUIDetectorWrapper* wrapper = (*j).second->buildDetectorGUIRepresentation();
            if (wrapper != 0) {
                myDetectorDict.push_back(wrapper);
                myGrid.addAdditionalGLObject(wrapper);
            }
        }
    }
    // initialise the tl-map
    initTLMap();
    // initialise edge storage for gui
    GUIEdge::fill(myEdgeWrapper);
    // initialise junction storage for gui
    int size = myJunctions->size();
    myJunctionWrapper.reserve(size);
    const std::map<std::string, MSJunction*>& junctions = myJunctions->getMyMap();
    for (std::map<std::string, MSJunction*>::const_iterator i = junctions.begin(); i != junctions.end(); ++i) {
        myJunctionWrapper.push_back(new GUIJunctionWrapper(*(*i).second));
    }
    // build the visualization tree
    for (std::vector<GUIEdge*>::iterator i = myEdgeWrapper.begin(); i != myEdgeWrapper.end(); ++i) {
        GUIEdge* edge = *i;
        Boundary b;
        const std::vector<MSLane*>& lanes = edge->getLanes();
        for (std::vector<MSLane*>::const_iterator j = lanes.begin(); j != lanes.end(); ++j) {
            b.add((*j)->getShape().getBoxBoundary());
        }
        // make sure persons are always drawn and selectable since they depend on their edge being drawn
        b.grow(MSPModel::SIDEWALK_OFFSET + 1);
        const float cmin[2] = { (float)b.xmin(), (float)b.ymin() };
        const float cmax[2] = { (float)b.xmax(), (float)b.ymax() };
        myGrid.Insert(cmin, cmax, edge);
        myBoundary.add(b);
        if (myBoundary.getWidth() > 10e16 || myBoundary.getHeight() > 10e16) {
            throw ProcessError("Network size exceeds 1 Lightyear. Please reconsider your inputs.\n");
        }
    }
    for (std::vector<GUIJunctionWrapper*>::iterator i = myJunctionWrapper.begin(); i != myJunctionWrapper.end(); ++i) {
        GUIJunctionWrapper* junction = *i;
        Boundary b = junction->getBoundary();
        b.grow(2.);
        const float cmin[2] = { (float)b.xmin(), (float)b.ymin() };
        const float cmax[2] = { (float)b.xmax(), (float)b.ymax() };
        myGrid.Insert(cmin, cmax, junction);
        myBoundary.add(b);
    }
    myGrid.add(myBoundary);
}


int
GUINet::getWholeDuration() const {
    return myLastSimDuration +/*myLastVisDuration+*/myLastIdleDuration;
}


int
GUINet::getSimDuration() const {
    return myLastSimDuration;
}

/*
int
GUINet::getVisDuration() const
{
    return myLastVisDuration;
}
*/


SUMOReal
GUINet::getRTFactor() const {
    if (myLastSimDuration == 0) {
        return -1;
    }
    return (SUMOReal) 1000. / (SUMOReal) myLastSimDuration;
}


SUMOReal
GUINet::getUPS() const {
    if (myLastSimDuration == 0) {
        return -1;
    }
    return (SUMOReal) myLastVehicleMovementCount / (SUMOReal) myLastSimDuration * (SUMOReal) 1000.;
}


SUMOReal
GUINet::getMeanRTFactor(int duration) const {
    if (myOverallSimDuration == 0) {
        return -1;
    }
    return ((SUMOReal)(duration) * (SUMOReal) 1000. / (SUMOReal)myOverallSimDuration);
}


SUMOReal
GUINet::getMeanUPS() const {
    if (myOverallSimDuration == 0) {
        return -1;
    }
    return ((SUMOReal)myVehiclesMoved / (SUMOReal)myOverallSimDuration * (SUMOReal) 1000.);
}


int
GUINet::getIdleDuration() const {
    return myLastIdleDuration;
}


void
GUINet::setSimDuration(int val) {
    myLastSimDuration = val;
    myOverallSimDuration += val;
    myLastVehicleMovementCount = getVehicleControl().getRunningVehicleNo();
    myOverallVehicleCount += myLastVehicleMovementCount;
}

/*
void
GUINet::setVisDuration(int val)
{
    myLastVisDuration = val;
}
*/

void
GUINet::setIdleDuration(int val) {
    myLastIdleDuration = val;
}


GUIGLObjectPopupMenu*
GUINet::getPopUpMenu(GUIMainWindow& app,
                     GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUINet::getParameterWindow(GUIMainWindow& app,
                           GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 28);
    // add items
    ret->mkItem("loaded vehicles [#]", true,
                new FunctionBinding<MSVehicleControl, int>(&getVehicleControl(), &MSVehicleControl::getLoadedVehicleNo));
    ret->mkItem("insertion-backlogged vehicles [#]", true,
                new FunctionBinding<MSInsertionControl, int>(&getInsertionControl(), &MSInsertionControl::getWaitingVehicleNo));
    ret->mkItem("departed vehicles [#]", true,
                new FunctionBinding<MSVehicleControl, int>(&getVehicleControl(), &MSVehicleControl::getDepartedVehicleNo));
    ret->mkItem("running vehicles [#]", true,
                new FunctionBinding<MSVehicleControl, int>(&getVehicleControl(), &MSVehicleControl::getRunningVehicleNo));
    ret->mkItem("arrived vehicles [#]", true,
                new FunctionBinding<MSVehicleControl, int>(&getVehicleControl(), &MSVehicleControl::getEndedVehicleNo));
    ret->mkItem("collisions [#]", true,
                new FunctionBinding<MSVehicleControl, int>(&getVehicleControl(), &MSVehicleControl::getCollisionCount));
    ret->mkItem("teleports [#]", true,
                new FunctionBinding<MSVehicleControl, int>(&getVehicleControl(), &MSVehicleControl::getTeleportCount));
    if (myPersonControl != 0) {
        ret->mkItem("loaded persons [#]", true,
                    new FunctionBinding<MSTransportableControl, int>(&getPersonControl(), &MSTransportableControl::getLoadedNumber));
        ret->mkItem("running persons [#]", true,
                    new FunctionBinding<MSTransportableControl, int>(&getPersonControl(), &MSTransportableControl::getRunningNumber));
        ret->mkItem("jammed persons [#]", true,
                    new FunctionBinding<MSTransportableControl, int>(&getPersonControl(), &MSTransportableControl::getJammedNumber));
    }
    ret->mkItem("end time [s]", false, OptionsCont::getOptions().getString("end"));
    ret->mkItem("begin time [s]", false, OptionsCont::getOptions().getString("begin"));
//    ret->mkItem("time step [s]", true, new FunctionBinding<GUINet, SUMOTime>(this, &GUINet::getCurrentTimeStep));
    if (logSimulationDuration()) {
        ret->mkItem("step duration [ms]", true, new FunctionBinding<GUINet, int>(this, &GUINet::getWholeDuration));
        ret->mkItem("simulation duration [ms]", true, new FunctionBinding<GUINet, int>(this, &GUINet::getSimDuration));
        /*
        ret->mkItem("visualisation duration [ms]", true,
            new CastingFunctionBinding<GUINet, SUMOReal, int>(
                &(getNet()), &GUINet::getVisDuration));
        */
        ret->mkItem("idle duration [ms]", true, new FunctionBinding<GUINet, int>(this, &GUINet::getIdleDuration));
        ret->mkItem("duration factor []", true, new FunctionBinding<GUINet, SUMOReal>(this, &GUINet::getRTFactor));
        /*
        ret->mkItem("mean duration factor []", true,
            new FuncBinding_IntParam<GUINet, SUMOReal>(
                &(getNet()), &GUINet::getMeanRTFactor), 1);
                */
        ret->mkItem("ups [#]", true, new FunctionBinding<GUINet, SUMOReal>(this, &GUINet::getUPS));
        ret->mkItem("mean ups [#]", true, new FunctionBinding<GUINet, SUMOReal>(this, &GUINet::getMeanUPS));
        if (OptionsCont::getOptions().getBool("duration-log.statistics")) {
            ret->mkItem("avg. trip length [m]", true, new FunctionBinding<GUINet, SUMOReal>(this, &GUINet::getAvgRouteLength));
            ret->mkItem("avg. trip duration [s]", true, new FunctionBinding<GUINet, SUMOReal>(this, &GUINet::getAvgDuration));
            ret->mkItem("avg. trip waiting time [s]", true, new FunctionBinding<GUINet, SUMOReal>(this, &GUINet::getAvgWaitingTime));
            ret->mkItem("avg. trip time loss [s]", true, new FunctionBinding<GUINet, SUMOReal>(this, &GUINet::getAvgTimeLoss));
            ret->mkItem("avg. trip depart delay [s]", true, new FunctionBinding<GUINet, SUMOReal>(this, &GUINet::getAvgDepartDelay));
        }
    }
    ret->mkItem("nodes [#]", false, (int)myJunctions->size());
    ret->mkItem("edges [#]", false, (int)GUIEdge::getIDs(false).size());
    ret->mkItem("total edge length [km]", false, GUIEdge::getTotalLength(false, false) / 1000);
    ret->mkItem("total lane length [km]", false, GUIEdge::getTotalLength(false, true) / 1000);
    ret->mkItem("network version ", false, toString(myVersion));

    // close building
    ret->closeBuilding();
    return ret;
}


void
GUINet::drawGL(const GUIVisualizationSettings& /*s*/) const {
}

Boundary
GUINet::getCenteringBoundary() const {
    return getBoundary();
}


GUINet*
GUINet::getGUIInstance() {
    GUINet* net = dynamic_cast<GUINet*>(MSNet::getInstance());
    if (net != 0) {
        return net;
    }
    throw ProcessError("A gui-network was not yet constructed.");
}


GUIVehicleControl*
GUINet::getGUIVehicleControl() {
    return dynamic_cast<GUIVehicleControl*>(myVehicleControl);
}


void
GUINet::lock() {
    myLock.lock();
}


void
GUINet::unlock() {
    myLock.unlock();
}

GUIMEVehicleControl*
GUINet::getGUIMEVehicleControl() {
    return dynamic_cast<GUIMEVehicleControl*>(myVehicleControl);
}


#ifdef HAVE_OSG
void
GUINet::updateColor(const GUIVisualizationSettings& s) {
    for (std::vector<GUIEdge*>::const_iterator i = myEdgeWrapper.begin(); i != myEdgeWrapper.end(); ++i) {
        if ((*i)->getPurpose() != MSEdge::EDGEFUNCTION_INTERNAL) {
            const std::vector<MSLane*>& lanes = (*i)->getLanes();
            for (std::vector<MSLane*>::const_iterator j = lanes.begin(); j != lanes.end(); ++j) {
                static_cast<GUILane*>(*j)->updateColor(s);
            }
        }
    }
    for (std::vector<GUIJunctionWrapper*>::iterator i = myJunctionWrapper.begin(); i != myJunctionWrapper.end(); ++i) {
        (*i)->updateColor(s);
    }
}
#endif

/****************************************************************************/

