/****************************************************************************/
/// @file    METriggeredCalibrator.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id: METriggeredCalibrator.cpp 247 2007-07-19 09:55:56Z behr_mi $
///
// Calibrates the flow on a segment to a specified one
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <utils/helpers/WrappingCommand.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSEventControl.h>
#include "MELoop.h"
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include "METriggeredCalibrator.h"
#include <utils/xml/XMLSubSys.h>
#include <utils/common/TplConvert.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/helpers/RandomDistributor.h>
#include "MESegment.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
int beginTime_2;

/// !!! "normale" Geschwindigkeit setzen, wenn kein Datum vorhanden


METriggeredCalibrator::METriggeredCalibrator(const std::string &id,
        MSNet &net,
        MESegment *edge,
        const std::string &routesFile,
        const std::string &aXMLFilename)
        : MSTrigger(id), SUMOSAXHandler(aXMLFilename),
        mySegment(edge), myRunningID(0),
        myAggregatedPassedQ(0), //myAggregatedPassedV(0),
        myWishedQ(0), myWishedV(edge->getMaxSpeed()), myWishedDensity(0),
        myDefaultSpeed(edge->getMaxSpeed()), blaDelay(0)
{
    m_pActiveRoute = new MSEdgeVector();
    bool running = true; // !!!
    // read the description
    try {
        myParser = XMLSubSys::getSAXReader(*this);
        if (routesFile.length()!=0) {
            myParser->parse(routesFile.c_str());
        }
        myParser->parse(aXMLFilename.c_str());
    } catch (SAXException &e) {
        MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(e.getMessage()));
        running = false;
    } catch (XMLException &e) {
        MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(e.getMessage()));
        running = false;
    }
    if (running) {
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
            new WrappingCommand<METriggeredCalibrator>(this, &METriggeredCalibrator::execute),
            MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
    }
    beginTime_2 = MSNet::getInstance()->getCurrentTimeStep();
    mySegment->addDetectorData(this, 1, 0);
}


METriggeredCalibrator::~METriggeredCalibrator() throw()
{
    delete myParser;
    for (std::vector<MSVehicle*>::iterator i=myCarPool.begin(); i!=myCarPool.end(); ++i) {
        delete(*i);
    }
    delete m_pActiveRoute;
}


void
METriggeredCalibrator::myStartElement(SumoXMLTag element,
                                      const Attributes &attrs) throw(ProcessError)
{
    if (element==SUMO_TAG_ROUTE_INTERVAL||element==SUMO_TAG_VTYPE_INTERVAL) {
        myCurrentIntervalBegin = getIntSecure(attrs, SUMO_ATTR_BEGIN, -1);
        myCurrentIntervalEnd = getIntSecure(attrs, SUMO_ATTR_END, -1);
    }
    if (element==SUMO_TAG_ROUTE_DIST||element==SUMO_TAG_ROUTEDISTELEM) {
        SUMOReal prob = -1;
        try {
            prob = getFloatSecure(attrs, SUMO_ATTR_PROB, -1);
        } catch (NumberFormatException) {
            MsgHandler::getErrorInstance()->inform("False probability (invalid number) while parsing '" + myID + "'.");
            return;
        }
        if (prob<=0) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing '" + myID + "'.");
            return;
        }
        // get the id
        string id;
        try {
            id = getString(attrs, SUMO_ATTR_ID);
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform("Error in description: missing id of a route-object.");
            return;
        }
        MSRoute *route = MSRoute::dictionary(id);
        if (route==0) {
            MsgHandler::getErrorInstance()->inform("Error in description: unknown route-object '" + id + "'.");
            return;
        }
        if (route->end()!=route->begin()+1) { // !!! false routes patch for soccer
            myCurrentRouteDist.add(prob, route);
        }
    }

    if (element==SUMO_TAG_VTYPE_DIST) {
        SUMOReal prob = -1;
        try {
            prob = getFloatSecure(attrs, SUMO_ATTR_PROB, -1);
        } catch (NumberFormatException) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing calibrator '" + getID() + "' (" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + ").");
            return;
        }
        if (prob<=0) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing calibrator '" + getID() + "' (" + toString(prob) + ").");
            return;
        }
        // get the id
        string id;
        try {
            id = getString(attrs, SUMO_ATTR_ID);
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform("Error in description: missing id of a vtype-object.");
            return;
        }
        if (id.find("LKW")!=string::npos) {
            // !!!
            return;
        }
        MSVehicleType *vtype = MSNet::getInstance()->getVehicleControl().getVType(id);
        if (vtype==0) {
            MsgHandler::getErrorInstance()->inform("Error in description: unknown vtype-object '" + id + "'.");
            return;
        }
        myCurrentVTypeDist.add(prob, vtype);
    }

    if (element==SUMO_TAG_STATE_INTERVAL) {
        ShouldBe sb;
        try {
            sb.v = getFloatSecure(attrs, SUMO_ATTR_V, -1);
        } catch (NumberFormatException) {
            //!!!MsgHandler::getErrorInstance()->inform("False speed while parsing '" + myID + "'.");
            return;
        }
        try {
            sb.q = getFloatSecure(attrs, SUMO_ATTR_Q, -1);
        } catch (NumberFormatException) {
            //!!!MsgHandler::getErrorInstance()->inform("False flow while parsing '" + myID + "'.");
            return;
        }
        sb.begin = getIntSecure(attrs, SUMO_ATTR_BEGIN, -1);
        sb.end = getIntSecure(attrs, SUMO_ATTR_END, -1);
        sb.beenDone = false;
        myIntervals.push_back(sb);
    }

    if (element==SUMO_TAG_ROUTE/*&&myGlobalRoutes.getOverallProb()<10*/) {
        // get the id
        string id;
        try {
            id = getString(attrs, SUMO_ATTR_ID);
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform("Error in description: missing id of a route-object.");
            return;
        }
        m_ActiveId = id;
    }

}


void
METriggeredCalibrator::myCharacters(SumoXMLTag element,
                                    const std::string &chars) throw(ProcessError)
{
    if (element==SUMO_TAG_ROUTE/*&&myGlobalRoutes.getOverallProb()<10*/) {
        StringTokenizer st(chars);
        if (st.size()==0) {
            MsgHandler::getErrorInstance()->inform("Empty route (" + m_ActiveId +")");
            return;
        }
        MSEdge *edge = 0;
        while (st.hasNext()) {
            string set = st.next();
            edge = MSEdge::dictionary(set);
            // check whether the edge exists
            if (edge==0) {
                MsgHandler::getErrorInstance()->inform("The edge '" + set +"' within route '" + m_ActiveId +"' is not known."
                                                           + "\n The route can not be build.");
                throw ProcessError();
            }
            m_pActiveRoute->push_back(edge);
        }
    }
}


void
METriggeredCalibrator::myEndElement(SumoXMLTag element) throw(ProcessError)
{
    if (element==SUMO_TAG_VTYPE_INTERVAL) {
        VehTypeDist dist;
        dist.begin = myCurrentIntervalBegin;
        dist.end = myCurrentIntervalEnd;
        dist.typeDist = myCurrentVTypeDist;
        myVehicleTypes.push_back(dist);
    }
    if (element==SUMO_TAG_ROUTE_INTERVAL) {
        RouteDist dist;
        dist.begin = myCurrentIntervalBegin;
        dist.end = myCurrentIntervalEnd;
        dist.routeDist = myCurrentRouteDist;
        myRoutes.push_back(dist);
    }
    if (element==SUMO_TAG_ROUTE/*&&myGlobalRoutes.getOverallProb()<10*/) {
        int size = m_pActiveRoute->size();
        if (size==0) {
            MsgHandler::getErrorInstance()->inform("The list belonging to the route with the id '"
				                       + m_ActiveId + "' is empty.");
        }
        MSRoute *route = new MSRoute(m_ActiveId, *m_pActiveRoute, true);
        m_pActiveRoute->clear();
        if (!MSRoute::dictionary(m_ActiveId, route)) {
            delete route;
            if (!MSGlobals::gStateLoaded) {
                MsgHandler::getErrorInstance()->inform("Another route with the id " + m_ActiveId + " exists.");
            } else {
                route = MSRoute::dictionary(m_ActiveId);
            }
        }
        if (route->inFurtherUse()) {
            myGlobalRoutes.add(1, route);
        }
    }
}


bool
METriggeredCalibrator::hasCurrentShouldBe(SUMOTime time) const
{
    std::vector<ShouldBe>::const_iterator i = myIntervals.begin();
    while (i!=myIntervals.end()) {
        if ((*i).begin<=time && (*i).end>=time) {
            return true;
        }
        i++;
    }
    return false;
}


const METriggeredCalibrator::ShouldBe &
METriggeredCalibrator::getCurrentShouldBe(SUMOTime time) const
{
    std::vector<ShouldBe>::const_iterator i = myIntervals.begin();
    while (i!=myIntervals.end()) {
        if ((*i).begin<=time && (*i).end>=time) {
            return *i;
        }
        i++;
    }
    throw 1;
}


SUMOReal
getNormedDensity(MESegment *s)
{
    const MSEdge *edge = s->getEdge();
    s = MSGlobals::gMesoNet->getSegmentForEdge(edge);
    size_t noCars = 0;
    SUMOReal length = 0;
    while (s!=0) {
        noCars += s->noCars();
        length += s->getLength();
        s = s->getNextSegment();
    }
    return (SUMOReal) noCars *(SUMOReal) 1000. / (SUMOReal) length;
}


bool
tryEmit(MESegment *s, MSVehicle *vehicle, MEState state)
{
//    while(s!=0) {
    bool insertToNet = false;
    if (s->get_state()==state&&s->initialise2(vehicle, 0, MSNet::getInstance()->getCurrentTimeStep(), insertToNet)) {
        if (insertToNet) {//s->noCars()==1) {
            MSGlobals::gMesoNet->addCar(vehicle);
            vehicle->inserted = true;
        }
        vehicle->onDepart();
        MSNet::getInstance()->getVehicleControl().vehiclesEmitted(1);
        if (!MSNet::getInstance()->getVehicleControl().addVehicle(vehicle->getID(), vehicle)) {
            //MsgHandler::getErrorInstance()->inform("Calibrator " + myID + " could not insert vehicle " + id);
            throw ProcessError();
        }
        return true;
    }
//        s = s->getNextSegment();
//    }
    return false;
}

SUMOTime
METriggeredCalibrator::execute(SUMOTime currentTime)
{
    // get current simulation values (valid for the last simulation second)
    MSLaneMeanDataValues &meanData = mySegment->getDetectorData(this, currentTime);
    SUMOReal isQ = meanData.nSamples;
    SUMOReal isV = meanData.speedSum / (SUMOReal) meanData.nSamples;
    if (meanData.nSamples==0) {
        if (mySegment->noCars()!=0) {
            if (mySegment->getNextSegment()!=0&&mySegment->getNextSegment()->get_state()==Jam) {
                isV = 1;
            } else {
                isV = mySegment->getMeanSpeed();
            }
        } else {
            isV = mySegment->getMaxSpeed();
        }
    }
    myAggregatedPassedQ += isQ;
    mySegment->flushDetectorData(this, currentTime);

    // check whether an adaptation value exists
    if (!hasCurrentShouldBe(MSNet::getInstance()->getCurrentTimeStep())) {
        // if not, reset adaptation values
        myAggregatedPassedQ = 0;
        MSGlobals::gMesoNet->setSpeed(mySegment, myDefaultSpeed);
        const MSEdge *edge = mySegment->getEdge();
        MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge(edge);
        while (first!=0) {
            MSGlobals::gMesoNet->setSpeed(first, myDefaultSpeed);
            first = first->getNextSegment();
        }
        return 1;
    }

    // get the adaptation values
    int running = 0;
    const ShouldBe &sb1 =
        getCurrentShouldBe(MSNet::getInstance()->getCurrentTimeStep());
    if (!sb1.beenDone) {
        myAggregatedPassedQ = isQ;
        sb1.beenDone = true;
        myWishedQ = sb1.q;
        myWishedV = sb1.v;
        if (myWishedV<=0) {
            myWishedV = myDefaultSpeed;
        }
    }

    if (mySegment->getEdge()->getID()=="-51114865") {
        int bla = 0;
    }

    myWishedDensity = (SUMOReal) myWishedQ /
                      (SUMOReal) myWishedV * (SUMOReal) 3.6;

    // compute the q that should have passed the calibrator within the time
    //  from begin of the interval
    SUMOReal currentlyWishedQ = myWishedQ * (SUMOReal)(sb1.end - sb1.begin + 1) / (SUMOReal) 3600.;

    currentlyWishedQ =
        currentlyWishedQ /* (SUMOReal) (sb1.end - sb1.begin + 1) * */ * (SUMOReal)(MSNet::getInstance()->getCurrentTimeStep() - sb1.begin + 1);
    const MSEdge *edge = mySegment->getEdge();
    MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge(edge);
    while (myWishedV!=0&&first!=0) {
        MSGlobals::gMesoNet->setSpeed(first, myWishedV);
        first = first->getNextSegment();
    }

    // patch densities
    if (myWishedQ<0) {
        // leave if input density is marked as invalid
        return 1;
    }
    if (blaDelay>0) {
        blaDelay--;
        return 1;
    }
    // patch only if not equal :-)
    size_t removed = 0;
    int inserted = 0;
    if (fabs(currentlyWishedQ-/*mySegment->*/myAggregatedPassedQ)>=.5) {
        if (currentlyWishedQ>/*mySegment->*/myAggregatedPassedQ) {
            // ok, some vehicles must be added
            //while(currentlyWishedDensity>myAggregatedPassedQ+inserted) {
            while (currentlyWishedQ>/*mySegment->*/myAggregatedPassedQ+inserted) {
                // get a route
                MSRoute *route = getRandomRoute(sb1);
                if (route==0&&myGlobalRoutes.getOverallProb()>0) {
                    route = myGlobalRoutes.get();
                }
                if (route==0) {
                    MsgHandler::getErrorInstance()->inform("Calibrator " + myID + " has a flow, but no routes at time " + toString(MSNet::getInstance()->getCurrentTimeStep()));
                    return 0;
                }

                // get a vehicle type
                MSVehicleType *vtype = getRandomVehicleType(sb1);
                if (vtype==0) {
                    MsgHandler::getErrorInstance()->inform("Calibrator " + myID + " has a flow, but no vtype at time " + toString(MSNet::getInstance()->getCurrentTimeStep()));
                    return 0;
                }

                // build the vehicle
                string id = "inserted_" + myID
                            + "_time_" + toString(MSNet::getInstance()->getCurrentTimeStep())
                            + "_" + toString(myRunningID++)
                            + "_" + toString(beginTime_2);
                ;
                MSVehicle *vehicle =
                    MSNet::getInstance()->getVehicleControl().buildVehicle(id,
                            route, MSNet::getInstance()->getCurrentTimeStep(), vtype, -1, -1);
                vehicle->update_segment(mySegment);
                vehicle->update_tEvent((SUMOReal) MSNet::getInstance()->getCurrentTimeStep());
                // move vehicle forward when the route does not begin at the calibrator's edge
                const MSEdge *myedge = mySegment->getEdge();
                while (vehicle->getEdge()!=myedge) {
                    const MSEdge *nextEdge = vehicle->succEdge(1);
                    // let the vehicle move to the next edge
                    vehicle->proceedVirtualReturnWhetherEnded(nextEdge);
                }
                // insert vehicle into the net
                MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(mySegment->getEdge());
                int state = 0;
                if (!tryEmit(mySegment, vehicle, Free)) {
                    if (!tryEmit(mySegment, vehicle, Jam)) {

                        delete vehicle;
                        break;
                    }
                }
                inserted++;
            }
        } else {
            bool ok = true;
            while (currentlyWishedQ</*mySegment->*/myAggregatedPassedQ-removed&&ok) {
                MEVehicle *erased = mySegment->eraseRandomCar2();
                if (erased!=0) {
                    removed++;
                    MSGlobals::gMesoNet->eraseCar(erased);
                } else {
                    ok = false;
                }
            }
        }
    }
    myAggregatedPassedQ -= removed;
    if (mySegment->getEdge()->getID()=="-51114865"&&currentTime==sb1.end) {
        WRITE_WARNING("------------" + toString(currentTime) + ":" + toString(currentlyWishedQ-(myAggregatedPassedQ+inserted-removed)));
    }

    if (fabs((currentlyWishedQ-(myAggregatedPassedQ+inserted-removed)))>1) {
        WRITE_WARNING(mySegment->getEdge()->getID() + ":" + toString(inserted) + ", " + toString(removed) + ", " + toString(currentlyWishedQ-(myAggregatedPassedQ+inserted-removed)));
    }
    return 1;
}


MSRoute *
METriggeredCalibrator::getRandomRoute(const ShouldBe &sb)
{
    for (std::vector<RouteDist>::const_iterator i=myRoutes.begin(); i!=myRoutes.end(); ++i) {
        if ((*i).begin<=sb.end&&(*i).end>=sb.begin) {
            if ((*i).routeDist.getOverallProb()!=0) {
                return (*i).routeDist.get();
            }
        }
    }
    return 0;
}


MSVehicleType *
METriggeredCalibrator::getRandomVehicleType(const ShouldBe &sb)
{
    for (std::vector<VehTypeDist>::const_iterator i=myVehicleTypes.begin(); i!=myVehicleTypes.end(); ++i) {
        if ((*i).begin<=sb.end&&(*i).end>=sb.begin) {
            return (*i).typeDist.get();
        }
    }
    return 0;
}


/****************************************************************************/

