/****************************************************************************/
/// @file    GUINet.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A MSNet extended by some values for usage within the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#pragma warning(disable: 4355)
#endif


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
#include <microsim/MSNet.h>
#include <microsim/MSJunction.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEmitControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/MSJunctionControl.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/common/RGBColor.h>
#include "GUINetWrapper.h"
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
#include <guisim/GUIRouteHandler.h>
#include <gui/GUIGlobals.h>
#include <microsim/MSUpdateEachTimestepContainer.h>
#include <microsim/MSRouteLoader.h>
#include "GUIVehicle.h"
#include "GUINet.h"
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/gui/globjects/GUIPolygon2D.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
GUINet::GUINet(MSVehicleControl *vc, MSEventControl *beginOfTimestepEvents,
               MSEventControl *endOfTimestepEvents, MSEventControl *emissionEvents)
        : MSNet(vc, beginOfTimestepEvents, endOfTimestepEvents, emissionEvents),
        myGrid(new SUMORTree(&GUIGlObject::drawGL)),
        myWrapper(new GUINetWrapper(gIDStorage, *this)),
        myLastSimDuration(0), /*myLastVisDuration(0),*/ myLastIdleDuration(0),
        myLastVehicleMovementCount(0), myOverallVehicleCount(0), myOverallSimDuration(0)
{
    // as it is possible to show all vehicle routes, we have to store them... (bug [ 2519761 ])
    MSCORN::setWished(MSCORN::CORN_VEH_SAVEREROUTING);
}


GUINet::~GUINet() throw()
{
    gIDStorage.clear();
    // delete allocated wrappers
    //  of junctions
    for (std::vector<GUIJunctionWrapper*>::iterator i1=myJunctionWrapper.begin(); i1!=myJunctionWrapper.end(); i1++) {
        delete(*i1);
    }
    //  of addition structures
    GUIGlObject_AbstractAdd::clearDictionary();
    //  of tl-logics
    for (Logics2WrapperMap::iterator i3=myLogics2Wrapper.begin(); i3!=myLogics2Wrapper.end(); i3++) {
        delete(*i3).second;
    }
    //  of detectors
    for (map<string, GUIDetectorWrapper*>::iterator i=myDetectorDict.begin(); i!=myDetectorDict.end(); ++i) {
        delete(*i).second;
    }
    // the visualization tree
    delete myGrid;
    // of the network itself
    delete myWrapper;
}


const Boundary &
GUINet::getBoundary() const
{
    return myBoundary;
}


void
GUINet::initDetectors()
{
    // e2-detectors
    const map<string, MSE2Collector*> &e2 = myDetectorControl->getE2Detectors().getMyMap();
    for (map<string, MSE2Collector*>::const_iterator i2=e2.begin(); i2!=e2.end(); i2++) {
        MSE2Collector *const e2i = (*i2).second;
        const MSLane *lane = e2i->getLane();
        const GUIEdge * const edge = static_cast<const GUIEdge * const>(lane->getEdge());
        /*
        // build the wrapper
            if ((*i2)->getUsageType()==DU_SUMO_INTERNAL
                    ||
                    (*i2)->getUsageType()==DU_TL_CONTROL) {
                continue;
            }
            */
        GUIDetectorWrapper *wrapper =
            static_cast<GUI_E2_ZS_Collector*>(e2i)->buildDetectorWrapper(
                gIDStorage, edge->getLaneGeometry(lane));
        // add to dictionary
        myDetectorDict[wrapper->getMicrosimID()] = wrapper;
    }
    // e2 over lanes -detectors
    const map<string, MS_E2_ZS_CollectorOverLanes*> &e2ol = myDetectorControl->getE2OLDetectors().getMyMap();
    for (map<string, MS_E2_ZS_CollectorOverLanes*>::const_iterator i2=e2ol.begin(); i2!=e2ol.end(); i2++) {
        MS_E2_ZS_CollectorOverLanes * const e2oli = (*i2).second;
        // build the wrapper
        GUIDetectorWrapper *wrapper =
            static_cast<GUI_E2_ZS_CollectorOverLanes*>(e2oli)->buildDetectorWrapper(
                gIDStorage);
        // add to dictionary
        myDetectorDict[wrapper->getMicrosimID()] = wrapper;
    }
    // induction loops
    const map<string, MSInductLoop*> &e1 = myDetectorControl->getInductLoops().getMyMap();
    for (map<string, MSInductLoop*>::const_iterator i2=e1.begin(); i2!=e1.end(); i2++) {
        MSInductLoop *const e1i = (*i2).second;
        const MSLane *lane = e1i->getLane();
        const GUIEdge * const edge = static_cast<const GUIEdge * const>(lane->getEdge());
        // build the wrapper
        GUIDetectorWrapper *wrapper =
            static_cast<GUIInductLoop*>(e1i)->buildDetectorWrapper(
                gIDStorage, edge->getLaneGeometry(lane));
        // add to dictionary
        myDetectorDict[wrapper->getMicrosimID()] = wrapper;
    }
    // e3-detectors
    const map<string, MSE3Collector*> &e3 = myDetectorControl->getE3Detectors().getMyMap();
    for (map<string, MSE3Collector*>::const_iterator i2=e3.begin(); i2!=e3.end(); i2++) {
        MSE3Collector *const e3i = (*i2).second;
        // build the wrapper
        GUIDetectorWrapper *wrapper =
            static_cast<GUIE3Collector*>(e3i)->buildDetectorWrapper(gIDStorage);
        // add to dictionary
        myDetectorDict[wrapper->getMicrosimID()] = wrapper;
    }
}


void
GUINet::initTLMap()
{
    // get the list of loaded tl-logics
    const vector<MSTrafficLightLogic*> &logics = getTLSControl().getAllLogics();
    // allocate storage for the wrappers
    myTLLogicWrappers.reserve(logics.size());
    // go through the logics
    for (vector<MSTrafficLightLogic*>::const_iterator i=logics.begin(); i!=logics.end(); ++i) {
        // get the logic
        MSTrafficLightLogic *tll = (*i);
        // get the links
        const MSTrafficLightLogic::LinkVectorVector &links = tll->getLinks();
        if (links.size()==0) {
            continue;
        }
        // build the wrapper
        GUITrafficLightLogicWrapper *tllw =
            new GUITrafficLightLogicWrapper(gIDStorage, *myLogics, *tll);
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
GUINet::getJunctionPosition(const std::string &name) const
{
    // !!! no check for existance!
    return myJunctions->get(name)->getPosition();
}


bool
GUINet::vehicleExists(const std::string &name) const
{
    return myVehicleControl->getVehicle(name)!=0;
}


Boundary
GUINet::getEdgeBoundary(const std::string &name) const
{
    GUIEdge *edge = static_cast<GUIEdge*>(MSEdge::dictionary(name));
    return edge->getBoundary();
}


GUINetWrapper * const
GUINet::getWrapper() const
{
    return myWrapper;
}


unsigned int
GUINet::getLinkTLID(MSLink *link) const
{
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
GUINet::getLinkTLIndex(MSLink *link) const
{
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
GUINet::guiSimulationStep()
{
    MSUpdateEachTimestepContainer<MSUpdateEachTimestep<GLObjectValuePassConnector<SUMOReal> > >::getInstance()->updateAll();
    MSUpdateEachTimestepContainer<MSUpdateEachTimestep<GLObjectValuePassConnector<std::pair<SUMOTime, MSPhaseDefinition> > > >::getInstance()->updateAll();
}


std::vector<GLuint>
GUINet::getJunctionIDs() const
{
    std::vector<GLuint> ret;
    for (std::vector<GUIJunctionWrapper*>::const_iterator i=myJunctionWrapper.begin(); i!=myJunctionWrapper.end(); ++i) {
        ret.push_back((*i)->getGlID());
    }
    return ret;
}


std::vector<GLuint>
GUINet::getTLSIDs() const
{
    std::vector<GLuint> ret;
    std::vector<string> ids;
    for (std::map<MSTrafficLightLogic*, GUITrafficLightLogicWrapper*>::const_iterator i=myLogics2Wrapper.begin(); i!=myLogics2Wrapper.end(); ++i) {
        size_t nid = (*i).second->getGlID();
        string sid = (*i).second->getMicrosimID();
        if (find(ids.begin(), ids.end(), sid)==ids.end()) {
            ret.push_back(nid);
            ids.push_back(sid);
        }
    }
    return ret;
}


std::vector<GLuint>
GUINet::getShapeIDs() const
{
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
GUINet::initGUIStructures()
{
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
        GUIJunctionWrapper *wrapper = ((*i).second)->buildJunctionWrapper(gIDStorage);
        if (wrapper!=0) {
            myJunctionWrapper.push_back(wrapper);
        }
    }
    // build the visualization tree
    float *cmin = new float[2];
    float *cmax = new float[2];
    for (vector<GUIEdge*>::iterator i=myEdgeWrapper.begin(); i!=myEdgeWrapper.end(); ++i) {
        GUIEdge *edge = *i;
        Boundary b;
        for (size_t j=0; j<edge->nLanes(); ++j) {
            GUILaneWrapper &lane = edge->getLaneGeometry(j);
            b.add(lane.getShape().getBoxBoundary());
        }
        b.grow(2.);
        cmin[0] = b.xmin();
        cmin[1] = b.ymin();
        cmax[0] = b.xmax();
        cmax[1] = b.ymax();
        myGrid->Insert(cmin, cmax, edge);
        myBoundary.add(b);
    }
    for (vector<GUIJunctionWrapper*>::iterator i=myJunctionWrapper.begin(); i!=myJunctionWrapper.end(); ++i) {
        GUIJunctionWrapper *junction = *i;
        if (junction->getShape().size()>0) {
            Boundary b = junction->getShape().getBoxBoundary();
            b.grow(2.);
            cmin[0] = b.xmin();
            cmin[1] = b.ymin();
            cmax[0] = b.xmax();
            cmax[1] = b.ymax();
            myGrid->Insert(cmin, cmax, junction);
            myBoundary.add(b);
        }
    }
    const vector<GUIGlObject_AbstractAdd*> &a = GUIGlObject_AbstractAdd::getObjectList();
    for (vector<GUIGlObject_AbstractAdd*>::const_iterator i=a.begin(); i!=a.end(); ++i) {
        GUIGlObject_AbstractAdd *o = *i;
        Boundary b = o->getCenteringBoundary();
        cmin[0] = b.xmin();
        cmin[1] = b.ymin();
        cmax[0] = b.xmax();
        cmax[1] = b.ymax();
        myGrid->Insert(cmin, cmax, o);
    }
    delete[] cmin;
    delete[] cmax;
    myGrid->add(myBoundary);
}


int
GUINet::getWholeDuration() const
{
    return myLastSimDuration+/*myLastVisDuration+*/myLastIdleDuration;
}


int
GUINet::getSimDuration() const
{
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
GUINet::getRTFactor() const
{
    if (myLastSimDuration==0) {
        return -1;
    }
    return (SUMOReal) 1000. / (SUMOReal) myLastSimDuration;
}


SUMOReal
GUINet::getUPS() const
{
    if (myLastSimDuration==0) {
        return -1;
    }
    return (SUMOReal) myLastVehicleMovementCount / (SUMOReal) myLastSimDuration *(SUMOReal) 1000.;
}


SUMOReal
GUINet::getMeanRTFactor(int duration) const
{
    if (myOverallSimDuration==0) {
        return -1;
    }
    return ((SUMOReal)(duration)*(SUMOReal) 1000./(SUMOReal)myOverallSimDuration);
}


SUMOReal
GUINet::getMeanUPS() const
{
    if (myOverallSimDuration==0) {
        return -1;
    }
    return ((SUMOReal)myVehiclesMoved / (SUMOReal)myOverallSimDuration *(SUMOReal) 1000.);
}


int
GUINet::getIdleDuration() const
{
    return myLastIdleDuration;
}


void
GUINet::setSimDuration(int val)
{
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
GUINet::setIdleDuration(int val)
{
    myLastIdleDuration = val;
}


MSRouteLoader *
GUINet::buildRouteLoader(const std::string &file)
{
    // return a new build route loader
    //  the handler is
    //  a) not adding the vehicles directly
    //  b) using colors
    return new MSRouteLoader(*this, new GUIRouteHandler(file, false));
}




/****************************************************************************/

