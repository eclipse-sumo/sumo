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
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.9  2003/06/05 10:21:56  roessel
// In ctor: set m_EventControl = 0. First guess because of modified MSEventControl. Needs additional discussion.
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
// possibility of retrival of the preinitialised net during loading implemented for trigger handling
//
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
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
// errors occured while building larger nets and adaption of new netconverting methods debugged
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <sax2/XMLReaderFactory.hpp>
#include <sstream>
#include "NLContainer.h"
#include <microsim/MSLane.h>
#include <microsim/MSSourceLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSRouteLoader.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSRoute.h>
#include <microsim/MSRouteLoaderControl.h>
#include "NLEdgeControlBuilder.h"
#include "NLJunctionControlBuilder.h"
#include "NLNetBuilder.h"
#include "NLSucceedingLaneBuilder.h"
#include <utils/common/SLogging.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/options/OptionsCont.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NLContainer::NLContainer(NLEdgeControlBuilder * const edgeBuilder)
    :
    m_pECB(edgeBuilder),
    m_pJCB(0),
    m_pSLB(new NLSucceedingLaneBuilder()),
    m_pDetectors(0),
    noEdges(0),
    noLanes(0),
    noJunctions(0),
    noVehicles(0),
    noVehicleTypes(0),
    noRoutes(0),
    noDetectors(0)
{
//     m_EventControl = MSEventControl::getInstance();
    m_EventControl = 0;
    // ... the storage for the detectors
    m_pDetectors = new MSNet::DetectorCont();
    m_pJCB = new NLJunctionControlBuilder(this);
}


NLContainer::~NLContainer()
{
    delete m_pECB;
    delete m_pJCB;
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
    m_pECB->prepare(noEdges);
}


void
NLContainer::setNodeNumber(size_t noNodes)
{
    // preallocate the storage for the edges
    m_pJCB->prepare(noEdges);
}




// interface to use the edge control builder
void
NLContainer::addEdge(const string &id)
{
    try {
        m_pECB->addEdge(id);
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
    if(funcEnum<0) {
        throw XMLIdNotKnownException("purpose", func);
    }
    //
    try {
        m_pECB->chooseEdge(id, funcEnum);
    } catch (XMLIdNotKnownException &e) {
        throw e;
    }
}


void
NLContainer::addLane(const string &id, const bool isDepartLane,
                     const float maxSpeed, const float length,
                     const float changeUrge)
{
    MSLane *lane =
        m_pECB->addLane(getNet(), id, maxSpeed, length, isDepartLane);
    if(!MSLane::dictionary(id, lane))
        throw XMLIdAlreadyUsedException("Lanes", id);
}


void
NLContainer::closeLanes()
{
    m_pECB->closeLanes();
}


void
NLContainer::openAllowedEdge(const string &id)
{
    MSEdge *edge = MSEdge::dictionary(id);
    if(edge==0) {
        throw XMLIdNotKnownException("edge", id);
    }
    m_pECB->openAllowedEdge(edge);
}


void
NLContainer::addAllowed(const string &id)
{
    MSLane *lane = MSLane::dictionary(id);
    if(lane==0) {
        throw XMLIdNotKnownException("lane", id);
    }
    try {
        m_pECB->addAllowed(lane);
    } catch (XMLInvalidChildException &e) {
        throw e;
    }
}


void
NLContainer::closeAllowedEdge()
{
    m_pECB->closeAllowedEdge();
}


void
NLContainer::closeEdge()
{
    m_pECB->closeEdge();
}







/// interfaces for the building of succeeding lanes
void
NLContainer::openSuccLane(const string &laneId)
{
    m_pSLB->openSuccLane(laneId);
}


void
NLContainer::addSuccLane(bool yield, const string &laneId)
{
    m_pSLB->addSuccLane(yield, laneId);
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
    m_pJCB->openJunction(id, key, type, x, y);
}


void
NLContainer::addInLane(const string &id)
{
    MSLane *lane = MSLane::dictionary(id);
    if(lane==0) {
        throw XMLIdNotKnownException("lane", id);
    }
    m_pJCB->addInLane(lane);
}

void
NLContainer::closeJunction()
{
    try {
        m_pJCB->closeJunction();
    } catch (XMLIdAlreadyUsedException &e) {
        throw e;
    } catch (XMLIdNotKnownException &e) {
        throw e;
    }
}


// ----- handling of detectors
void
NLContainer::addDetector(MSDetector *detector)
{
    m_pDetectors->push_back(detector);
}


void
NLContainer::addDetectors(MSNet::DetectorCont detectors)
{
    for(MSNet::DetectorCont::iterator i=detectors.begin(); i!=detectors.end(); i++) {
        m_pDetectors->push_back(*i);
    }
}



MSEventControl &
NLContainer::getEventControl() const
{
    return *m_EventControl;
}


// end of operations
MSNet *
NLContainer::buildMSNet(const OptionsCont &options)
{
    MSEdgeControl *edges = m_pECB->build();
    MSJunctionControl *junctions = m_pJCB->build();
    MSRouteLoaderControl *routeLoaders = buildRouteLoaderControl(options);
    MSNet::init( m_Id, edges, junctions,
                 m_pDetectors,
                 routeLoaders);
    return MSNet::getInstance();
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
                SErrorHandler::add(
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
            loaders.push_back(
                new MSRouteLoader(oc.getBool("v"), oc.getBool("w"), st.next(),
                *(MSNet::getInstance())));
        }
    }
    // build the route control
    return new MSRouteLoaderControl(getNet(), oc.getInt("s"), loaders);
}


MSNet::DetectorCont *
NLContainer::getDetectors() {
    return m_pDetectors;
}

MSNet &
NLContainer::getNet()
{
    return *(MSNet::getInstance());
}


const std::vector<MSLane*> &
NLContainer::getInLanes() const
{
    return m_pJCB->getInLanes();
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLContainer.icc"
//#endif

// Local Variables:
// mode:C++
// End:


