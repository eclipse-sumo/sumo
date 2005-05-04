/***************************************************************************
                          NLContainer.cpp
              Holds the builded structures or structures that
              hold these
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
     const char rcsid[] =
         "$Id$";
}
// $Log$
// Revision 1.29  2005/05/04 07:55:29  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.28  2005/02/01 10:07:26  dkrajzew
// performance computation added
//
// Revision 1.27  2004/11/23 10:12:45  dkrajzew
// new detectors usage applied
//
// Revision 1.26  2004/08/02 12:46:58  dkrajzew
// using OutputDevices instead of ostreams
//
// Revision 1.25  2004/07/02 09:37:31  dkrajzew
// work on class derivation (for online-routing mainly)
//
// Revision 1.23  2004/04/02 11:23:51  dkrajzew
// extended traffic lights are now no longer templates; MSNet now handles all simulation-wide output
//
// Revision 1.22  2004/02/18 05:32:51  dkrajzew
// missing pass of lane continuation to detector builder added
//
// Revision 1.21  2004/01/26 07:07:36  dkrajzew
// work on detectors: e3-detectors loading and visualisation;
//  variable offsets and lengths for lsa-detectors;
//  coupling of detectors to tl-logics; different
//  detector visualistaion in dependence to his controller
//
// Revision 1.20  2004/01/12 15:12:05  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.19  2004/01/12 14:36:20  dkrajzew
// removed some dead code; documentation added
//
// Revision 1.18  2003/12/04 13:18:23  dkrajzew
// handling of internal links added
//
// Revision 1.17  2003/11/18 14:23:57  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.16  2003/09/05 15:20:19  dkrajzew
// loading of internal links added
//
// Revision 1.15  2003/07/30 09:25:17  dkrajzew
// loading of directions and priorities of links implemented
//
// Revision 1.14  2003/07/21 18:07:44  roessel
// Adaptions due to new MSInductLoop.
//
// Revision 1.13  2003/07/07 08:35:10  dkrajzew
// changes due to loading of geometry applied from the gui-version
//  (no major drawbacks in loading speed)
//
// Revision 1.12  2003/06/18 11:18:04  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
// this handler puts it further to output instances.
// changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.11  2003/06/06 10:40:18  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.10  2003/06/05 11:52:26  dkrajzew
// class templates applied; documentation added
//
// Revision 1.8  2003/05/27 18:27:59  roessel
// Access EventControl via singleton-mechanism in ctor.
// Don't pass m_EventControl to MSNet::init in NLContainer::buildMSNet.
// MSNet::init will get it via the singleton-mechanism too.
//
// Revision 1.7  2003/05/21 15:18:21  dkrajzew
// yellow traffic lights implemented
//
// Revision 1.6  2003/03/17 14:24:30  dkrajzew
// windows eol removed
//
// Revision 1.5  2003/03/03 15:06:31  dkrajzew
// new import format applied; new detectors applied
//
// Revision 1.4  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.3  2002/10/21 09:52:57  dkrajzew
// support for route multireferencing added
//
// Revision 1.2  2002/10/17 10:34:48  dkrajzew
// possibility of retrival of the preinitialised net during loading
//  implemented for trigger handling
//
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition
//  parseable in one step
//
// Revision 1.9  2002/07/31 17:34:50  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.10  2002/07/30 15:18:59  croessel
// Changes because MSNet is singleton now.
//
// Revision 1.9  2002/07/22 12:44:32  dkrajzew
// Source loading structures added
//
// Revision 1.8  2002/06/11 14:39:26  dkrajzew
// windows eol removed
//
// Revision 1.7  2002/06/11 13:44:32  dkrajzew
// Windows eol removed
//
// Revision 1.6  2002/06/10 08:36:07  dkrajzew
// Conversion of strings generalized
//
// Revision 1.5  2002/06/07 14:39:57  dkrajzew
// errors occured while building larger nets and adaption of new netconverting
//  methods debugged
//
// Revision 1.4  2002/04/17 11:17:01  dkrajzew
// windows-newlines removed
//
// Revision 1.3  2002/04/16 06:05:03  dkrajzew
// Output of the number of build detectors added
//
// Revision 1.2  2002/04/15 07:05:35  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.1  2002/03/15 09:32:23  traffic
// Handling of map logics removed; Catching of invalid logics implemented
//
// Revision 2.0  2002/02/14 14:43:21  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:40  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:04  traffic
// moved from netbuild
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <sstream>
#include "NLContainer.h"
#include <microsim/MSLane.h>
#include <microsim/MSSourceLane.h>
#include <microsim/MSInternalLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSRouteLoader.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSRoute.h>
#include <microsim/MSRouteLoaderControl.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/traffic_lights/MSExtendedTrafficLightLogic.h>
#include "NLEdgeControlBuilder.h"
#include "NLJunctionControlBuilder.h"
#include "NLNetBuilder.h"
#include "NLSucceedingLaneBuilder.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/convert/TplConvert.h>
#include <sumo_only/SUMOFrame.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/gfx/GfxConvHelper.h>

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NLContainer::NLContainer(NLEdgeControlBuilder &edgeBuilder,
                         NLJunctionControlBuilder &junctionBuilder)
    :
    myEdgeControlBuilder(edgeBuilder),
    myJunctionControlBuilder(junctionBuilder),
    m_pSLB(new NLSucceedingLaneBuilder()),
    noEdges(0),
    noLanes(0),
    noJunctions(0),
    noVehicles(0),
    noVehicleTypes(0),
    noRoutes(0),
    noDetectors(0)
{
}


NLContainer::~NLContainer()
{
    delete m_pSLB;
}




// counting of data
void
NLContainer::incEdges()
{
    noEdges++;
}


void
NLContainer::incLanes()
{
    noLanes++;
}


void
NLContainer::incJunctions()
{
    noJunctions++;
}


void
NLContainer::incVehicles()
{
    noVehicles++;
}


void
NLContainer::incVehicleTypes()
{
    noVehicleTypes++;
}


void
NLContainer::incRoutes()
{
    noRoutes++;
}

void
NLContainer::incDetectors()
{
    noRoutes++;
}

void
NLContainer::addKey(string key) {
    m_LogicKeys.insert(LogicKeyCont::value_type(key, 0));
}


void
NLContainer::setEdgeNumber(size_t noEdges)
{
    // preallocate the storage for the edges
    myEdgeControlBuilder.prepare(noEdges);
}


void
NLContainer::setNodeNumber(size_t noNodes)
{
    // preallocate the storage for the edges
    myJunctionControlBuilder.prepare(noEdges);
}




// interface to use the edge control builder
void
NLContainer::addEdge(const string &id)
{
    try {
        myEdgeControlBuilder.addEdge(id);
    } catch (XMLIdAlreadyUsedException &e) {
        throw e;
    }
}


void
NLContainer::chooseEdge(const string &id, const std::string &func)
{
    // get the type
    MSEdge::EdgeBasicFunction funcEnum = MSEdge::EDGEFUNCTION_UNKNOWN;
    if(func=="normal") {
        funcEnum = MSEdge::EDGEFUNCTION_NORMAL;
    }
    if(func=="source") {
        funcEnum = MSEdge::EDGEFUNCTION_SOURCE;
    }
    if(func=="sink") {
        funcEnum = MSEdge::EDGEFUNCTION_SINK;
    }
    if(func=="internal") {
        funcEnum = MSEdge::EDGEFUNCTION_INTERNAL;
    }
    if(funcEnum<0) {
        throw XMLIdNotKnownException("purpose", func);
    }
    //
    try {
        myEdgeControlBuilder.chooseEdge(id, funcEnum);
    } catch (XMLIdNotKnownException &e) {
        throw e;
    }
    // continuation
    myCurrentID = id;
}


void
NLContainer::addLaneShape(const Position2DVector &shape)
{
    myShape = shape;
}


void
NLContainer::addLane(const string &id, const bool isDepartLane,
                      const float maxSpeed, const float length,
                      const float changeUrge)
{
    myID = id;
    myLaneIsDepart = isDepartLane;
    myCurrentMaxSpeed = maxSpeed;
    myCurrentLength = length;
    myCurrentChangeUrge = changeUrge;
}

/*
void
NLContainer::addLane(const string &id, const bool isDepartLane,
                     const float maxSpeed, const float length,
                     const float changeUrge)
{
    MSLane *lane =
        myEdgeControlBuilder.addLane(getNet(), id, maxSpeed, length, isDepartLane, myShape);
    if(!MSLane::dictionary(id, lane))
        throw XMLIdAlreadyUsedException("Lanes", id);
}
*/

void
NLContainer::closeLane()
{
    MSLane *lane =
        myEdgeControlBuilder.addLane(
            getNet(), myID, myCurrentMaxSpeed, myCurrentLength,
            myLaneIsDepart, myShape);
    // insert the lane into the lane-dictionary, checking
    if(!MSLane::dictionary(myID, lane)) {
        throw XMLIdAlreadyUsedException("Lanes", myID);
    }
}

void
NLContainer::closeLanes()
{
    myEdgeControlBuilder.closeLanes();
}


void
NLContainer::addPoly(const std::string &name, const std::string &type,
                     const std::string &color)
{
    RGBColor col;
    try {
       col = GfxConvHelper::parseColor(color);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("The color definition for polygon '") + name +
            string("' is not given, using default."));
        col = RGBColor(1, 1, 0);
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(
            string("The color definition for polygon '") + name +
            string("' is malicious."));
        return;
    }
    if(!MSNet::getInstance()->addPoly(name, type, col)) {
        MsgHandler::getErrorInstance()->inform(
            string("Duplicate polygon '") + name +
            string("' occured."));
    }
}


void
NLContainer::openAllowedEdge(const string &id)
{
    MSEdge *edge = MSEdge::dictionary(id);
    if(edge==0) {
        throw XMLIdNotKnownException("edge", id);
    }
    myEdgeControlBuilder.openAllowedEdge(edge);
    // continuation
    StringVector pred;
    if(myContinuations.find(id)!=myContinuations.end()) {
        pred = myContinuations[id];
    }
    pred.push_back(myCurrentID);
    myContinuations[id] = pred;
}


void
NLContainer::addAllowed(const string &id)
{
    MSLane *lane = MSLane::dictionary(id);
    if(lane==0) {
        throw XMLIdNotKnownException("lane", id);
    }
    try {
        myEdgeControlBuilder.addAllowed(lane);
    } catch (XMLInvalidChildException &e) {
        throw e;
    }
}


void
NLContainer::closeAllowedEdge()
{
    myEdgeControlBuilder.closeAllowedEdge();
}


void
NLContainer::closeEdge()
{
    myEdgeControlBuilder.closeEdge();
}







/// interfaces for the building of succeeding lanes
void
NLContainer::openSuccLane(const string &laneId)
{
    m_pSLB->openSuccLane(laneId);
}


void
NLContainer::addSuccLane(bool yield, const string &laneId,
                         const std::string &viaID,
                         MSLink::LinkDirection dir, MSLink::LinkState state,
                         bool internalEnd,
                         const std::string &tlid, size_t linkNo)
{
    m_pSLB->addSuccLane(yield, laneId, viaID, dir, state, internalEnd,
        tlid, linkNo);
}


void
NLContainer::closeSuccLane()
{
    m_pSLB->closeSuccLane();
}


std::string
NLContainer::getSuccingLaneName() const
{
    return m_pSLB->getSuccingLaneName();
}




// interfaces to the junction builder
void
NLContainer::openJunction(const string &id, const string &key, string type,
                          double x, double y)
{
    myJunctionControlBuilder.openJunction(id, key, type, x, y);
}


void
NLContainer::addIncomingLane(const string &id)
{
    MSLane *lane = MSLane::dictionary(id);
    if(lane==0) {
        throw XMLIdNotKnownException("lane", id);
    }
    myJunctionControlBuilder.addIncomingLane(lane);
}

void
NLContainer::addInternalLane(const string &id)
{
    MSLane *lane = MSLane::dictionary(id);
    if(lane==0) {
        throw XMLIdNotKnownException("lane", id);
    }
    myJunctionControlBuilder.addInternalLane(lane);
}

void
NLContainer::closeJunction()
{
    try {
        myJunctionControlBuilder.closeJunction();
    } catch (XMLIdAlreadyUsedException &e) {
        throw e;
    } catch (XMLIdNotKnownException &e) {
        throw e;
    }
}


// end of operations
MSNet *
NLContainer::buildMSNet(NLDetectorBuilder &db, const OptionsCont &options)
{
    closeJunctions(db);
    MSEdgeControl *edges = myEdgeControlBuilder.build();
    MSJunctionControl *junctions = myJunctionControlBuilder.build();
    MSRouteLoaderControl *routeLoaders = buildRouteLoaderControl(options);
    MSTLLogicControl *tlc = new MSTLLogicControl(getTLLogicVector());
    std::vector<OutputDevice*> streams = SUMOFrame::buildStreams(options);
    MSNet::init( "", edges, junctions, routeLoaders, tlc,
        !options.getBool("no-duration-log"),
        streams,
        options.getIntVector("dump-intervals"),
        options.getString("dump-basename"));
    return MSNet::getInstance();
}


void
NLContainer::closeJunctions(NLDetectorBuilder &db)
{
    for(TLLogicInitInfoMap::iterator i=myJunctions2PostLoadInit.begin(); i!=myJunctions2PostLoadInit.end(); i++) {
        (*i).first->init(
            db, (*i).second.first, myContinuations, (*i).second.second);
    }
}


void
NLContainer::addJunctionInitInfo(MSExtendedTrafficLightLogic *key,
                                 const LaneVector &lv, double det_offset)
{
    if(myJunctions2PostLoadInit.find(key)!=myJunctions2PostLoadInit.end()) {
        throw 1; // !!!
    }
    myJunctions2PostLoadInit[key] = TLInitInfo(lv, det_offset);
}


MSRouteLoaderControl *
NLContainer::buildRouteLoaderControl(const OptionsCont &oc)
{
    // build the loaders
    MSRouteLoaderControl::LoaderVector loaders;
    // check whether a list is existing
    if(oc.isSet("r")&&oc.getInt("route-steps")>0) {
        // extract the list
        StringTokenizer st(oc.getString("r"), ';');
        // check whether all files can be opened
        bool ok = true;
        while(st.hasNext()) {
            string name = st.next();
            if(!FileHelpers::exists(name)) {
                MsgHandler::getErrorInstance()->inform(
                    string("The route file '") + name
                    + string("' does not exist."));
                ok = false;
            }
        }
        if(!ok) {
            throw ProcessError();
        }
        // open files for reading
        st.reinit();
        while(st.hasNext()) {
            string file = st.next();
            loaders.push_back(buildRouteLoader(file));
/*                new MSRouteLoader(
                    *(MSNet::getInstance()),
                    new MSRouteHandler(file, false)));*/
        }
    }
    // build the route control
    return new MSRouteLoaderControl(getNet(), oc.getInt("s"), loaders);
}


MSRouteLoader *
NLContainer::buildRouteLoader(const std::string &file)
{
    return new MSRouteLoader(
        *(MSNet::getInstance()),
        new MSRouteHandler(file, false));
}


MSNet &
NLContainer::getNet()
{
    return *(MSNet::getInstance());
}


const std::vector<MSLane*> &
NLContainer::getIncomingLanes() const
{
    return myJunctionControlBuilder.getIncomingLanes();
}


void
NLContainer::initIncomingLanes()
{
    myJunctionControlBuilder.initIncomingLanes();
}

void
NLContainer::addTLLogic(MSTrafficLightLogic *logic)
{
    myLogics[logic->id()] = logic;
}


MSTrafficLightLogic*
NLContainer::getTLLogic(const std::string &id) const
{
    std::map<std::string, MSTrafficLightLogic*>::const_iterator i =
        myLogics.find(id);
    if(i==myLogics.end()) {
        return 0;
    }
    return (*i).second;
}


std::vector<MSTrafficLightLogic*>
NLContainer::getTLLogicVector() const
{
    std::vector<MSTrafficLightLogic*> ret;
    ret.reserve(myLogics.size());
    std::map<std::string, MSTrafficLightLogic*>::const_iterator i;
    for(i=myLogics.begin(); i!=myLogics.end(); i++) {
        ret.push_back((*i).second);
    }
    return ret;
}


const NLContainer::SSVMap &
NLContainer::getLaneConts() const
{
    return myContinuations;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
//

