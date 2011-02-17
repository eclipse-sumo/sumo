/****************************************************************************/
/// @file    GUINet.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A MSNet extended by some values for usage within the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include <utility>
#include <set>
#include <vector>
#include <map>
#include <microsim/MSNet.h>
#include <microsim/MSJunction.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/MSJunctionControl.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/common/RGBColor.h>
#include <guisim/GLObjectValuePassConnector.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILaneSpeedTrigger.h>
#include <guisim/GUIDetectorWrapper.h>
#include <guisim/GUIInductLoop.h>
#include <guisim/GUI_E2_ZS_Collector.h>
#include <guisim/GUI_E2_ZS_CollectorOverLanes.h>
#include <guisim/GUIE3Collector.h>
#include <guisim/GUITrafficLightLogicWrapper.h>
#include <guisim/GUIJunctionWrapper.h>
#include <guisim/GUIVehicleControl.h>
#include <gui/GUIGlobals.h>
#include <microsim/MSRouteLoader.h>
#include "GUIVehicle.h"
#include "GUINet.h"
#include "GUIShapeContainer.h"
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIPolygon2D.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/common/StringUtils.h>
#include "GLObjectValuePassConnector.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// definition of static variables used for visualisation of objects' values
// ===========================================================================
template std::vector< GLObjectValuePassConnector<SUMOReal>* > GLObjectValuePassConnector<SUMOReal>::myContainer;
template MFXMutex GLObjectValuePassConnector<SUMOReal>::myLock;

template std::vector< GLObjectValuePassConnector<std::pair<int,class MSPhaseDefinition> >* > GLObjectValuePassConnector<std::pair<int,class MSPhaseDefinition> >::myContainer;
template MFXMutex GLObjectValuePassConnector<std::pair<int,class MSPhaseDefinition> >::myLock;


// ===========================================================================
// member method definitions
// ===========================================================================
GUINet::GUINet(MSVehicleControl *vc, MSEventControl *beginOfTimestepEvents,
               MSEventControl *endOfTimestepEvents, MSEventControl *insertionEvents) throw(ProcessError)
        : MSNet(vc, beginOfTimestepEvents, endOfTimestepEvents, insertionEvents, new GUIShapeContainer(myGrid)),
        GUIGlObject(GUIGlObjectStorage::gIDStorage, "network"),
        myLastSimDuration(0), /*myLastVisDuration(0),*/ myLastIdleDuration(0),
        myLastVehicleMovementCount(0), myOverallVehicleCount(0), myOverallSimDuration(0) {
    GUIGlObjectStorage::gIDStorage.setNetObject(this);
}


GUINet::~GUINet() throw() {
    GUIGlObjectStorage::gIDStorage.clear();
    // delete allocated wrappers
    //  of junctions
    for (std::vector<GUIJunctionWrapper*>::iterator i1=myJunctionWrapper.begin(); i1!=myJunctionWrapper.end(); i1++) {
        delete(*i1);
    }
    //  of additional structures
    GUIGlObject_AbstractAdd::clearDictionary();
    //  of tl-logics
    for (Logics2WrapperMap::iterator i3=myLogics2Wrapper.begin(); i3!=myLogics2Wrapper.end(); i3++) {
        delete(*i3).second;
    }
    //  of detectors
    for (std::map<std::string, GUIDetectorWrapper*>::iterator i=myDetectorDict.begin(); i!=myDetectorDict.end(); ++i) {
        delete(*i).second;
    }
}


const Boundary &
GUINet::getBoundary() const {
    return myBoundary;
}


void
GUINet::initDetectors() {
    // e2-detectors
    const std::map<std::string, MSE2Collector*> &e2 = myDetectorControl->getE2Detectors().getMyMap();
    for (std::map<std::string, MSE2Collector*>::const_iterator i2=e2.begin(); i2!=e2.end(); i2++) {
        MSE2Collector *const e2i = (*i2).second;
        const MSLane *lane = e2i->getLane();
        GUIEdge &edge = static_cast<GUIEdge&>(lane->getEdge());
        /*
        // build the wrapper
            if ((*i2)->getUsageType()==DU_SUMO_INTERNAL
                    ||
                    (*i2)->getUsageType()==DU_TL_CONTROL) {
                continue;
            }
            */
        GUIDetectorWrapper *wrapper = static_cast<GUI_E2_ZS_Collector*>(e2i)->buildDetectorWrapper(GUIGlObjectStorage::gIDStorage, edge.getLaneGeometry(lane));
        // add to dictionary
        myDetectorDict[wrapper->getMicrosimID()] = wrapper;
        // add to visualisation
        myGrid.addAdditionalGLObject(wrapper);
    }
    // e2 over lanes -detectors
    const std::map<std::string, MS_E2_ZS_CollectorOverLanes*> &e2ol = myDetectorControl->getE2OLDetectors().getMyMap();
    for (std::map<std::string, MS_E2_ZS_CollectorOverLanes*>::const_iterator i2=e2ol.begin(); i2!=e2ol.end(); i2++) {
        MS_E2_ZS_CollectorOverLanes * const e2oli = (*i2).second;
        GUIDetectorWrapper *wrapper = static_cast<GUI_E2_ZS_CollectorOverLanes*>(e2oli)->buildDetectorWrapper(GUIGlObjectStorage::gIDStorage);
        myDetectorDict[wrapper->getMicrosimID()] = wrapper;
        myGrid.addAdditionalGLObject(wrapper);
    }
    // induction loops
    const std::map<std::string, MSInductLoop*> &e1 = myDetectorControl->getInductLoops().getMyMap();
    for (std::map<std::string, MSInductLoop*>::const_iterator i2=e1.begin(); i2!=e1.end(); i2++) {
        MSInductLoop *const e1i = (*i2).second;
        const MSLane *lane = e1i->getLane();
        GUIEdge &edge = static_cast<GUIEdge&>(lane->getEdge());
        GUIDetectorWrapper *wrapper = static_cast<GUIInductLoop*>(e1i)->buildDetectorWrapper(GUIGlObjectStorage::gIDStorage, edge.getLaneGeometry(lane));
        myDetectorDict[wrapper->getMicrosimID()] = wrapper;
        myGrid.addAdditionalGLObject(wrapper);
    }
    // e3-detectors
    const std::map<std::string, MSE3Collector*> &e3 = myDetectorControl->getE3Detectors().getMyMap();
    for (std::map<std::string, MSE3Collector*>::const_iterator i2=e3.begin(); i2!=e3.end(); i2++) {
        MSE3Collector *const e3i = (*i2).second;
        GUIDetectorWrapper *wrapper = static_cast<GUIE3Collector*>(e3i)->buildDetectorWrapper(GUIGlObjectStorage::gIDStorage);
        myDetectorDict[wrapper->getMicrosimID()] = wrapper;
        myGrid.addAdditionalGLObject(wrapper);
    }
}


void
GUINet::initTLMap() {
    // get the list of loaded tl-logics
    const std::vector<MSTrafficLightLogic*> &logics = getTLSControl().getAllLogics();
    // allocate storage for the wrappers
    myTLLogicWrappers.reserve(logics.size());
    // go through the logics
    for (std::vector<MSTrafficLightLogic*>::const_iterator i=logics.begin(); i!=logics.end(); ++i) {
        // get the logic
        MSTrafficLightLogic *tll = (*i);
        // get the links
        const MSTrafficLightLogic::LinkVectorVector &links = tll->getLinks();
        if (links.size()==0) {
            continue;
        }
        // build the wrapper
        GUITrafficLightLogicWrapper *tllw =
            new GUITrafficLightLogicWrapper(GUIGlObjectStorage::gIDStorage, *myLogics, *tll);
        // build the association link->wrapper
        MSTrafficLightLogic::LinkVectorVector::const_iterator j;
        for (j=links.begin(); j!=links.end(); j++) {
            MSTrafficLightLogic::LinkVector::const_iterator j2;
            for (j2=(*j).begin(); j2!=(*j).end(); j2++) {
                myLinks2Logic[*j2] = tll->getID();
            }
        }
        myLogics2Wrapper[tll] = tllw;
    }
}


Position2D
GUINet::getJunctionPosition(const std::string &name) const {
    // !!! no check for existance!
    return myJunctions->get(name)->getPosition();
}


bool
GUINet::vehicleExists(const std::string &name) const {
    return myVehicleControl->getVehicle(name)!=0;
}


Boundary
GUINet::getEdgeBoundary(const std::string &name) const {
    GUIEdge *edge = static_cast<GUIEdge*>(MSEdge::dictionary(name));
    return edge->getBoundary();
}


unsigned int
GUINet::getLinkTLID(MSLink *link) const {
    Links2LogicMap::const_iterator i = myLinks2Logic.find(link);
    if (i==myLinks2Logic.end()) {
        return -1;
    }
    if (myLogics2Wrapper.find(myLogics->getActive((*i).second))==myLogics2Wrapper.end()) {
        return -1;
    }
    return myLogics2Wrapper.find(myLogics->getActive((*i).second))->second->getGlID();
}


int
GUINet::getLinkTLIndex(MSLink *link) const {
    Links2LogicMap::const_iterator i = myLinks2Logic.find(link);
    if (i==myLinks2Logic.end()) {
        return -1;
    }
    if (myLogics2Wrapper.find(myLogics->getActive((*i).second))==myLogics2Wrapper.end()) {
        return -1;
    }
    return myLogics2Wrapper.find(myLogics->getActive((*i).second))->second->getLinkIndex(link);
}


void
GUINet::guiSimulationStep() {
    GLObjectValuePassConnector<SUMOReal>::updateAll();
    GLObjectValuePassConnector<std::pair<SUMOTime, MSPhaseDefinition> >::updateAll();
}


std::vector<GLuint>
GUINet::getJunctionIDs() const {
    std::vector<GLuint> ret;
    for (std::vector<GUIJunctionWrapper*>::const_iterator i=myJunctionWrapper.begin(); i!=myJunctionWrapper.end(); ++i) {
        ret.push_back((*i)->getGlID());
    }
    return ret;
}


std::vector<GLuint>
GUINet::getTLSIDs() const {
    std::vector<GLuint> ret;
    std::vector<std::string> ids;
    for (std::map<MSTrafficLightLogic*, GUITrafficLightLogicWrapper*>::const_iterator i=myLogics2Wrapper.begin(); i!=myLogics2Wrapper.end(); ++i) {
        std::string sid = (*i).second->getMicrosimID();
        if (find(ids.begin(), ids.end(), sid)==ids.end()) {
            ret.push_back((*i).second->getGlID());
            ids.push_back(sid);
        }
    }
    return ret;
}


std::vector<GLuint>
GUINet::getShapeIDs() const {
    std::vector<GLuint> ret;
    if (myShapeContainer!=0) {
        int minLayer = myShapeContainer->getMinLayer();
        int maxLayer = myShapeContainer->getMaxLayer();
        for (int j=minLayer; j<=maxLayer; ++j) {
            const std::map<std::string, Polygon2D*> &pol = myShapeContainer->getPolygonCont(j).getMyMap();
            for (std::map<std::string, Polygon2D*>::const_iterator i=pol.begin(); i!=pol.end(); ++i) {
                ret.push_back(static_cast<GUIPolygon2D*>((*i).second)->getGlID());
            }
            const std::map<std::string, PointOfInterest*> &poi = myShapeContainer->getPOICont(j).getMyMap();
            for (std::map<std::string, PointOfInterest*>::const_iterator i=poi.begin(); i!=poi.end(); ++i) {
                ret.push_back(static_cast<GUIPointOfInterest*>((*i).second)->getGlID());
            }
        }
    }
    return ret;
}


void
GUINet::initGUIStructures() {
    // initialise detector storage for gui
    initDetectors();
    // initialise the tl-map
    initTLMap();
    // initialise edge storage for gui
    GUIEdge::fill(myEdgeWrapper);
    // initialise junction storage for gui
    size_t size = myJunctions->size();
    myJunctionWrapper.reserve(size);
    const std::map<std::string, MSJunction*> &junctions = myJunctions->getMyMap();
    for (std::map<std::string, MSJunction*>::const_iterator i=junctions.begin(); i!=junctions.end(); ++i) {
        myJunctionWrapper.push_back(new GUIJunctionWrapper(GUIGlObjectStorage::gIDStorage, *(*i).second));
    }
    // build the visualization tree
    float *cmin = new float[2];
    float *cmax = new float[2];
    for (std::vector<GUIEdge*>::iterator i=myEdgeWrapper.begin(); i!=myEdgeWrapper.end(); ++i) {
        GUIEdge *edge = *i;
        Boundary b;
        const std::vector<MSLane*> &lanes = edge->getLanes();
        for (std::vector<MSLane*>::const_iterator j=lanes.begin(); j!=lanes.end(); ++j) {
            b.add((*j)->getShape().getBoxBoundary());
        }
        b.grow(2.);
        cmin[0] = b.xmin();
        cmin[1] = b.ymin();
        cmax[0] = b.xmax();
        cmax[1] = b.ymax();
        myGrid.Insert(cmin, cmax, edge);
        myBoundary.add(b);
    }
    for (std::vector<GUIJunctionWrapper*>::iterator i=myJunctionWrapper.begin(); i!=myJunctionWrapper.end(); ++i) {
        GUIJunctionWrapper *junction = *i;
        Boundary b = junction->getBoundary();
        b.grow(2.);
        cmin[0] = b.xmin();
        cmin[1] = b.ymin();
        cmax[0] = b.xmax();
        cmax[1] = b.ymax();
        myGrid.Insert(cmin, cmax, junction);
        myBoundary.add(b);
    }
    delete[] cmin;
    delete[] cmax;
    myGrid.add(myBoundary);
}


unsigned int
GUINet::getWholeDuration() const throw() {
    return myLastSimDuration+/*myLastVisDuration+*/myLastIdleDuration;
}


unsigned int
GUINet::getSimDuration() const throw() {
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
    if (myLastSimDuration==0) {
        return -1;
    }
    return (SUMOReal) 1000. / (SUMOReal) myLastSimDuration;
}


SUMOReal
GUINet::getUPS() const {
    if (myLastSimDuration==0) {
        return -1;
    }
    return (SUMOReal) myLastVehicleMovementCount / (SUMOReal) myLastSimDuration *(SUMOReal) 1000.;
}


SUMOReal
GUINet::getMeanRTFactor(int duration) const {
    if (myOverallSimDuration==0) {
        return -1;
    }
    return ((SUMOReal)(duration)*(SUMOReal) 1000./(SUMOReal)myOverallSimDuration);
}


SUMOReal
GUINet::getMeanUPS() const {
    if (myOverallSimDuration==0) {
        return -1;
    }
    return ((SUMOReal)myVehiclesMoved / (SUMOReal)myOverallSimDuration *(SUMOReal) 1000.);
}


unsigned int
GUINet::getIdleDuration() const throw() {
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


GUIGLObjectPopupMenu *
GUINet::getPopUpMenu(GUIMainWindow &app,
                     GUISUMOAbstractView &parent) throw() {
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUINet::getParameterWindow(GUIMainWindow &app,
                           GUISUMOAbstractView &) throw() {
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 13);
    // add items
    ret->mkItem("loaded vehicles [#]", true,
                new FunctionBinding<MSVehicleControl, unsigned int>(&getVehicleControl(), &MSVehicleControl::getLoadedVehicleNo));
    ret->mkItem("waiting vehicles [#]", true,
                new FunctionBinding<MSInsertionControl, unsigned int>(&getInsertionControl(), &MSInsertionControl::getWaitingVehicleNo));
    ret->mkItem("departed vehicles [#]", true,
                new FunctionBinding<MSVehicleControl, unsigned int>(&getVehicleControl(), &MSVehicleControl::getDepartedVehicleNo));
    ret->mkItem("running vehicles [#]", true,
                new FunctionBinding<MSVehicleControl, unsigned int>(&getVehicleControl(), &MSVehicleControl::getRunningVehicleNo));
    ret->mkItem("arrived vehicles [#]", true,
                new FunctionBinding<MSVehicleControl, unsigned int>(&getVehicleControl(), &MSVehicleControl::getEndedVehicleNo));
    ret->mkItem("end time [s]", false, OptionsCont::getOptions().getString("end"));
    ret->mkItem("begin time [s]", false, OptionsCont::getOptions().getString("begin"));
//    ret->mkItem("time step [s]", true, new FunctionBinding<GUINet, SUMOTime>(this, &GUINet::getCurrentTimeStep));
    if (logSimulationDuration()) {
        ret->mkItem("step duration [ms]", true, new FunctionBinding<GUINet, unsigned int>(this, &GUINet::getWholeDuration));
        ret->mkItem("simulation duration [ms]", true, new FunctionBinding<GUINet, unsigned int>(this, &GUINet::getSimDuration));
        /*
        ret->mkItem("visualisation duration [ms]", true,
            new CastingFunctionBinding<GUINet, SUMOReal, int>(
                &(getNet()), &GUINet::getVisDuration));
        */
        ret->mkItem("idle duration [ms]", true, new FunctionBinding<GUINet, unsigned int>(this, &GUINet::getIdleDuration));
        ret->mkItem("duration factor []", true, new FunctionBinding<GUINet, SUMOReal>(this, &GUINet::getRTFactor));
        /*
        ret->mkItem("mean duration factor []", true,
            new FuncBinding_IntParam<GUINet, SUMOReal>(
                &(getNet()), &GUINet::getMeanRTFactor), 1);
                */
        ret->mkItem("ups [#]", true, new FunctionBinding<GUINet, SUMOReal>(this, &GUINet::getUPS));
        ret->mkItem("mean ups [#]", true, new FunctionBinding<GUINet, SUMOReal>(this, &GUINet::getMeanUPS));
    }
    // close building
    ret->closeBuilding();
    return ret;
}


const std::string &
GUINet::getMicrosimID() const throw() {
    return StringUtils::emptyString;
}


void
GUINet::drawGL(const GUIVisualizationSettings&/*s*/) const throw() {
}

Boundary
GUINet::getCenteringBoundary() const throw() {
    return getBoundary();
}


/****************************************************************************/

