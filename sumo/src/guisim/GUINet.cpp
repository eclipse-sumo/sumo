/****************************************************************************/
/// @file    GUINet.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A MSNet extended by some values for usage within the gui
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#pragma warning(disable: 4355)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
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
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gfx/RGBColor.h>
#include "GUINetWrapper.h"
#include <guisim/guilogging/GLObjectValuePassConnector.h>
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
#include "GUIHelpingJunction.h"
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include "GUIGridBuilder.h"

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
GUINet::GUINet(SUMOTime startTimestep, SUMOTime stopTimestep,
               MSVehicleControl *vc,
               SUMOReal tooSlowRTF, bool logExecTime)
        : MSNet(startTimestep, stopTimestep, vc, tooSlowRTF, logExecTime),
        _grid(10, 10),
        myWrapper(new GUINetWrapper(gIDStorage, *this)),
        myLastSimDuration(0), /*myLastVisDuration(0),*/ myLastIdleDuration(0),
        myLastVehicleMovementCount(0), myOverallVehicleCount(0), myOverallSimDuration(0)
{}


GUINet::~GUINet()
{
    gIDStorage.clear();
    // delete allocated wrappers
    // of junctions
    for (std::vector<GUIJunctionWrapper*>::iterator i1=myJunctionWrapper.begin(); i1!=myJunctionWrapper.end(); i1++) {
        delete(*i1);
    }
    // of addition structures
    GUIGlObject_AbstractAdd::clearDictionary();
    // of tl-logics
    {
        for (Logics2WrapperMap::iterator i3=myLogics2Wrapper.begin(); i3!=myLogics2Wrapper.end(); i3++) {
            delete(*i3).second;
        }
    }
    {
        std::map<std::string, GUIDetectorWrapper*>::iterator i;
        for (i=myDetectorDict.begin(); i!=myDetectorDict.end(); ++i) {
            delete(*i).second;
        }
    }
    // of the network itself
    delete myWrapper;
}


const Boundary &
GUINet::getBoundary() const
{
    return _boundary;
}


void
GUINet::initDetectors()
{
    // e2-detectors
    {
        MSDetectorControl::E2Vect loopVec2 = myDetectorControl->getE2Vector();
        for (MSDetectorControl::E2Vect::const_iterator i2=loopVec2.begin(); i2!=loopVec2.end(); i2++) {
            const MSLane *lane = (*i2)->getLane();
            const GUIEdge * const edge = static_cast<const GUIEdge * const>(lane->getEdge());
            // build the wrapper
            if ((*i2)->getUsageType()==DU_SUMO_INTERNAL
                    ||
                    (*i2)->getUsageType()==DU_TL_CONTROL) {
                continue;
            }
            GUIDetectorWrapper *wrapper =
                static_cast<GUI_E2_ZS_Collector*>(*i2)->buildDetectorWrapper(
                    gIDStorage, edge->getLaneGeometry(lane));
            // add to dictionary
            myDetectorDict[wrapper->microsimID()] = wrapper;
        }
    }
    // e2 over lanes -detectors
    {
        MSDetectorControl::E2ZSOLVect loopVec3 = myDetectorControl->getE2OLVector();
        for (MSDetectorControl::E2ZSOLVect::const_iterator i3=loopVec3.begin(); i3!=loopVec3.end(); i3++) {
            // build the wrapper
            GUIDetectorWrapper *wrapper =
                static_cast<GUI_E2_ZS_CollectorOverLanes*>(*i3)->buildDetectorWrapper(
                    gIDStorage);
            // add to dictionary
            myDetectorDict[wrapper->microsimID()] = wrapper;
        }
    }
    // e1-detectors
    {
        MSDetectorControl::LoopVect loopVec = myDetectorControl->getLoopVector();
        for (MSDetectorControl::LoopVect::const_iterator i=loopVec.begin(); i!=loopVec.end(); i++) {
            const MSLane *lane = (*i)->getLane();
            const GUIEdge * const edge = static_cast<const GUIEdge * const>(lane->getEdge());
            // build the wrapper
            GUIDetectorWrapper *wrapper =
                static_cast<GUIInductLoop*>(*i)->buildDetectorWrapper(
                    gIDStorage, edge->getLaneGeometry(lane));
            // add to dictionary
            myDetectorDict[wrapper->microsimID()] = wrapper;
        }
    }
    // e3-detectors
    {
        MSDetectorControl::E3Vect loopVec4 = myDetectorControl->getE3Vector();
        for (MSDetectorControl::E3Vect::const_iterator i4=loopVec4.begin(); i4!=loopVec4.end(); i4++) {
            // build the wrapper
            GUIDetectorWrapper *wrapper =
                static_cast<GUIE3Collector*>(*i4)->buildDetectorWrapper(gIDStorage);
            // add to dictionary
            myDetectorDict[wrapper->microsimID()] = wrapper;
        }
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
    MSJunction *junction = MSJunction::dictionary(name);
    return Position2D(junction->getPosition());
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
    MSUpdateEachTimestepContainer<MSUpdateEachTimestep<GLObjectValuePassConnector<CompletePhaseDef> > >::getInstance()->updateAll();
}


std::vector<size_t>
GUINet::getJunctionIDs() const
{
    std::vector<size_t> ret;
    for (std::vector<GUIJunctionWrapper*>::const_iterator i=myJunctionWrapper.begin(); i!=myJunctionWrapper.end(); i++) {
        ret.push_back((*i)->getGlID());
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
    GUIHelpingJunction::fill(myJunctionWrapper, gIDStorage);
    // build the grid
    GUIGridBuilder b(*this, _grid);
    b.build();
    // get the boundary
    _boundary = _grid.getBoundary();
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
GUINet::buildRouteLoader(const std::string &file, int incDUABase, int incDUAStage)
{
    // return a new build route loader
    //  the handler is
    //  a) not adding the vehicles directly
    //  b) using colors
    return new MSRouteLoader(*this, new GUIRouteHandler(file, *myVehicleControl, false, incDUABase, incDUAStage));
}



/****************************************************************************/

